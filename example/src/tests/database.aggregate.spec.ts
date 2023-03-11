import Chance from 'chance';
import {
	open,
	QuickSQLiteConnection,
} from 'react-native-quick-sqlite';
import {beforeEach, afterEach, pluckFromJson, describe, it} from './MochaRNAdapter';
import chai from 'chai';

let expect = chai.expect;
const chance = new Chance();
let db: QuickSQLiteConnection;

export function registerAggregateTests() {
	let get: (SQL: any, ...args: any[]) => any;
	let all: (SQL: any, ...args: any[]) => any[] | undefined;
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
		db.close();
		db.delete();
	})

	// it('should throw an exception if step.length or inverse.length is invalid', function () {
	// 	const length = (x: any) => Object.defineProperty(() => {}, 'length', { value: x });
	//
	// 	expect(() => db.aggregate('a', { step: length(undefined) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('b', { step: length(null) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('c', { step: length('2') })).to.throw(TypeError);
	// 	expect(() => db.aggregate('d', { step: length(NaN) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('e', { step: length(Infinity) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('f', { step: length(2.000000001) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('g', { step: length(-0.000000001) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('h', { step: length(-2) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('i', { step: length(100.000000001) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('j', { step: length(102) })).to.throw(RangeError);
	// 	expect(() => db.aggregate('aa', { step: () => {}, inverse: length(undefined) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('bb', { step: () => {}, inverse: length(null) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('cc', { step: () => {}, inverse: length('2') })).to.throw(TypeError);
	// 	expect(() => db.aggregate('dd', { step: () => {}, inverse: length(NaN) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('ee', { step: () => {}, inverse: length(Infinity) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('ff', { step: () => {}, inverse: length(2.000000001) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('gg', { step: () => {}, inverse: length(-0.000000001) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('hh', { step: () => {}, inverse: length(-2) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('ii', { step: () => {}, inverse: length(100.000000001) })).to.throw(TypeError);
	// 	expect(() => db.aggregate('jj', { step: () => {}, inverse: length(102) })).to.throw(RangeError);
	// });
	// it('should register an aggregate function and return the database object', function () {
	// 	const length = (x: any) => Object.defineProperty(() => {}, 'length', { value: x });
	// 	expect(db.aggregate('a', { step: () => {} })).to.equal(db);
	// 	expect(db.aggregate('b', { step: function x() {} })).to.equal(db);
	// 	expect(db.aggregate('c', { step: length(1) })).to.equal(db);
	// 	expect(db.aggregate('d', { step: length(101) })).to.equal(db);
	// });
	// it('should enable the registered aggregate function to be executed from SQL', function () {
	// 	// numbers
	// 	db.aggregate('a', { step: (ctx, a, b) => a * b + ctx });
	// 	expect(db.execute('a(_, ?) FROM ints', [2])).to.equal(150);
	//
	// 	// strings
	// 	db.aggregate('b', { step: (ctx, a, b) => a + b + ctx });
	// 	expect(get('b(_, ?) FROM texts', '!')).to.equal('g!f!e!d!c!b!a!null');
	//
	// 	// starting value is null
	// 	db.aggregate('c', { step: (ctx, x) => null });
	// 	db.aggregate('d', { step: (ctx, x) => ctx });
	// 	db.aggregate('e', { step: (ctx, x) => {} });
	// 	expect(get('c(_) FROM ints')).to.equal(null);
	// 	expect(get('d(_) FROM ints')).to.equal(null);
	// 	expect(get('e(_) FROM ints')).to.equal(null);
	//
	// 	// buffers
	// 	db.aggregate('f', { step: (ctx, x) => x });
	// 	// @ts-ignore
	// 	const input = Buffer.alloc(8).fill(0xdd);
	// 	const output = get('f(?)', input);
	// 	expect(input).to.not.equal(output);
	// 	expect(input.equals(output)).to.be.true;
	// 	// @ts-ignore
	// 	expect(output.equals(Buffer.alloc(8).fill(0xdd))).to.be.true;
	//
	// 	// zero arguments
	// 	db.aggregate('g', { step: (ctx) => 'z' + ctx });
	// 	db.aggregate('h', { step: (ctx) => 12 });
	// 	db.aggregate('i', { step: () => 44 });
	// 	expect(get('g()')).to.equal('znull');
	// 	expect(get('h()')).to.equal(12);
	// 	expect(get('i()')).to.equal(44);
	// 	expect(get('g() FROM empty')).to.equal(null);
	// 	expect(get('h() FROM empty')).to.equal(null);
	// 	expect(get('i() FROM empty')).to.equal(null);
	// 	expect(get('g() FROM ints')).to.equal('zzzzzzznull');
	// 	expect(get('h() FROM ints')).to.equal(12);
	// 	expect(get('i() FROM ints')).to.equal(44);
	// 	expect(get('g(*) FROM ints')).to.equal('zzzzzzznull');
	// 	expect(get('h(*) FROM ints')).to.equal(12);
	// 	expect(get('i(*) FROM ints')).to.equal(44);
	// });
	// it('should use a strict number of arguments by default', function () {
	// 	db.aggregate('agg', { step: (ctx, a, b) => {} });
	// 	expect(() => get('agg()')).to.throw();
	// 	expect(() => get('agg(?)', 4)).to.throw();
	// 	expect(() => get('agg(?, ?, ?)', 4, 8, 3)).to.throw();
	// 	get('agg(?, ?)', 4, 8);
	// });
	// // it('should accept a "varargs" option', function () {
	// // 	const step = (ctx: any, ...args: any[]) => args.reduce((a, b) => a * b, 1) + ctx;
	// // 	Object.defineProperty(step, 'length', { value: '-2' });
	// // 	db.aggregate('agg', { varargs: true, step });
	// // 	expect(get('agg()')).to.equal(1);
	// // 	expect(get('agg(?)', 7)).to.equal(7);
	// // 	expect(get('agg(?, ?)', 4, 8)).to.equal(32);
	// // 	expect(get('agg(?, ?, ?, ?, ?, ?)', 2, 3, 4, 5, 6, 7)).to.equal(5040);
	// // });
	// it('should accept an optional start value', function () {
	// 	db.aggregate('a', { start: 10000, step: (ctx, a, b) => a * b + ++ctx });
	// 	expect(get('a(_, ?) FROM ints', 2)).to.equal(10157);
	// 	expect(get('a(_, ?) FROM ints', 2)).to.equal(10157);
	//
	// 	db.aggregate('b', { start: { foo: 1000 }, step: (ctx, a, b) => a * b + (ctx.foo ? ++ctx.foo : ++ctx) });
	// 	expect(get('b(_, ?) FROM ints', 2)).to.equal(1157);
	// 	expect(get('b(_, ?) FROM ints', 2)).to.equal(1158);
	//
	// 	let ranOnce = false;
	// 	db.aggregate('c', { start: undefined, step: (ctx, a, b) => {
	// 		if (ranOnce) expect(ctx).to.be.NaN;
	// 		else expect(ctx).to.be.undefined;
	// 		ranOnce = true;
	// 		return a * b + ++ctx;
	// 	} });
	// 	expect(get('c(_, ?) FROM ints', 2)).to.equal(null);
	// 	expect(ranOnce).to.be.true;
	// 	ranOnce = false;
	// 	expect(get('c(_, ?) FROM ints', 2)).to.equal(null);
	// 	expect(ranOnce).to.be.true;
	// });




	////////////////////
	// it('should accept an optional start() function', function () {
	// 	let start = 10000;
	// 	db.aggregate('a', { start: () => start++, step: (ctx, a, b) => a * b + ctx });
	// 	expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(10150);
	// 	expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(10151);
	// 	expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(10152);
	//
	// 	db.aggregate('b', { start: () => ({ foo: start-- }), step: (ctx, a, b) => a * b + (ctx.foo || ctx) });
	// 	expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(10153);
	// 	expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(10152);
	// 	expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(10151);
	//
	// 	let ranOnce = false;
	// 	db.aggregate('c', { start: () => undefined, step: (ctx, a, b) => {
	// 		if (ranOnce) expect(ctx).to.be.NaN;
	// 		else expect(ctx).to.be.undefined;
	// 		ranOnce = true;
	// 		return a * b + ++ctx;
	// 	} });
	// 	expect(get('c(_, ?) as c FROM ints', 2)?.['c']).to.equal(null);
	// 	expect(ranOnce).to.be.true;
	// 	ranOnce = false;
	// 	expect(get('c(_, ?) as c FROM ints', 2)?.['c']).to.equal(null);
	// 	expect(ranOnce).to.be.true;
	// });
	// it('should not change the aggregate value when step() returns undefined', function () {
	// 	db.aggregate('a', { start: 10000, step: (ctx, a, b) => a === 11 ? undefined : a * b + ctx });
	// 	expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(10128);
	// 	db.aggregate('b', { start: () => 1000, step: (ctx, a, b) => {
	// 		console.log(ctx, a, b);
	// 		} });
	// 	expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(1000);
	// 	db.aggregate('c', { start: () => 1000, step: (ctx, a, b) => null });
	// 	expect(get('c(_, ?) as c FROM ints', 2)?.['c']).to.equal(null);
	// });
	//
	// it('should accept a result() transformer function', function () {
	// 	db.aggregate('a', {
	// 		start: 10000,
	// 		step: (ctx, a, b) => a * b + ctx,
	// 		result: ctx => ctx / 2,
	// 	});
	// 	expect(get('a(_, ?) as a FROM ints', 2)?.['a']).to.equal(5075);
	// 	db.aggregate('b', {
	// 		start: () => ({ foo: 1000 }),
	// 		step: (ctx, a, b) => { ctx.foo += a * b; },
	// 		result: ctx => ctx.foo,
	// 	});
	// 	expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(1150);
	// 	expect(get('b(_, ?) as b FROM ints', 2)?.['b']).to.equal(1150); // should play well when ran multiple times
	// 	db.aggregate('c', {
	// 		start: () => ({ foo: 1000 }),
	// 		step: (ctx, a, b) => { ctx.foo += 1; },
	// 		result: ctx => ctx.foo,
	// 	});
	// 	expect(get('c(_, ?) as c FROM empty', 2)?.['c']).to.equal(1000);
	// });
	// it('should interpret undefined as null within a result() function', function () {
	// 	db.aggregate('agg', {
	// 		start: 10000,
	// 		step: (ctx, a, b) => a * b + ctx,
	// 		result: () => {},
	// 	});
	// 	expect(get('agg(_, ?) as agg FROM ints', 2)?.['agg']).to.equal(null);
	// });
	it('should accept an inverse() function to support aggregate window functions', function () {
		// db.aggregate('agg', {
		// 	start: () => 10000,
		// 	step: (ctx, a, b) => a * b + ctx,
		// });
		// expect(() => all('agg(_, ?) as agg OVER win FROM ints', 2))
		// 	.to.throw();
		db.aggregate('wn', {
			start: () => 10000,
			step: (ctx, a, b) => a * b + ctx,
			inverse: (ctx, a, b) => ctx - a * b,
		});
		console.log(all('wn(_, ?) OVER win as wn FROM ints', 2));
		expect(all('wn(_, ?) OVER win as wn FROM ints', 2))
			.to.deep.equal([{ wn: 10016 }, { wn: 10030}, { wn: 10046}, { wn: 10062}, { wn: 10082}, { wn: 10098}, { wn: 10072 }]);
	});
	// it('should not change the aggregate value when inverse() returns undefined', function () {
	// 	db.aggregate('a', {
	// 		start: () => 10000,
	// 		step: (ctx, a, b) => a * b + ctx,
	// 		inverse: (ctx, a, b) => a === 11 ? undefined : ctx - a * b,
	// 	});
	// 	expect(all('a(_, ?) OVER win FROM ints', 2))
	// 		.to.deep.equal([10016, 10030, 10046, 10062, 10082, 10120, 10094]);
	// 	db.aggregate('b', {
	// 		start: () => 10000,
	// 		step: (ctx, a, b) => ctx ? a * b + ctx : null,
	// 		inverse: (ctx, a, b) => null,
	// 	});
	// 	expect(all('b(_, ?) OVER win FROM ints', 2))
	// 		.to.deep.equal([10016, 10030, null, null, null, null, null]);
	// });
	// it('should potentially call result() multiple times for window functions', function () {
	// 	let startCount = 0;
	// 	let stepCount = 0;
	// 	let inverseCount = 0;
	// 	let resultCount = 0;
	// 	db.aggregate('wn', {
	// 		start: () => {
	// 			startCount += 1;
	// 			return { foo: 1000, results: 0 };
	// 		},
	// 		step: (ctx, a, b) => {
	// 			stepCount += 1;
	// 			ctx.foo += a * b;
	// 		},
	// 		inverse: (ctx, a, b) => {
	// 			inverseCount += 1;
	// 			ctx.foo -= a * b;
	// 		},
	// 		result: (ctx) => {
	// 			resultCount += 1;
	// 			return ctx.foo + ctx.results++ * 10000;
	// 		},
	// 	});
	// 	expect(all('wn(_, ?) OVER win FROM ints', 2))
	// 		.to.deep.equal([1016, 11030, 21046, 31062, 41082, 51098, 61072]);
	// 	expect(startCount).to.equal(1);
	// 	expect(stepCount).to.equal(7);
	// 	expect(inverseCount).to.equal(5);
	// 	expect(resultCount).to.equal(7);
	// 	expect(all('wn(_, ?) OVER win FROM ints', 2)) // should play well when ran multiple times
	// 		.to.deep.equal([1016, 11030, 21046, 31062, 41082, 51098, 61072]);
	// 	expect(startCount).to.equal(2);
	// 	expect(stepCount).to.equal(14);
	// 	expect(inverseCount).to.equal(10);
	// 	expect(resultCount).to.equal(14);
	// 	expect(all('wn(_, ?) OVER win FROM empty', 2))
	// 		.to.deep.equal([]);
	// 	expect(startCount).to.equal(2);
	// 	expect(stepCount).to.equal(14);
	// 	expect(inverseCount).to.equal(10);
	// 	expect(resultCount).to.equal(14);
	// });
	// it('should infer argument count from the greater of step() and inverse()', function () {
	// 	db.aggregate('a', {
	// 		start: () => 10000,
	// 		step: (ctx, a) => a + ctx,
	// 		inverse: (ctx, a, b) => ctx - a,
	// 	});
	// 	expect(all('a(_, ?) OVER win FROM ints', 2))
	// 		.to.deep.equal([10008, 10015, 10023, 10031, 10041, 10049, 10036]);
	// 	expect(() => all('a(_) OVER win FROM ints'))
	// 		.to.throw();
	// 	db.aggregate('b', {
	// 		start: () => 10000,
	// 		step: (ctx, a, b) => a + ctx,
	// 		inverse: (ctx, a) => ctx - a,
	// 	});
	// 	expect(all('b(_, ?) OVER win FROM ints', 2))
	// 		.to.deep.equal([10008, 10015, 10023, 10031, 10041, 10049, 10036]);
	// 	expect(() => all('b(_) OVER win FROM ints'))
	// 		.to.throw();
	// 	db.aggregate('c', {
	// 		// @ts-ignore
	// 		start: (a, b, c, d, e) => 10000,
	// 		step: () => {},
	// 		inverse: (ctx, a) => --ctx,
	// 		result: (ctx, a, b, c, d, e) => ctx,
	// 	});
	// 	expect(all('c(_) OVER win FROM ints'))
	// 		.to.deep.equal([10000, 10000, 9999, 9998, 9997, 9996, 9995]);
	// 	expect(() => all('c() OVER win FROM ints'))
	// 		.to.throw();
	// 	expect(() => all('c(*) OVER win FROM ints'))
	// 		.to.throw();
	// 	expect(() => all('c(_, ?) OVER win FROM ints', 2))
	// 		.to.throw();
	// });
	//
	// it('should cause the aggregate to throw when returning an invalid value', function () {
	// 	db.aggregate('a', {
	// 		start: () => ({}),
	// 		step: () => ({}),
	// 		inverse: () => ({}),
	// 		result: () => 42,
	// 	});
	// 	db.aggregate('b', {
	// 		start: () => 42,
	// 		step: () => 42,
	// 		inverse: () => 42,
	// 		result: () => ({}),
	// 	});
	// 	db.aggregate('c', {
	// 		step: () => {},
	// 		result: () => 42,
	// 	});
	// 	db.aggregate('d', {
	// 		step: () => {},
	// 		result: () => ({}),
	// 	});
	// 	expect(all('a(*) OVER win FROM ints')).to.deep.equal([42, 42, 42, 42, 42, 42, 42]);
	// 	expect(() => all('b(*) OVER win FROM ints')).to.throw(TypeError);
	// 	expect(get('c(*) FROM ints')).to.equal(42);
	// 	expect(get('c(*) FROM empty')).to.equal(42);
	// 	expect(() => get('d(*) FROM ints')).to.throw(TypeError);
	// 	expect(() => get('d(*) FROM empty')).to.throw(TypeError);
	// });
	//
	// it('should be able to register multiple aggregates with the same name', function () {
	// 	db.aggregate('agg', { step: (ctx) => 0 });
	// 	db.aggregate('agg', { step: (ctx, a) => 1 });
	// 	db.aggregate('agg', { step: (ctx, a, b) => 2 });
	// 	db.aggregate('agg', { step: (ctx, a, b, c) => 3, inverse: () => {} });
	// 	db.aggregate('agg', { step: (ctx, a, b, c, d) => 4 });
	// 	expect(get('agg()')).to.equal(0);
	// 	expect(get('agg(555)')).to.equal(1);
	// 	expect(get('agg(555, 555)')).to.equal(2);
	// 	expect(get('agg(555, 555, 555)')).to.equal(3);
	// 	expect(get('agg(555, 555, 555, 555)')).to.equal(4);
	// 	db.aggregate('agg', { step: (ctx, a, b) => 'foo', inverse: () => {} });
	// 	db.aggregate('agg', { step: (ctx, a, b, c) => 'bar' });
	// 	expect(get('agg()')).to.equal(0);
	// 	expect(get('agg(555)')).to.equal(1);
	// 	expect(get('agg(555, 555)')).to.equal('foo');
	// 	expect(get('agg(555, 555, 555)')).to.equal('bar');
	// 	expect(get('agg(555, 555, 555, 555)')).to.equal(4);
	// });
	// it('should not be able to affect bound buffers mid-query', function () {
	// 	// @ts-ignore
	// 	const input = Buffer.alloc(1024 * 8).fill(0xbb);
	// 	let startCalled = false;
	// 	let stepCalled = false;
	// 	db.aggregate('agg', {
	// 		start: () => {
	// 			startCalled = true;
	// 			input[0] = 2;
	// 		},
	// 		step: () => {
	// 			stepCalled = true;
	// 			input[0] = 2;
	// 		},
	// 	});
	// 	const output = get('?, agg(*) FROM ints', input);
	// 	expect(startCalled).to.be.true;
	// 	expect(stepCalled).to.be.true;
	// 	// @ts-ignore
	// 	expect(output.equals(Buffer.alloc(1024 * 8).fill(0xbb))).to.be.true;
	// });
	// describe('should propagate exceptions', function () {
	// 	const exceptions = [ new TypeError('foobar'), new Error('baz'), { yup: 'ok' }, 'foobarbazqux', '', null, 123.4];
	// 	const expectError = (exception: any, fn: () => any) => {
	// 		try { fn(); } catch (ex) {
	// 			expect(ex).to.equal(exception);
	// 			return;
	// 		}
	// 		throw new TypeError('Expected aggregate to throw an exception');
	// 	};
	//
	// 	it('thrown in the start() function', function () {
	// 		exceptions.forEach((exception, index) => {
	// 			const calls: string[] = [];
	// 			db.aggregate(`wn${index}`, {
	// 				start: () => { calls.push('a'); throw exception; },
	// 				step: () => { calls.push('b'); },
	// 				inverse: () => { calls.push('c'); },
	// 				result: () => { calls.push('d'); },
	// 			});
	// 			expectError(exception, () => get(`wn${index}() FROM empty`));
	// 			expect(calls.splice(0)).to.deep.equal(['a']);
	// 			expectError(exception, () => get(`wn${index}() FROM ints`));
	// 			expect(calls.splice(0)).to.deep.equal(['a']);
	// 			expectError(exception, () => all(`wn${index}() OVER win FROM ints`));
	// 			expect(calls.splice(0)).to.deep.equal(['a']);
	// 		});
	// 	});
	// 	it('thrown in the step() function', function () {
	// 		exceptions.forEach((exception, index) => {
	// 			const calls: string[] = [];
	// 			db.aggregate(`wn${index}`, {
	// 				start: () => { calls.push('a'); },
	// 				step: () => { calls.push('b'); throw exception; },
	// 				inverse: () => { calls.push('c'); },
	// 				result: () => { calls.push('d'); },
	// 			});
	// 			expect(get(`wn${index}() FROM empty`)).to.equal(null);
	// 			expect(calls.splice(0)).to.deep.equal(['a', 'd']);
	// 			expectError(exception, () => get(`wn${index}() FROM ints`));
	// 			expect(calls.splice(0)).to.deep.equal(['a', 'b']);
	// 			expectError(exception, () => all(`wn${index}() OVER win FROM ints`));
	// 			expect(calls.splice(0)).to.deep.equal(['a', 'b']);
	// 		});
	// 	});
	// 	it('thrown in the inverse() function', function () {
	// 		exceptions.forEach((exception, index) => {
	// 			const calls: string[] = [];
	// 			db.aggregate(`wn${index}`, {
	// 				start: () => { calls.push('a'); },
	// 				step: () => { calls.push('b'); },
	// 				inverse: () => { calls.push('c'); throw exception; },
	// 				result: () => { calls.push('d'); },
	// 			});
	// 			expect(get(`wn${index}() FROM empty`)).to.equal(null);
	// 			expect(calls.splice(0)).to.deep.equal(['a', 'd']);
	// 			expect(get(`wn${index}() FROM ints`)).to.equal(null);
	// 			expect(calls.splice(0)).to.deep.equal(['a', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'd']);
	// 			expectError(exception, () => all(`wn${index}() OVER win FROM ints`));
	// 			expect(calls.length).to.be.above(2);
	// 			expect(calls.indexOf('c')).to.equal(calls.length - 1);
	// 		});
	// 	});
	// 	it('thrown in the result() function', function () {
	// 		exceptions.forEach((exception, index) => {
	// 			const calls: string[] = [];
	// 			db.aggregate(`wn${index}`, {
	// 				start: () => { calls.push('a'); },
	// 				step: () => { calls.push('b'); },
	// 				inverse: () => { calls.push('c'); },
	// 				result: () => { calls.push('d'); throw exception; },
	// 			});
	// 			expectError(exception, () => get(`wn${index}() FROM empty`));
	// 			expect(calls.splice(0)).to.deep.equal(['a', 'd']);
	// 			expectError(exception, () => get(`wn${index}() FROM ints`));
	// 			expect(calls.splice(0)).to.deep.equal(['a', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'd']);
	// 			expectError(exception, () => all(`wn${index}() OVER win FROM ints`));
	// 			expect(calls.splice(0)).to.deep.equal(['a', 'b', 'b', 'd']);
	// 		});
	// 	});
	// 	it('thrown due to returning an invalid value', function () {
	// 		const calls: string[] = [];
	// 		db.aggregate('wn', {
	// 			start: () => { calls.push('a'); },
	// 			step: () => { calls.push('b'); },
	// 			inverse: () => { calls.push('c'); },
	// 			result: () => { calls.push('d'); return {}; },
	// 		});
	// 		expect(() => get('wn() FROM empty')).to.throw(TypeError);
	// 		expect(calls.splice(0)).to.deep.equal(['a', 'd']);
	// 		expect(() => get('wn() FROM ints')).to.throw(TypeError);;
	// 		expect(calls.splice(0)).to.deep.equal(['a', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'd']);
	// 		expect(() => all('wn() OVER win FROM ints')).to.throw(TypeError);;
	// 		expect(calls.splice(0)).to.deep.equal(['a', 'b', 'b', 'd']);
	// 	});
	// });
};
