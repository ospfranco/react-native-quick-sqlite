<h1 align="center">React Native Quick SQLite</h1>

![screenshot](https://raw.githubusercontent.com/ospfranco/react-native-quick-sqlite/main/header.png)

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

This library provides a low-level API to execute SQL queries, fast bindings via [JSI](https://formidable.com/blog/2019/jsi-jsc-part-2).

Inspired/compatible with [react-native-sqlite-storage](https://github.com/andpor/react-native-sqlite-storage) and [react-native-sqlite2](https://github.com/craftzdog/react-native-sqlite-2).

## Gotchas

- **Javascript cannot represent integers larger than 53 bits**, be careful when loading data if it came from other systems. [Read more](https://github.com/ospfranco/react-native-quick-sqlite/issues/16#issuecomment-1018412991).
- **It's not possible to use a browser to debug a JSI app**, use [Flipper](https://github.com/facebook/flipper) (for android Flipper also has SQLite Database explorer).
- Your app will now include C++, you will need to install the NDK on your machine for android.

## API

```typescript
interface QueryResult {
  status?: 0 | 1; // 0 for correct execution
  insertId?: number;
  rowsAffected: number;
  message?: string;
  rows?: {
    /** Raw array with all dataset */
    _array: any[];
    /** The lengh of the dataset */
    length: number;
  };
  /**
   * Query metadata, avaliable only for select query results
   */
  metadata?: ColumnMetadata[];
}

/**
 * Column metadata
 * Describes some information about columns fetched by the query
 */
declare type ColumnMetadata = {
  /** The name used for this column for this resultset */
  columnName: string;
  /** The declared column type for this column, when fetched directly from a table or a View resulting from a table column. "UNKNOWN" for dynamic values, like function returned ones. */
  columnDeclaredType: string;
  /**
   * The index for this column for this resultset*/
  columnIndex: number;
};

interface BatchQueryResult {
  status?: 0 | 1;
  rowsAffected?: number;
  message?: string;
}

interface ISQLite {
  open: (dbName: string, location?: string) => { status: 0 | 1 };
  close: (dbName: string) => { status: 0 | 1 };
  executeSql: (
    dbName: string,
    query: string,
    params: any[] | undefined
  ) => QueryResult;
  asyncExecuteSql: (
    dbName: string,
    query: string,
    params: any[] | undefined,
    cb: (res: QueryResult) => void
  ) => void;
  executeSqlBatch: (
    dbName: string,
    commands: SQLBatchParams[]
  ) => BatchQueryResult;
  asyncExecuteSqlBatch: (
    dbName: string,
    commands: SQLBatchParams[],
    cb: (res: BatchQueryResult) => void
  ) => void;
  loadSqlFile: (dbName: string, location: string) => FileLoadResult;
  asyncLoadSqlFile: (
    dbName: string,
    location: string,
    cb: (res: FileLoadResult) => void
  ) => void;
}
```

# Usage

```typescript
// Import as early as possible, auto-installs bindings. 
// Thanks to @msrousavy for this installation method, see one example: https://github.com/mrousavy/react-native-mmkv/blob/75b425db530e26cf10c7054308583d03ff01851f/src/createMMKV.ts#L56
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

In some scenarios, dynamic applications may need to get some metadata information about the returned resultset.
This can be done testing the returned data directly, but in some cases may not be enough, like when data is stored outside
storage datatypes, like booleans or datetimes. When fetching data directly from tables or views linked to table columns, SQLite is able
to identify the table declared types:

```typescript
let result = sqlite.executeSql('myDatabase', 'SELECT int_column_1, bol_column_2 FROM sometable');
if (!result.status) {
  // result.status undefined or 0 === sucess
  for (let i = 0; i < result.metadata.length; i++) {
    const column = result.metadata[i];
    console.log(`${column.columnName} - ${column.columnDeclaredType}`);
    // Output:
    // int_column_1 - INTEGER
    // bol_column_2 - BOOLEAN
  }
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

Async versions are also available if you have too much SQL to execute

```ts
sqlite.asyncExecuteSql('myDatabase', 'SELECT * FROM "User";', [], (result) => {
  if (result.status === 0) {
    console.log('users', result.rows);
  }
});
```

## Use TypeORM

This package offers a low-level API to raw execute SQL queries. I strongly recommend to use [TypeORM](https://github.com/typeorm/typeorm) (with [patch-package](https://github.com/ds300/patch-package)). TypeORM already has a sqlite-storage driver. In the `example` project on the `patch` folder you can a find a [patch for TypeORM](https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch).

Follow the instructions to make TypeORM work with React Native (enable decorators, configure babel, etc), then apply the example patch via patch-package.

## Learn React Native JSI

If you want to learn how to make your own JSI module buy my [JSI/C++ Cheatsheet](http://ospfranco.gumroad.com/l/jsi_guide), I'm also available for [freelance work](mailto:ospfranco@protonmail.com?subject=Freelance)!

## License

react-native-quick-sqlite is licensed under MIT.
