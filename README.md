![screenshot](https://raw.githubusercontent.com/ospfranco/react-native-quick-sqlite/main/header.png)

<div align="center">
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

Inspired/compatible with [react-native-sqlite-storage](https://github.com/andpor/react-native-sqlite-storage) and [react-native-sqlite2](https://github.com/craftzdog/react-native-sqlite-2). Performance metrics are intentionally not posted, [annecdotical testimonies](https://dev.to/craftzdog/a-performant-way-to-use-pouchdb7-on-react-native-in-2022-24ej) suggest anywhere between 2x and 5x speed improvement.

## Gotchas

- **Javascript cannot represent integers larger than 53 bits**, be careful when loading data if it came from other systems. [Read more](https://github.com/ospfranco/react-native-quick-sqlite/issues/16#issuecomment-1018412991).
- **It's not possible to use a browser to debug a JSI app**, use [Flipper](https://github.com/facebook/flipper) (for android Flipper also has SQLite Database explorer).
- Install the NDK on your machine for android.

## API

```typescript
/**
 * All SQLIte command results will have at least this status definition:
 * Specific statments or actions can bring more data, relative to its context
 * status: 0 or undefined for correct execution, 1 for error
 *  message: if status === 1, here you will find error description
 */
export interface StatementResult {
  status?: 0 | 1;
  message?: string;
}

interface QueryResult extends StatementResult {
  insertId?: number;
  rowsAffected: number;
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

/**
 * status: 0 or undefined for correct execution, 1 for error
 * message: if status === 1, here you will find error description
 * rowsAffected: Number of affected rows if status == 0
 */
export interface BatchQueryResult extends StatementResult {
  rowsAffected?: number;
}

/**
 * Result of loading a file and executing every line as a SQL command
 * Similar to BatchQueryResult
 */
export interface FileLoadResult extends BatchQueryResult {
  commands?: number;
}

interface ISQLite {
  open: (dbName: string, location?: string) => StatementResult;
  close: (dbName: string) => StatementResult;
  delete: (dbName: string, location?: string) => StatementResult;
  attach: (
    mainDbName: string,
    dbNameToAttach: string,
    alias: string,
    location?: string
  ) => StatementResult;
  detach: (mainDbName: string, alias: string) => StatementResult;
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

### WebSQL wrapper

You can get a WebSQL wrapper (meant to be used with TypeORM or other drivers) with a different global call. It's a simple wrapper around the low level API.

```ts
openDatabase(
  options: IConnectionOptions,
  ok: (db: IDBConnection) => void,
  fail: (msg: string) => void
): IDBConnection
```

# Usage

Just import the package and fire away

```typescript
// Thanks to @mrousavy for this installation method, see one example: https://github.com/mrousavy/react-native-mmkv/blob/75b425db530e26cf10c7054308583d03ff01851f/src/createMMKV.ts#L56
import { QuickSQLite } from 'react-native-quick-sqlite';

const dbOpenResult = QuickSQLite.open('myDatabase', 'databases');

// status === 1, operation failed
if (dbOpenResult.status) {
  console.error('Database could not be opened');
}
```

### Simple queries

The basic query is **synchronous**, it will block rendering on large operations, below there are async versions.

```typescript
let { status, rows } = QuickSQLite.executeSql(
  'myDatabase',
  'SELECT somevalue FROM sometable'
);
if (!status) {
  rows.forEach((row) => {
    console.log(row);
  });
}

let { status, rowsAffected } = QuickSQLite.executeSql(
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
QuickSQLite.transaction('myDatabase', (tx) => {
  const { status } = tx.executeSql(
    'UPDATE sometable SET somecolumn = ? where somekey = ?',
    [0, 1]
  );

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
const result = QuickSQLite.executeSqlBatch('myDatabase', commands);
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
let { status, metadata } = QuickSQLite.executeSql(
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
QuickSQLite.asyncExecuteSql(
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

### Attach or Detach another databases

SQLite supports to attach or detach another database files into your main database connection through an alias.
You can do any operation you like on this attached databases like JOIN results across tables in different schemas, or update data or objects.
This databases can have different configurations, like journal modes, cache settings.

You can, at any moment detach a database that you don't need anymore.
Note: You don't need to detach an attached database before closing your connection. Closing the main connection
will dettach any dettached databases.
SQLite have a limit for attached databases: A default of 10, and a global max of 125

References: [Attach](https://www.sqlite.org/lang_attach.html) - [Detach](https://www.sqlite.org/lang_detach.html)

```ts
const result = QuickSQLite.attach(
  'mainDatabase',
  'statistics',
  'stats',
  '../databases'
);

// Database is attached sucefully
if (!result.status) {
  const data = QuickSQLite.executeSql(
    'mainDatabase',
    'SELECT * FROM some_table_from_mainschema a INNER JOIN stats.some_table b on a.id_column = b.id_column'
  );
  // Consume the results
  if (!data.status) {
  }
}

// You can detach databases at any moment
const detachResult = QuickSQLite.detach('mainDatabase', 'stats');
if (!detachResult.status) {
  // Database dettached
}
```

## Use built-in SQLite

App size is a real concern for some teams.

On iOS you can use the OS embedded SQLite instance, when running `pod-install` add an environment flag:

```
QUICK_SQLITE_USE_PHONE_VERSION=1 npx pod-install
```

On Android unfortunately it is not possible to link from C++ to the phone's embedded SQLite. It is also very buggy (vendor changes, old android bugs, etc). The recommended way is to embed your own version of SQLite anyways. Unfortunately this means we are stuck and this library will add some mbs to your app size.

## Use TypeORM

You can use this driver with [TypeORM](https://github.com/typeorm/typeorm), when initializing the connection use:

```ts
datasource = new DataSource({
  type: 'react-native',
  database: 'typeormdb',
  location: '.',
  driver: require('react-native-quick-sqlite'),
  entities: [Book, User],
  synchronize: true,
});
```

If you are using Node 14+, TypeORM is currently broken with React Native. You can patch your node-modules installation and apply the fix [in this issue](https://github.com/typeorm/typeorm/issues/9178).

## More

If you want to learn how to make your own JSI module buy my [JSI/C++ Cheatsheet](http://ospfranco.gumroad.com/), I'm also available for [freelance work](mailto:ospfranco@protonmail.com?subject=Freelance).

## License

react-native-quick-sqlite is licensed under MIT.
