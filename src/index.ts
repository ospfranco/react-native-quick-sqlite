import { NativeModules } from 'react-native';

declare global {
  function nativeCallSyncHook(): unknown;
  var __QuickSQLiteProxy: object | undefined;
}

if (global.__QuickSQLiteProxy == null) {
  const QuickSQLiteModule = NativeModules.QuickSQLite;

  if (QuickSQLiteModule == null) {
    throw new Error(
      'Base quick-sqlite module not found. Maybe try rebuilding the app.'
    );
  }

  // Check if we are running on-device (JSI)
  if (global.nativeCallSyncHook == null || QuickSQLiteModule.install == null) {
    throw new Error(
      'Failed to install react-native-quick-sqlite: React Native is not running on-device. QuickSQLite can only be used when synchronous method invocations (JSI) are possible. If you are using a remote debugger (e.g. Chrome), switch to an on-device debugger (e.g. Flipper) instead.'
    );
  }

  // Call the synchronous blocking install() function
  const result = QuickSQLiteModule.install();
  if (result !== true)
    throw new Error(
      `Failed to install react-native-quick-sqlite: The native QuickSQLite Module could not be installed! Looks like something went wrong when installing JSI bindings: ${result}`
    );

  // Check again if the constructor now exists. If not, throw an error.
  if (global.__QuickSQLiteProxy == null)
    throw new Error(
      'Failed to install react-native-quick-sqlite, the native initializer function does not exist. Are you trying to use QuickSQLite from different JS Runtimes?'
    );
}

const proxy = global.__QuickSQLiteProxy;
export const QuickSQLite = proxy as ISQLite;
/**
 * JSI BINDINGS DO NOT WORK WHEN CONNECTED TO THE CHROME DEBUGGER
 * Use flipper to debug your RN apps from now on
 */

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

/**
 * Object returned by SQL Query executions {
 *  status: 0 or undefined for correct execution, 1 for error
 *  insertId: Represent the auto-generated row id if applicable
 *  rowsAffected: Number of affected rows if result of a update query
 *  message: if status === 1, here you will find error description
 *  rows: if status is undefined or 0 this object will contain the query results
 * }
 *
 * @interface QueryResult
 */
