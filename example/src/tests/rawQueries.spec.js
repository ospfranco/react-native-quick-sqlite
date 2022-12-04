"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.registerBaseTests = void 0;
const chance_1 = __importDefault(require("chance"));
const react_native_quick_sqlite_1 = require("react-native-quick-sqlite");
const MochaRNAdapter_1 = require("./MochaRNAdapter");
const chance = new chance_1.default();
let db;
function registerBaseTests() {
    (0, MochaRNAdapter_1.beforeEach)(() => {
        try {
            if (db) {
                db.close();
                db.delete();
            }
            db = (0, react_native_quick_sqlite_1.open)({
                name: 'test',
            });
            db.execute('DROP TABLE IF EXISTS User;');
            db.execute('CREATE TABLE User ( id INT PRIMARY KEY, name TEXT NOT NULL, age INT, networth REAL) STRICT;');
        }
        catch (e) {
            console.warn('error on before each', e);
        }
    });
    (0, MochaRNAdapter_1.describe)('Raw queries', () => {
        // it('Insert', async () => {
        //   const id = chance.integer();
        //   const name = chance.name();
        //   const age = chance.integer();
        //   const networth = chance.floating();
        //   const res = db.execute(
        //     'INSERT INTO "User" (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //     [id, name, age, networth]
        //   );
        //   expect(res.rowsAffected).to.equal(1);
        //   expect(res.insertId).to.equal(1);
        //   expect(res.metadata).to.eql([]);
        //   expect(res.rows._array).to.eql([]);
        //   expect(res.rows.length).to.equal(0);
        //   expect(res.rows.item).to.be.a('function');
        // });
        // it('Query without params', async () => {
        //   const id = chance.integer();
        //   const name = chance.name();
        //   const age = chance.integer();
        //   const networth = chance.floating();
        //   db.execute(
        //     'INSERT INTO User (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //     [id, name, age, networth]
        //   );
        //   const res = db.execute('SELECT * FROM User');
        //   expect(res.rowsAffected).to.equal(1);
        //   expect(res.insertId).to.equal(1);
        //   expect(res.rows._array).to.eql([
        //     {
        //       id,
        //       name,
        //       age,
        //       networth,
        //     },
        //   ]);
        // });
        // it('Query with params', async () => {
        //   const id = chance.integer();
        //   const name = chance.name();
        //   const age = chance.integer();
        //   const networth = chance.floating();
        //   db.execute(
        //     'INSERT INTO User (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //     [id, name, age, networth]
        //   );
        //   const res = db.execute('SELECT * FROM User WHERE id = ?', [id]);
        //   expect(res.rowsAffected).to.equal(1);
        //   expect(res.insertId).to.equal(1);
        //   expect(res.rows._array).to.eql([
        //     {
        //       id,
        //       name,
        //       age,
        //       networth,
        //     },
        //   ]);
        // });
        // it('Failed insert', async () => {
        //   const id = chance.string();
        //   const name = chance.name();
        //   const age = chance.string();
        //   const networth = chance.string();
        //   // expect(
        //   try {
        //     db.execute(
        //       'INSERT INTO User (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //       [id, name, age, networth]
        //     );
        //   } catch (e) {
        //     expect(typeof e).to.equal('object');
        //     expect(e.message).to.include(
        //       `cannot store TEXT value in INT column User.id`
        //     );
        //   }
        // });
        // it('Transaction, auto commit', (done) => {
        //   const id = chance.integer();
        //   const name = chance.name();
        //   const age = chance.integer();
        //   const networth = chance.floating();
        //   db.transaction((tx) => {
        //     const res = tx.execute(
        //       'INSERT INTO "User" (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //       [id, name, age, networth]
        //     );
        //     expect(res.rowsAffected).to.equal(1);
        //     expect(res.insertId).to.equal(1);
        //     expect(res.metadata).to.eql([]);
        //     expect(res.rows._array).to.eql([]);
        //     expect(res.rows.length).to.equal(0);
        //     expect(res.rows.item).to.be.a('function');
        //   });
        //   setTimeout(() => {
        //     const res = db.execute('SELECT * FROM User');
        //     expect(res.rows._array).to.eql([
        //       {
        //         id,
        //         name,
        //         age,
        //         networth,
        //       },
        //     ]);
        //     done();
        //   }, 200);
        // });
        // it('Transaction, manual commit', (done) => {
        //   const id = chance.integer();
        //   const name = chance.name();
        //   const age = chance.integer();
        //   const networth = chance.floating();
        //   db.transaction((tx) => {
        //     const res = tx.execute(
        //       'INSERT INTO "User" (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //       [id, name, age, networth]
        //     );
        //     expect(res.rowsAffected).to.equal(1);
        //     expect(res.insertId).to.equal(1);
        //     expect(res.metadata).to.eql([]);
        //     expect(res.rows._array).to.eql([]);
        //     expect(res.rows.length).to.equal(0);
        //     expect(res.rows.item).to.be.a('function');
        //     tx.commit();
        //   });
        //   setTimeout(() => {
        //     const res = db.execute('SELECT * FROM User');
        //     expect(res.rows._array).to.eql([
        //       {
        //         id,
        //         name,
        //         age,
        //         networth,
        //       },
        //     ]);
        //     done();
        //   }, 200);
        // });
        // it('Transaction, cannot execute after commit', (done) => {
        //   const id = chance.integer();
        //   const name = chance.name();
        //   const age = chance.integer();
        //   const networth = chance.floating();
        //   db.transaction((tx) => {
        //     const res = tx.execute(
        //       'INSERT INTO "User" (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //       [id, name, age, networth]
        //     );
        //     expect(res.rowsAffected).to.equal(1);
        //     expect(res.insertId).to.equal(1);
        //     expect(res.metadata).to.eql([]);
        //     expect(res.rows._array).to.eql([]);
        //     expect(res.rows.length).to.equal(0);
        //     expect(res.rows.item).to.be.a('function');
        //     tx.commit();
        //     try {
        //       tx.execute('SELECT * FROM "User"');
        //     } catch (e) {
        //       expect(!!e).to.equal(true);
        //     }
        //   });
        //   setTimeout(() => {
        //     const res = db.execute('SELECT * FROM User');
        //     expect(res.rows._array).to.eql([
        //       {
        //         id,
        //         name,
        //         age,
        //         networth,
        //       },
        //     ]);
        //     done();
        //   }, 1000);
        // });
        // it('Incorrect transaction, manual rollback', (done) => {
        //   const id = chance.string();
        //   const name = chance.name();
        //   const age = chance.integer();
        //   const networth = chance.floating();
        //   db.transaction((tx) => {
        //     try {
        //       tx.execute(
        //         'INSERT INTO "User" (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //         [id, name, age, networth]
        //       );
        //     } catch (e) {
        //       tx.rollback();
        //     }
        //   });
        //   setTimeout(() => {
        //     const res = db.execute('SELECT * FROM User');
        //     expect(res.rows._array).to.eql([]);
        //     done();
        //   }, 200);
        // });
        (0, MochaRNAdapter_1.it)('Incorrect transaction, auto rollback', done => {
            const id = chance.string();
            const name = chance.name();
            const age = chance.integer();
            const networth = chance.floating();
            try {
                db.transaction(tx => {
                    tx.execute('INSERT INTO "User" (id, name, age, networth) VALUES(?, ?, ?, ?)', [id, name, age, networth]);
                });
            }
            catch (e) {
                console.warn('catched error!', e);
                // const res = db.execute('SELECT * FROM User');
                // expect(res.rows._array).to.eql([]);
                // done();
            }
        });
        // it('Async transaction, auto commit', (done) => {
        //   const id = chance.integer();
        //   const name = chance.name();
        //   const age = chance.integer();
        //   const networth = chance.floating();
        //   db.transactionAsync(async (tx) => {
        //     const res = await tx.executeAsync(
        //       'INSERT INTO "User" (id, name, age, networth) VALUES(?, ?, ?, ?)',
        //       [id, name, age, networth]
        //     );
        //     expect(res.rowsAffected).to.equal(1);
        //     expect(res.insertId).to.equal(1);
        //     expect(res.metadata).to.eql([]);
        //     expect(res.rows._array).to.eql([]);
        //     expect(res.rows.length).to.equal(0);
        //     expect(res.rows.item).to.be.a('function');
        //   });
        //   setTimeout(() => {
        //     const res = db.execute('SELECT * FROM User');
        //     expect(res.rows._array).to.eql([
        //       {
        //         id,
        //         name,
        //         age,
        //         networth,
        //       },
        //     ]);
        //     done();
        //   }, 200);
        // });
    });
}
exports.registerBaseTests = registerBaseTests;
