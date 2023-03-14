import Chance from 'chance';
import {
	open,
	QuickSQLiteConnection,
} from 'react-native-quick-sqlite';
import {beforeEach, afterEach, describe, it} from './MochaRNAdapter';
import chai from 'chai';

let expect = chai.expect;
let db: QuickSQLiteConnection;

export function registerAggregateTests() {
	let get: (SQL: any, ...args: any[]) => any;
	let all: (SQL: any, ...args: any[]) => any[] | undefined;

	describe ("Aggregate queries", () => {
		beforeEach(() => {
			try {
				db = open({
					name: 'test',
				});

				db.execute('DROP TABLE IF EXISTS User;');
				db.execute(
					'CREATE TABLE User ( id INT PRIMARY KEY, name TEXT NOT NULL, age INT, networth REAL) STRICT;',
				);
				db.execute('CREATE TABLE IF NOT EXISTS empty (_)');
				db.execute('CREATE TABLE IF NOT EXISTS ints (_)');
				db.execute('CREATE TABLE IF NOT EXISTS texts (_)');
				db.execute('INSERT INTO ints VALUES (?), (?), (?), (?), (?), (?), (?)', [3, 5, 7, 11, 13, 17, 19]);
				db.execute('INSERT INTO texts VALUES (?), (?), (?), (?), (?), (?), (?)', ['a', 'b', 'c', 'd', 'e', 'f', 'g']);
				get = (SQL, ...args) => db.execute(`SELECT ${SQL}`, args)?.rows?._array[0];
				all = (SQL, ...args) => db.execute(`SELECT ${SQL} WINDOW win AS (ORDER BY rowid ROWS BETWEEN 1 PRECEDING AND 1 FOLLOWING) ORDER BY rowid`, args)?.rows?._array;
			} catch (e) {
				console.warn('error on before each', e);
			}
		});

		afterEach(() => {
			if (!db) return;

			db.close();
			db.delete();
		})

		it('should accept an optional start() function', function () {
			let start = 10000;
			db.aggregate('a', {start: () => start++, step: (ctx, a, b) => a * b + ctx});
			expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(10150);
			expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(10151);
			expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(10152);

			db.aggregate('b', {start: () => ({foo: start--}), step: (ctx, a, b) => a * b + (ctx.foo || ctx)});
			expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(10153);
			expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(10152);
			expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(10151);

			let ranOnce = false;
			db.aggregate('c', {
				start: () => undefined, step: (ctx, a, b) => {
					if (ranOnce) expect(ctx).to.be.NaN;
					else expect(ctx).to.be.undefined;
					ranOnce = true;
					return a * b + ++ctx;
				}
			});
			expect(get('c(_, ?) as c FROM ints', 2)?.['c']).to.equal(null);
			expect(ranOnce).to.be.true;
			ranOnce = false;
			expect(get('c(_, ?) as c FROM ints', 2)?.['c']).to.equal(null);
			expect(ranOnce).to.be.true;
		});
		it('should not change the aggregate value when step() returns undefined', function () {
			db.aggregate('a', {start: 10000, step: (ctx, a, b) => a === 11 ? undefined : a * b + ctx});
			expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(10128);
			db.aggregate('b', {
				start: () => 1000, step: (ctx, a, b) => {
					console.log(ctx, a, b);
				}
			});
			expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(1000);
			db.aggregate('c', {start: () => 1000, step: (ctx, a, b) => null});
			expect(get('c(_, ?) as c FROM ints', 2)?.['c']).to.equal(null);
		});

		it('should accept a result() transformer function', function () {
			db.aggregate('a', {
				start: 10000,
				step: (ctx, a, b) => a * b + ctx,
				result: ctx => ctx / 2,
			});
			expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(5075);
			db.aggregate('b', {
				start: () => ({foo: 1000}),
				step: (ctx, a, b) => {
					ctx.foo += a * b;
				},
				result: ctx => ctx.foo,
			});
			expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(1150);
			expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(1150); // should play well when ran multiple times
			db.aggregate('c', {
				start: () => ({foo: 1000}),
				step: (ctx, a, b) => {
					ctx.foo += 1;
				},
				result: ctx => ctx.foo,
			});
			expect(get('c(_, ?) as c FROM empty', 2)?.['c']).to.equal(1000);
		});
		it('should interpret undefined as null within a result() function', function () {
			db.aggregate('agg', {
				start: 10000,
				step: (ctx, a, b) => a * b + ctx,
				result: () => {
				},
			});
			expect(get('agg(_, ?) as agg FROM ints', 2)?.['agg']).to.equal(null);
		});
		it('should accept an inverse() function to support aggregate window functions', function () {
			db.aggregate('agg', {
				start: () => 10000,
				step: (ctx, a, b) => a * b + ctx,
			});
			expect(() => all('agg(_, ?) as agg OVER win FROM ints', 2))
				.to.throw();
			db.aggregate('wn', {
				start: () => 10000,
				step: (ctx, a, b) => a * b + ctx,
				inverse: (ctx, a, b) => ctx - a * b,
			});
			console.log(all('wn(_, ?) OVER win as wn FROM ints', 2));
			expect(all('wn(_, ?) OVER win as wn FROM ints', 2))
				.to.deep.equal([{wn: 10016}, {wn: 10030}, {wn: 10046}, {wn: 10062}, {wn: 10082}, {wn: 10098}, {wn: 10072}]);
		});
		it('should not change the aggregate value when inverse() returns undefined', function () {
			db.aggregate('a', {
				start: () => 10000,
				step: (ctx, a, b) => a * b + ctx,
				inverse: (ctx, a, b) => a === 11 ? undefined : ctx - a * b,
			});
			expect(all('a(_, ?) OVER win as wn FROM ints', 2))
				.to.deep.equal([{wn: 10016}, {wn: 10030}, {wn: 10046}, {wn: 10062}, {wn: 10082}, {wn: 10120}, {wn: 10094}]);
			db.aggregate('b', {
				start: () => 10000,
				step: (ctx, a, b) => ctx ? a * b + ctx : null,
				inverse: (ctx, a, b) => null,
			});
			expect(all('b(_, ?) OVER win as wn FROM ints', 2))
				.to.deep.equal([{wn: 10016}, {wn: 10030}, {wn: null}, {wn: null}, {wn: null}, {wn: null}, {wn: null}]);
		});
		it('should potentially call result() multiple times for window functions', function () {
			let startCount = 0;
			let stepCount = 0;
			let inverseCount = 0;
			let resultCount = 0;
			db.aggregate('wn', {
				start: () => {
					startCount += 1;
					return {foo: 1000, results: 0};
				},
				step: (ctx, a, b) => {
					stepCount += 1;
					ctx.foo += a * b;
				},
				inverse: (ctx, a, b) => {
					inverseCount += 1;
					ctx.foo -= a * b;
				},
				result: (ctx) => {
					resultCount += 1;
					return ctx.foo + ctx.results++ * 10000;
				},
			});
			expect(all('wn(_, ?) OVER win as wn FROM ints', 2))
				.to.deep.equal([{wn: 1016}, {wn: 11030}, {wn: 21046}, {wn: 31062}, {wn: 41082}, {wn: 51098}, {wn: 61072}]);
			expect(startCount).to.equal(1);
			expect(stepCount).to.equal(7);
			expect(inverseCount).to.equal(5);
			expect(resultCount).to.equal(7);
			expect(all('wn(_, ?) OVER win as wn FROM ints', 2)) // should play well when ran multiple times
				.to.deep.equal([{wn: 1016}, {wn: 11030}, {wn: 21046}, {wn: 31062}, {wn: 41082}, {wn: 51098}, {wn: 61072}]);
			expect(startCount).to.equal(2);
			expect(stepCount).to.equal(14);
			expect(inverseCount).to.equal(10);
			expect(resultCount).to.equal(14);
			expect(all('wn(_, ?) OVER win as wn FROM empty', 2))
				.to.deep.equal([]);
			expect(startCount).to.equal(2);
			expect(stepCount).to.equal(14);
			expect(inverseCount).to.equal(10);
			expect(resultCount).to.equal(14);
		});
		it('should infer argument count from the greater of step() and inverse()', function () {
			db.aggregate('a', {
				start: () => 10000,
				step: (ctx, a) => a + ctx,
				inverse: (ctx, a, b) => ctx - a,
			});
			expect(all('a(_, ?) OVER win as wn FROM ints', 2))
				.to.deep.equal([{wn: 10008}, {wn: 10015}, {wn: 10023}, {wn: 10031}, {wn: 10041}, {wn: 10049}, {wn: 10036}]);
			expect(() => all('a(_) OVER win as wn FROM ints'))
				.to.throw();
			db.aggregate('b', {
				start: () => 10000,
				step: (ctx, a, b) => a + ctx,
				inverse: (ctx, a) => ctx - a,
			});
			expect(all('b(_, ?) OVER win as wn FROM ints', 2))
				.to.deep.equal([{wn: 10008}, {wn: 10015}, {wn: 10023}, {wn: 10031}, {wn: 10041}, {wn: 10049}, {wn: 10036}]);
			expect(() => all('b(_) OVER win as wn FROM ints'))
				.to.throw();
			db.aggregate('c', {
				// @ts-ignore
				start: (a, b, c, d, e) => 10000,
				step: () => {
				},
				inverse: (ctx, a) => --ctx,
				result: (ctx, a, b, c, d, e) => ctx,
			});
			expect(all('c(_) OVER win as wn FROM ints'))
				.to.deep.equal([{wn: 10000}, {wn: 10000}, {wn: 9999}, {wn: 9998}, {wn: 9997}, {wn: 9996}, {wn: 9995}]);
			expect(() => all('c() OVER win FROM ints'))
				.to.throw();
			expect(() => all('c(*) OVER win FROM ints'))
				.to.throw();
			expect(() => all('c(_, ?) OVER win FROM ints', 2))
				.to.throw();
		});

		it('should cause the aggregate to throw when returning an invalid value', function () {
			db.aggregate('a', {
				start: () => Object(),
				step: () => Object(),
				inverse: () => Object(),
				result: () => 42,
			});
			db.aggregate('b', {
				start: () => 42,
				step: () => 42,
				inverse: () => 42,
				result: () => Object(),
			});
			db.aggregate('c', {
				step: () => {
				},
				result: () => 42,
			});
			db.aggregate('d', {
				step: () => {
				},
				result: () => Object(),
			});

			expect(all('a(*) OVER win as wn FROM ints')).to.deep.equal([{wn: 42}, {wn: 42}, {wn: 42}, {wn: 42}, {wn: 42}, {wn: 42}, {wn: 42}]);
			expect(() => all('b(*) OVER win FROM ints')).to.throw();
			expect(get('c(*) as wn FROM ints')).to.deep.equal({wn: 42});
			expect(get('c(*) as wn FROM empty')).to.deep.equal({wn: 42});
			expect(() => get('d(*) FROM ints')).to.throw();
			expect(() => get('d(*) FROM empty')).to.throw();
		});

		it('should be able to register multiple aggregates with the same name', function () {
			db.aggregate('agg', {step: (ctx) => 0});
			db.aggregate('agg', {step: (ctx, a) => 1});
			db.aggregate('agg', {step: (ctx, a, b) => 2});
			db.aggregate('agg', {
				step: (ctx, a, b, c) => 3, inverse: () => {
				}
			});
			db.aggregate('agg', {step: (ctx, a, b, c, d) => 4});
			expect(get('agg() as a')).to.deep.equal({a: 0});
			expect(get('agg(555) as a')).to.deep.equal({a: 1});
			expect(get('agg(555, 555) as a')).to.deep.equal({a: 2});
			expect(get('agg(555, 555, 555) as a')).to.deep.equal({a: 3});
			expect(get('agg(555, 555, 555, 555) as a')).to.deep.equal({a: 4});
			db.aggregate('agg', {
				step: (ctx, a, b) => 'foo', inverse: () => {
				}
			});
			db.aggregate('agg', {step: (ctx, a, b, c) => 'bar'});
			expect(get('agg() as a')).to.deep.equal({a: 0});
			expect(get('agg(555) as a')).to.deep.equal({a: 1});
			expect(get('agg(555, 555) as a')).to.deep.equal({a: 'foo'});
			expect(get('agg(555, 555, 555) as a')).to.deep.equal({a: 'bar'});
			expect(get('agg(555, 555, 555, 555) as a')).to.deep.equal({a: 4});
		});
	});
};