export interface QueryResult extends StatementResult {
  insertId?: number;
  rowsAffected: number;
  rows?: {
    /** Raw array with all dataset */
    _array: any[];
    /** The lengh of the dataset */
    length: number;
    /** A convenience function to acess the index based the row object
     * @param idx the row index
     * @returns the row structure identified by column names
     */
    item: (idx: number) => any;
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
export type ColumnMetadata = {
  /** The name used for this column for this resultset */
  columnName: string;
  /** The declared column type for this column, when fetched directly from a table or a View resulting from a table column. "UNKNOWN" for dynamic values, like function returned ones. */
  columnDeclaredType: string;
  /**
   * The index for this column for this resultset*/
  columnIndex: number;
};

/**
 * Allows the execution of bulk of sql commands
 * inside a transaction
 * If a single query must be executed many times with different arguments, its preferred
 * to declare it a single time, and use an array of array parameters.
 */
type SQLBatchParams = [string] | [string, Array<any> | Array<Array<any>>];

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

export interface Transaction {
  execute: (query: string, params?: any[]) => QueryResult;
}

export interface AsyncTransaction {
  execute: (query: string, params?: any[]) => QueryResult;
  executeAsync: (
    query: string,
    params: any[] | undefined
  ) => Promise<QueryResult>;
  promiseexecute: (
    query: string,
    params: any[] | undefined
  ) => Promise<QueryResult>;
}

export interface PendingTransaction {
  start: () => void;
}

interface ISQLite {
  open: (
    dbName: string,
    location?: string
  ) => {
    status: 0 | 1;
    message?: string;
  };
  close: (dbName: string) => {
    status: 0 | 1;
    message?: string;
  };
  delete: (dbName: string, location?: string) => StatementResult;
  attach: (
    mainDbName: string,
    dbNameToAttach: string,
    alias: string,
    location?: string
  ) => StatementResult;
  detach: (mainDbName: string, alias: string) => StatementResult;
  transaction: (dbName: string, fn: (tx: Transaction) => boolean) => void;
  asyncTransaction: (
    dbName: string,
    fn: (tx: AsyncTransaction) => Promise<boolean>
  ) => void;
  execute: (dbName: string, query: string, params?: any[]) => QueryResult;
  executeAsync: (
    dbName: string,
    query: string,
    params?: any[]
  ) => Promise<QueryResult>;
  executeBatch: (
    dbName: string,
    commands: SQLBatchParams[]
  ) => BatchQueryResult;
  executeBatchAsync: (
    dbName: string,
    commands: SQLBatchParams[],
    cb: (res: BatchQueryResult) => void
  ) => void;
  loadFile: (dbName: string, location: string) => FileLoadResult;
  loadFileAsync: (
    dbName: string,
    location: string,
    cb: (res: FileLoadResult) => void
  ) => void;
}

//   _______ _____            _   _  _____         _____ _______ _____ ____  _   _  _____
//  |__   __|  __ \     /\   | \ | |/ ____|  /\   / ____|__   __|_   _/ __ \| \ | |/ ____|
//     | |  | |__) |   /  \  |  \| | (___   /  \ | |       | |    | || |  | |  \| | (___
//     | |  |  _  /   / /\ \ | . ` |\___ \ / /\ \| |       | |    | || |  | | . ` |\___ \
//     | |  | | \ \  / ____ \| |\  |____) / ____ \ |____   | |   _| || |__| | |\  |____) |
//     |_|  |_|  \_\/_/    \_\_| \_|_____/_/    \_\_____|  |_|  |_____\____/|_| \_|_____/

const locks: Record<
  string,
  { queue: PendingTransaction[]; inProgress: boolean }
> = {};

// Enhance some host functions

// Add 'item' function to result object to allow the sqlite-storage typeorm driver to work
const enhanceQueryResult = (result: QueryResult): void => {
  // Add 'item' function to result object to allow the sqlite-storage typeorm driver to work
  if (result.rows == null) {
    result.rows = {
      _array: [],
      length: 0,
      item: (idx: number) => result.rows._array[idx],
    };
  } else {
    result.rows.item = (idx: number) => result.rows._array[idx];
  }
};

const _open = QuickSQLite.open;
QuickSQLite.open = (dbName: string, location?: string) => {
  const res = _open(dbName, location);
  if (res.status === 0) {
    locks[dbName] = {
      queue: [],
      inProgress: false,
    };
  }

  return res;
};

const _close = QuickSQLite.close;
QuickSQLite.close = (dbName: string) => {
  const res = _close(dbName);
  if (res.status === 0) {
    setImmediate(() => {
      delete locks[dbName];
    });
  }
  return res;
};

const _execute = QuickSQLite.execute;
QuickSQLite.execute = (
  dbName: string,
  query: string,
  params: any[] | undefined
): QueryResult => {
  const result = _execute(dbName, query, params);
  enhanceQueryResult(result);
  return result;
};

const _executeAsync = QuickSQLite.executeAsync;
QuickSQLite.executeAsync = async (
  dbName: string,
  query: string,
  params: any[] | undefined
): Promise<QueryResult> => {
  const res = await _executeAsync(dbName, query, params);
  enhanceQueryResult(res);
  return res;
};

QuickSQLite.transaction = (
  dbName: string,
  callback: (tx: Transaction) => boolean
) => {
  if (!locks[dbName]) {
    throw Error(`No lock found on db: ${dbName}`);
  }

  // Local transaction context object implementation
  const execute = (query: string, params?: any[]): QueryResult => {
    return QuickSQLite.execute(dbName, query, params);
  };

  const tx: PendingTransaction = {
    start: () => {
      try {
        QuickSQLite.execute(dbName, 'BEGIN TRANSACTION', null);
        const result = callback({ execute });
        if (result === true) {
          QuickSQLite.execute(dbName, 'COMMIT', null);
        } else {
          QuickSQLite.execute(dbName, 'ROLLBACK', null);
        }
      } catch (e: any) {
        QuickSQLite.execute(dbName, 'ROLLBACK', null);
        throw e;
      } finally {
        locks[dbName].inProgress = false;
        startNextTransaction(dbName);
      }
    },
  };

  locks[dbName].queue.push(tx);
  startNextTransaction(dbName);
};

QuickSQLite.asyncTransaction = (
  dbName: string,
  callback: (tx: AsyncTransaction) => Promise<boolean>
) => {
  if (!locks[dbName]) {
    throw Error(`Quick SQLite Error: No lock found on db: ${dbName}`);
  }

  // Local transaction context object implementation
  const execute = (query: string, params?: any[]): QueryResult => {
    return QuickSQLite.execute(dbName, query, params);
  };

  const executeAsync = (query: string, params: any[] | undefined) => {
    return QuickSQLite.executeAsync(dbName, query, params);
  };

  const tx: PendingTransaction = {
    start: async () => {
      try {
        QuickSQLite.execute(dbName, 'BEGIN TRANSACTION', null);
        const result = await callback({
          execute,
          executeAsync,
          promiseexecute,
        });
        if (result === true) {
          QuickSQLite.execute(dbName, 'COMMIT', null);
        } else {
          QuickSQLite.execute(dbName, 'ROLLBACK', null);
        }
      } catch (e: any) {
        QuickSQLite.execute(dbName, 'ROLLBACK', null);
        throw e;
      } finally {
        locks[dbName].inProgress = false;
        startNextTransaction(dbName);
      }
    },
  };

  locks[dbName].queue.push(tx);
  startNextTransaction(dbName);
};

const startNextTransaction = (dbName: string) => {
  if (locks[dbName].inProgress) {
    // Transaction is already in process bail out
    return;
  }

  setImmediate(() => {
    if (!locks[dbName]) {
      throw Error(`Lock not found for db ${dbName}`);
    }

    if (locks[dbName].queue.length) {
      locks[dbName].inProgress = true;
      locks[dbName].queue.shift().start();
    }
  });
};

// //   _________     _______  ______ ____  _____  __  __            _____ _____
// //  |__   __\ \   / /  __ \|  ____/ __ \|  __ \|  \/  |     /\   |  __ \_   _|
// //     | |   \ \_/ /| |__) | |__ | |  | | |__) | \  / |    /  \  | |__) || |
// //     | |    \   / |  ___/|  __|| |  | |  _  /| |\/| |   / /\ \ |  ___/ | |
// //     | |     | |  | |    | |___| |__| | | \ \| |  | |  / ____ \| |    _| |_
// //     |_|     |_|  |_|    |______\____/|_|  \_\_|  |_| /_/    \_\_|   |_____|

// interface IConnectionOptions {
//   name: string;
//   location?: string; // not used, we are storing everything on the documents folder
// }

// export interface IDBConnection {
//   execute: (
//     sql: string,
//     args: any[],
//     ok: (res: QueryResult) => void,
//     fail: (msg: string) => void
//   ) => void;
//   executeAsync: (
//     query: string,
//     params: any[] | undefined,
//     cb: (res: QueryResult) => void,
//     fail: (msg: string) => void
//   ) => void;
//   executeBatch: (
//     commands: SQLBatchParams[],
//     callback?: (res: BatchQueryResult) => void
//   ) => void;
//   executeBatchAsync: (
//     commands: SQLBatchParams[],
//     cb: (res: BatchQueryResult) => void
//   ) => void;
//   close: (ok: (res: any) => void, fail: (msg: string) => void) => void;
//   attach: (
//     dbNameToAttach: string,
//     alias: string,
//     location: string | undefined,
//     callback: (result: StatementResult) => void
//   ) => void;
//   detach: (alias: string, callback: (result: StatementResult) => void) => void;
//   transaction: (fn: (tx: Transaction) => boolean) => void;
//   asyncTransaction: (fn: (tx: AsyncTransaction) => Promise<boolean>) => void;
//   loadFile: (
//     location: string,
//     callback: (result: FileLoadResult) => void
//   ) => void;
//   loadFileAsync: (
//     location: string,
//     callback: (res: FileLoadResult) => void
//   ) => void;
// }

// export const openDatabase = (
//   options: IConnectionOptions,
//   ok: (db: IDBConnection) => void,
//   fail: (msg: string) => void
// ): IDBConnection => {
//   try {
//     QuickSQLite.open(options.name, options.location);

//     const connection: IDBConnection = {
//       execute: (
//         sql: string,
//         params: any[] | undefined,
//         ok: (res: QueryResult) => void,
//         fail: (msg: string) => void
//       ) => {
//         try {
//           let response = QuickSQLite.execute(options.name, sql, params);
//           enhanceQueryResult(response);

//           if (response.status === 1) {
//             fail(response.message);
//           } else {
//             ok(response);
//           }
//         } catch (e) {
//           fail(e);
//         }
//       },
//       executeAsync: (
//         sql: string,
//         params: any[] | undefined,
//         cb: (res: QueryResult) => void,
//         fail: (msg: string) => void
//       ) => {
//         try {
//           QuickSQLite.executeAsync(options.name, sql, params, (response) => {
//             if (response.status === 1) {
//               fail(response.message);
//             }
//             enhanceQueryResult(response);
//             cb(response);
//           });
//         } catch (e) {
//           fail(e);
//         }
//       },
//       executeBatch: (
//         commands: SQLBatchParams[],
//         callback?: (res: BatchQueryResult) => void
//       ) => {
//         const response = QuickSQLite.executeBatch(options.name, commands);
//         if (callback) callback(response);
//       },
//       executeBatchAsync: (
//         commands: SQLBatchParams[],
//         cb: (res: BatchQueryResult) => void
//       ) => {
//         QuickSQLite.executeBatchAsync(options.name, commands, cb);
//       },
//       transaction: (fn: (tx: Transaction) => boolean): void => {
//         QuickSQLite.transaction(options.name, fn);
//       },
//       asyncTransaction: (
//         fn: (tx: AsyncTransaction) => Promise<boolean>
//       ): void => {
//         return QuickSQLite.asyncTransaction(options.name, fn);
//       },
//       close: (ok: any, fail: any) => {
//         try {
//           QuickSQLite.close(options.name);
//           ok();
//         } catch (e) {
//           fail(e);
//         }
//       },
//       attach: (
//         dbNameToAttach: string,
//         alias: string,
//         location: string | undefined,
//         callback: (result: StatementResult) => void
//       ) => {
//         const result = QuickSQLite.attach(
//           options.name,
//           dbNameToAttach,
//           alias,
//           location
//         );
//         callback(result);
//       },
//       detach: (alias, callback: (result: StatementResult) => void) => {
//         const result = QuickSQLite.detach(options.name, alias);
//         callback(result);
//       },
//       loadFile: (
//         location: string,
//         callback: (result: FileLoadResult) => void
//       ) => {
//         const result = QuickSQLite.loadFile(options.name, location);
//         if (callback) {
//           callback(result);
//         }
//       },
//       loadFileAsync: (
//         location: string,
//         callback: (result: FileLoadResult) => void
//       ) => {
//         QuickSQLite.loadFileAsync(options.name, location, callback);
//       },
//     };

//     ok(connection);

//     return connection;
//   } catch (e) {
//     fail(e);
//   }
// };
