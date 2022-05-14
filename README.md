![screenshot](https://raw.githubusercontent.com/ospfranco/react-native-quick-sqlite/main/header.png)

<div align="center">
  <h3>With TypeORM</h3>
  <pre align="center">
    <a href="https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch">Copy typeORM patch-package from example dir</a>
    yarn add react-native-quick-sqlite typeorm
    npx pod-install
    <a href="https://dev.to/vinipachecov/setup-typeorm-with-react-native-50c4">Enable decorators and configure babel</a></pre>
  <br>
  <h3>Low level bindings only</h3>
  <pre align="center">
    yarn add react-native-quick-sqlite
    npx pod-install</pre>
  <a align="center" href="https://github.com/ospfranco?tab=followers">
    <img src="https://img.shields.io/github/followers/ospfranco?label=Follow%20%40ospfranco&style=social" />
  </a>
  <br />
  <a align="center" href="https://twitter.com/ospfranco">
    <img src="https://img.shields.io/twitter/follow/ospfranco?label=Follow%20%40ospfranco&style=social" />
  </a>
</div>
<br />

Quick SQLite embeds the latest version of SQLite and provides a low-level API to execute SQL queries, uses fast bindings via [JSI](https://formidable.com/blog/2019/jsi-jsc-part-2). By using an embedded SQLite you get access the latest security patches and latest features.

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
    /** The length of the dataset */
    length: number;
  };
  /**
   * Query metadata, available only for select query results
   */
  metadata?: ColumnMetadata[];
}

/**
 * Column metadata
 * Describes some information about columns fetched by the query
 * columnDeclaredType - declared column type for this column, when fetched directly from a table or a View resulting from a table column. "UNKNOWN" for dynamic values, like function returned ones.
 */
interface ColumnMetadata = {
  columnName: string;
  columnDeclaredType: string;
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
  delete: (dbName: string, location?: string) => { status: 0 | 1 };
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

Import as early as possible, auto-installs bindings in a thread-safe manner.

```typescript
// Thanks to @mrousavy for this installation method, see one example: https://github.com/mrousavy/react-native-mmkv/blob/75b425db530e26cf10c7054308583d03ff01851f/src/createMMKV.ts#L56
import 'react-native-quick-sqlite';

// Afterwards `sqlite` is a globally registered object, so you can directly call it from anywhere in your javascript
const dbOpenResult = sqlite.open('myDatabase', 'databases');

// status === 1, operation failed
if (dbOpenResult.status) {
  console.error('Database could not be opened');
}
```

### Simple queries

The basic query is **synchronous**, it will block rendering on large operations, below there are async versions.

```typescript
let { status, rows } = sqlite.executeSql(
  'myDatabase',
  'SELECT somevalue FROM sometable'
);
if (!status) {
  rows.forEach((row) => {
    console.log(row);
  });
}

let { status, rowsAffected } = sqlite.executeSql(
  'myDatabase',
  'UPDATE sometable SET somecolumn = ? where somekey = ?',
  [0, 1]
);
if (!status) {
  console.log(`Update affected ${rowsAffected} rows`);
}
```

### Transactions

Transactions are supported. However, due to the library being opinionated and mostly not throwing errors you need to return a boolean (true for correct execution, false for incorrect execution) to either commit or rollback the transaction.

JSI bindings are fast but there is still some overhead calling `executeSql` for single queries, if you want to execute a large set of commands as fast as possible you should use the `executeSqlBatch` method below, it still uses transactions, but only transmits data between JS and native once.

```typescript
sqlite.transaction('myDatabase', (tx) => {
  const {
    status,
  } = tx.executeSql('UPDATE sometable SET somecolumn = ? where somekey = ?', [
    0,
    1,
  ]);

  if (status) {
    return false;
  }

  return true;
});
```

### Batch operation

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
  // result.status undefined or 0 === success
  console.log(`Batch affected ${result.rowsAffected} rows`);
}
```

### Dynamic Column Metadata

In some scenarios, dynamic applications may need to get some metadata information about the returned result set.

This can be done testing the returned data directly, but in some cases may not be enough, for example when data is stored outside
sqlite datatypes. When fetching data directly from tables or views linked to table columns, SQLite is able
to identify the table declared types:

```typescript
let { status, metadata } = sqlite.executeSql(
  'myDatabase',
  'SELECT int_column_1, bol_column_2 FROM sometable'
);
if (!status) {
  metadata.forEach((column) => {
    // Output:
    // int_column_1 - INTEGER
    // bol_column_2 - BOOLEAN
    console.log(`${column.columnName} - ${column.columnDeclaredType}`);
  });
}
```

### Async operations

You might have too much SQL to process and it will cause your application to freeze. There are async versions for some of the operations. This will offload the SQLite processing to a different thread.

```ts
sqlite.asyncExecuteSql(
  'myDatabase',
  'SELECT * FROM "User";',
  [],
  ({ status, rows }) => {
    if (status === 0) {
      console.log('users', rows);
    }
  }
);
```

## Use TypeORM

This package offers a low-level API to raw execute SQL queries. I strongly recommend to use [TypeORM](https://github.com/typeorm/typeorm) (with [patch-package](https://github.com/ds300/patch-package)). TypeORM already has a sqlite-storage driver. In the `example` project on the `patch` folder you can a find a [patch for TypeORM](https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch).

Follow the instructions to make TypeORM work with React Native (enable decorators, configure babel, etc), then apply the example patch via patch-package.

## More

If you want to learn how to make your own JSI module buy my [JSI/C++ Cheatsheet](http://ospfranco.gumroad.com/), I'm also available for [freelance work](mailto:ospfranco@protonmail.com?subject=Freelance)!

## License

react-native-quick-sqlite is licensed under MIT.
