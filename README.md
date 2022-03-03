<h1 align="center">React Native Quick SQLite</h1>

<h3 align="center">The **fastest** SQLite implementation for react-native.</h3>

![Frame 2](https://user-images.githubusercontent.com/1634213/127499575-aed1d0e2-8a93-42ab-917e-badaab8916f6.png)

<div align="center">
  <pre align="center">
    <a href="https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch">Copy typeORM patch-package from example dir</a>
    npm i react-native-quick-sqlite typeorm
    npx pod-install
    <a href="https://dev.to/vinipachecov/setup-typeorm-with-react-native-50c4">Enable decorators and configure babel</a>
  </pre>
  <a align="center" href="https://github.com/ospfranco?tab=followers">
    <img src="https://img.shields.io/github/followers/ospfranco?label=Follow%20%40ospfranco&style=social" />
  </a>
  <br />
  <a align="center" href="https://twitter.com/ospfranco">
    <img src="https://img.shields.io/twitter/follow/ospfranco?label=Follow%20%40ospfranco&style=social" />
  </a>
</div>
<br />

Quick SQLite uses [JSI](https://formidable.com/blog/2019/jsi-jsc-part-2), removes all the overhead of intercommunication between JavaScript code and C++ code, making CRUDing entities from SQLite super fast!

Big ❤️ to [react-native-sqlite-storage](https://github.com/andpor/react-native-sqlite-storage) and [react-native-sqlite2](https://github.com/craftzdog/react-native-sqlite-2), this library also provides a WebSQL interface.

If you are using quick-sqlite in your app, please get in touch or open a PR with a modified README (with your company/product logo, would love to showcase you!

## GOTCHAS

- **It's not possible to use the browser debugger with JSI**, use [Flipper](https://github.com/facebook/flipper), on android Flipper also has an integrated database explorer you can use to debug your sqlite database, [you will have to configure your database path though](https://fbflipper.com/docs/setup/plugins/databases/).
  ![130516553-15c18d0f-65ad-44cf-8235-a4d6f41859e2](https://user-images.githubusercontent.com/1634213/130755919-7539d3dd-7d30-4234-9965-bfef2450ab0a.png)
- Your app will now include C++, you will need to install the NDK on your machine for android. (unless you know how to generate an AAR, feel free to open a PR)
- If you want to run the example project on android, you will have to change the paths on the android/CMakeLists.txt file, they are already there, just uncomment them.
- This library supports SQLite BLOBs which are mapped to JS ArrayBuffers, check out the sample project on how to use it
- Starting with version 2.0.0 the library no longer throws errors when an invalid statement is passed, but rather returns an object with a `status` enum property, where 0 signals a succesful execution and `1` an incorrect execution (this is to keep typeorm from exploding when an incorrect query is executed)
- This library cannot retrieve integers larger than 53 bits because it's not possible to represent such numbers in JavaScript. [Read here](https://github.com/ospfranco/react-native-quick-sqlite/issues/16#issuecomment-1018412991) for more information. 

## Use TypeORM

The recommended way to use this package is to use [TypeORM](https://github.com/typeorm/typeorm) with [patch-package](https://github.com/ds300/patch-package). TypeORM already has a sqlite-storage driver. In the `example` project on the `patch` folder you can a find a [patch for TypeORM](https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch), it basically just replaces all the `react-native-sqlite-storage` strings in TypeORM with `react-native-quick-sqlite`.

Follow the instructions to make TypeORM work with rn (enable decorators, configure babel, etc), then apply the patch via patch-package and you should be good to go.

## Low level API

It is also possible to directly execute SQL against the db:

```typescript
interface ISQLite {
  open: (dbName: string, location?: string) => any;
  close: (dbName: string) => any;
  executeSql: (
    dbName: string,
    query: string,
    params: any[] | undefined
  ) => QueryResult;
  executeSqlBatch: (
    dbName: string,
    commands: SQLBatchParams[]
  ) => BatchExecutionResult;
}
```

In your code

```typescript
// If you want to register the (globalThis) types for the low level API do an empty import
import 'react-native-quick-sqlite';

// `sqlite` is a globally registered object, so you can directly call it from anywhere in your javascript
// The methods `throw` when an execution error happens, so try/catch them
try {
  sqlite.open('myDatabase', 'databases');
} catch (e) {
  console.log(e); // [react-native-quick-sqlite]: Could not open database file: ERR XXX
}
```

Some query examples:
```typescript
let result = sqlite.executeSql('myDatabase', 'SELECT somevalue FROM sometable');
if(!result.status) { // result.status undefined or 0 === sucess
  for(let i = 0; i<result.rows.length; i++) {
    const row = result.rows.item(i);
    console.log(row.somevalue);
  }
}

result = sqlite.executeSql('myDatabase', 'UPDATE sometable set somecolumn = ? where somekey = ?', [0, 1]);
if(!result.status) { // result.status undefined or 0 === sucess
  console.log(`Update affected ${result.rowsAffected} rows`);
}
```

Bulkupdate allows transactional execution of a set of commands
```typescript
const commands = [
  ['CREATE TABLE TEST (id integer)'],
  ['INSERT INTO TABLE TEST (id) VALUES (?)', [1]]
  ['INSERT INTO TABLE TEST (id) VALUES (?)', [2]]
  ['INSERT INTO TABLE TEST (id) VALUES (?)', [[3], [4], [5], [6]]]
];
const result = sqlite.executeSqlBatch('myDatabase', commands);
if(!result.status) { // result.status undefined or 0 === sucess
  console.log(`Batch affected ${result.rowsAffected} rows`);
}
```

## JSI Cheatsheet

If you want to learn how to make your own JSI module and also get a reference guide for all things C++/JSI you can buy my [JSI/C++ Cheatsheet](http://ospfranco.gumroad.com/l/IeeIvl)

## License

react-native-quick-sqlite is licensed under MIT.
