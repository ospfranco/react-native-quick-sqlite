<h1 align="center">React Native Quick SQLite</h1>

<h3 align="center">Fast SQLite for react-native.</h3>

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

This library uses [JSI](https://formidable.com/blog/2019/jsi-jsc-part-2) to directly call C++ code from JS. It provides a low-level API to execute SQL queries, therefore I recommend you use it with TypeORM.

Inspired/compatible with [react-native-sqlite-storage](https://github.com/andpor/react-native-sqlite-storage) and [react-native-sqlite2](https://github.com/craftzdog/react-native-sqlite-2).

## Gotchas

- **Javascript cannot represent intergers larger than 53 bits**, be careful when loading data if it came from other systems. [Read more](https://github.com/ospfranco/react-native-quick-sqlite/issues/16#issuecomment-1018412991).
- **It's not possible to use a browser to debug a JSI app**, use [Flipper](https://github.com/facebook/flipper) (for android Flipper also has SQLite Database explorer).
- Your app will now include C++, you will need to install the NDK on your machine for android.
- This library supports SQLite BLOBs which are mapped to JS ArrayBuffers, check out the sample project on how to use it
- From version 2.0.0 onwards errors are no longer thrown on invalid SQL statements. The response contains a `status` number, `0` signals correct execution, `1` signals an error.
- From version 3.0.0 onwards no JS errors are thown, every operation returns an object with a `status` field.
- If you want to run the example project on android, you will have to change the paths on the android/CMakeLists.txt file, they are already there, just uncomment them.

## Use TypeORM

This package offers a low-level API to raw execute SQL queries. I strongly recommend to use [TypeORM](https://github.com/typeorm/typeorm) (with [patch-package](https://github.com/ds300/patch-package)). TypeORM already has a sqlite-storage driver. In the `example` project on the `patch` folder you can a find a [patch for TypeORM](https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch).

Follow the instructions to make TypeORM work with React Native (enable decorators, configure babel, etc), then apply the patch via patch-package and you should be good to go.

## API

It is also possible to directly execute SQL against the db:

```typescript
interface QueryResult {
  status: 0 | 1; // 0 for correct execution
  message: string; // if status === 1, here you will find error description
  rows: any[];
  insertId?: number;
}

interface BatchQueryResult {
  status?: 0 | 1;
  rowsAffected?: number;
  message?: string;
}

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
  ) => BatchQueryResult;
}
```

In your code

```typescript
import 'react-native-quick-sqlite';

// `sqlite` is a globally registered object, so you can directly call it from anywhere in your javascript
// the import on the top of the file only registers typescript types but it is not mandatory
const dbOpenResult = sqlite.open('myDatabase', 'databases');

// status === 1, operation failed
if (dbOpenResult.status) {
  console.error('Database could not be opened');
}
```

### Example queries

```typescript
let result = sqlite.executeSql('myDatabase', 'SELECT somevalue FROM sometable');
if (!result.status) {
  // result.status undefined or 0 === sucess
  for (let i = 0; i < result.rows.length; i++) {
    const row = result.rows.item(i);
    console.log(row.somevalue);
  }
}

result = sqlite.executeSql(
  'myDatabase',
  'UPDATE sometable set somecolumn = ? where somekey = ?',
  [0, 1]
);
if (!result.status) {
  // result.status undefined or 0 === sucess
  console.log(`Update affected ${result.rowsAffected} rows`);
}
```

Batch execution allows transactional execution of a set of commands

```typescript
const commands = [
  ['CREATE TABLE TEST (id integer)'],
  ['INSERT INTO TABLE TEST (id) VALUES (?)', [1]][
    ('INSERT INTO TABLE TEST (id) VALUES (?)', [2])
  ][('INSERT INTO TABLE TEST (id) VALUES (?)', [[3], [4], [5], [6]])],
];
const result = sqlite.executeSqlBatch('myDatabase', commands);
if (!result.status) {
  // result.status undefined or 0 === sucess
  console.log(`Batch affected ${result.rowsAffected} rows`);
}
```

## Learn React Native JSI

If you want to learn how to make your own JSI module buy my [JSI/C++ Cheatsheet](http://ospfranco.gumroad.com/l/IeeIvl), I'm also available for [freelance work](mailto:ospfranco@protonmail.com?subject=Freelance)!

## License

react-native-quick-sqlite is licensed under MIT.
