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
  QuickSQLiteModule.install();

  // Check again if the constructor now exists. If not, throw an error.
  if (global.__QuickSQLiteProxy == null) {
    throw new Error(
      'Failed to install react-native-quick-sqlite, the native initializer function does not exist. Are you trying to use QuickSQLite from different JS Runtimes?'
    );
  }
}

const proxy = global.__QuickSQLiteProxy;
export const QuickSQLite = proxy as ISQLite;

/**
 * Object returned by SQL Query executions {
 *  insertId: Represent the auto-generated row id if applicable
 *  rowsAffected: Number of affected rows if result of a update query
 *  message: if status === 1, here you will find error description
 *  rows: if status is undefined or 0 this object will contain the query results
 * }
 *
 * @interface QueryResult
 */
export type QueryResult = {
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
};

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
export type SQLBatchTuple = [string] | [string, Array<any> | Array<Array<any>>];

/**
 * status: 0 or undefined for correct execution, 1 for error
 * message: if status === 1, here you will find error description
 * rowsAffected: Number of affected rows if status == 0
 */
export type BatchQueryResult = {
  rowsAffected?: number;
};

/**
 * Result of loading a file and executing every line as a SQL command
 * Similar to BatchQueryResult
 */
export interface FileLoadResult extends BatchQueryResult {
  commands?: number;
}

export interface Transaction {
  commit: () => QueryResult;
  execute: (query: string, params?: any[]) => QueryResult;
  executeAsync: (
    query: string,
    params?: any[] | undefined
  ) => Promise<QueryResult>;
  rollback: () => QueryResult;
}

export interface PendingTransaction {
  /*
   * The start function should not throw or return a promise because the
   * queue just calls it and does not monitor for failures or completions.
   *
   * It should catch any errors and call the resolve or reject of the wrapping
   * promise when complete.
   *
   * It should also automatically commit or rollback the transaction if needed
   */
  start: () => void;
}

interface ISQLite {
  open: (dbName: string, location?: string) => void;
  close: (dbName: string) => void;
  delete: (dbName: string, location?: string) => void;
  attach: (
    mainDbName: string,
    dbNameToAttach: string,
    alias: string,
    location?: string
  ) => void;
  detach: (mainDbName: string, alias: string) => void;
  transaction: (
    dbName: string,
    fn: (tx: Transaction) => Promise<void> | void
  ) => Promise<void>;
  execute: (dbName: string, query: string, params?: any[]) => QueryResult;
  executeAsync: (
    dbName: string,
    query: string,
    params?: any[]
  ) => Promise<QueryResult>;
  executeBatch: (dbName: string, commands: SQLBatchTuple[]) => BatchQueryResult;
  executeBatchAsync: (
    dbName: string,
    commands: SQLBatchTuple[]
  ) => Promise<BatchQueryResult>;
  loadFile: (dbName: string, location: string) => FileLoadResult;
  loadFileAsync: (dbName: string, location: string) => Promise<FileLoadResult>;
  function: (
    dbName: string,
    name: string,
    nArgs: number,
    DETERMINISTIC: boolean,
    DIRECTONLY: boolean,
    INNOCUOUS: boolean,
    SUBTYPE: boolean,
    callback: (...args: any[]) => void,
    key?: string
    ) => void;

  aggregate: (
      dbName: string,
      name: string,
      nArgs: number,
      DETERMINISTIC: boolean,
      DIRECTONLY: boolean,
      INNOCUOUS: boolean,
      SUBTYPE: boolean,
      step: (...args: any[]) => void,
      start?: any,
      inverse?: (...args: any[]) => void,
      result?: (...args: any[]) => any,
      key?: string
      ) => void;
}

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

const getLength = ({ length }) => {
	if (Number.isInteger(length) && length >= 0) return length;
	throw new TypeError('Expected function.length to be a positive integer');
};

const getFunctionOption = (options, key, required) => {
	const value = key in options ? options[key] : null;
	if (typeof value === 'function') return value;
	if (value != null) throw new TypeError(`Expected the "${key}" option to be a function`);
	if (required) throw new TypeError(`Missing required option "${key}"`);
	return null;
};

const _open = QuickSQLite.open;
QuickSQLite.open = (dbName: string, location?: string) => {
  _open(dbName, location);

  locks[dbName] = {
    queue: [],
    inProgress: false,
  };
};

const _close = QuickSQLite.close;
QuickSQLite.close = (dbName: string) => {
  _close(dbName);
  delete locks[dbName];
};

const _execute = QuickSQLite.execute;
QuickSQLite.execute = (
  dbName: string,
  query: string,
  params?: any[] | undefined
): QueryResult => {
  const result = _execute(dbName, query, params);
  enhanceQueryResult(result);
  return result;
};

const _executeAsync = QuickSQLite.executeAsync;
QuickSQLite.executeAsync = async (
  dbName: string,
  query: string,
  params?: any[] | undefined
): Promise<QueryResult> => {
  const res = await _executeAsync(dbName, query, params);
  enhanceQueryResult(res);
  return res;
};

QuickSQLite.transaction = async (
  dbName: string,
  fn: (tx: Transaction) => Promise<void>
): Promise<void> => {
  if (!locks[dbName]) {
    throw Error(`Quick SQLite Error: No lock found on db: ${dbName}`);
  }

  let isFinalized = false;

  // Local transaction context object implementation
  const execute = (query: string, params?: any[]): QueryResult => {
    if (isFinalized) {
      throw Error(
        `Quick SQLite Error: Cannot execute query on finalized transaction: ${dbName}`
      );
    }
    return QuickSQLite.execute(dbName, query, params);
  };

  const executeAsync = (query: string, params?: any[] | undefined) => {
    if (isFinalized) {
      throw Error(
        `Quick SQLite Error: Cannot execute query on finalized transaction: ${dbName}`
      );
    }
    return QuickSQLite.executeAsync(dbName, query, params);
  };

  const commit = () => {
    if (isFinalized) {
      throw Error(
        `Quick SQLite Error: Cannot execute commit on finalized transaction: ${dbName}`
      );
    }
    const result = QuickSQLite.execute(dbName, 'COMMIT');
    isFinalized = true;
    return result;
  };

  const rollback = () => {
    if (isFinalized) {
      throw Error(
        `Quick SQLite Error: Cannot execute rollback on finalized transaction: ${dbName}`
      );
    }
    const result = QuickSQLite.execute(dbName, 'ROLLBACK');
    isFinalized = true;
    return result;
  };

  async function run() {
    try {
      await QuickSQLite.executeAsync(dbName, 'BEGIN TRANSACTION');

      await fn({
        commit,
        execute,
        executeAsync,
        rollback,
      });

      if (!isFinalized) {
        commit();
      }
    } catch (executionError) {
      if (!isFinalized) {
        try {
          rollback();
        } catch (rollbackError) {
          throw rollbackError;
        }
      }

      throw executionError;
    } finally {
      locks[dbName].inProgress = false;
      isFinalized = false;
      startNextTransaction(dbName);
    }
  }

  return await new Promise((resolve, reject) => {
    const tx: PendingTransaction = {
      start: () => {
        run().then(resolve).catch(reject);
      },
    };

    locks[dbName].queue.push(tx);
    startNextTransaction(dbName);
  });
};

const startNextTransaction = (dbName: string) => {
  if (!locks[dbName]) {
    throw Error(`Lock not found for db: ${dbName}`);
  }

  if (locks[dbName].inProgress) {
    // Transaction is already in process bail out
    return;
  }

  if (locks[dbName].queue.length) {
    locks[dbName].inProgress = true;
    const tx = locks[dbName].queue.shift();
    setImmediate(() => {
      tx.start();
    });
  }
};

//   _________     _______  ______ ____  _____  __  __            _____ _____
//  |__   __\ \   / /  __ \|  ____/ __ \|  __ \|  \/  |     /\   |  __ \_   _|
//     | |   \ \_/ /| |__) | |__ | |  | | |__) | \  / |    /  \  | |__) || |
//     | |    \   / |  ___/|  __|| |  | |  _  /| |\/| |   / /\ \ |  ___/ | |
//     | |     | |  | |    | |___| |__| | | \ \| |  | |  / ____ \| |    _| |_
//     |_|     |_|  |_|    |______\____/|_|  \_\_|  |_| /_/    \_\_|   |_____|

/**
 * DO NOT USE THIS! THIS IS MEANT FOR TYPEORM
 * If you are looking for a convenience wrapper use `connect`
 */
export const typeORMDriver = {
  openDatabase: (
    options: {
      name: string;
      location?: string;
    },
    ok: (db: any) => void,
    fail: (msg: string) => void
  ): any => {
    try {
      QuickSQLite.open(options.name, options.location);

      const connection = {
        executeSql: async (
          sql: string,
          params: any[] | undefined,
          ok: (res: QueryResult) => void,
          fail: (msg: string) => void
        ) => {
          try {
            let response = await QuickSQLite.executeAsync(
              options.name,
              sql,
              params
            );
            enhanceQueryResult(response);
            ok(response);
          } catch (e) {
            fail(e);
          }
        },
        transaction: (
          fn: (tx: Transaction) => Promise<void>
        ): Promise<void> => {
          return QuickSQLite.transaction(options.name, fn);
        },
        close: (ok: any, fail: any) => {
          try {
            QuickSQLite.close(options.name);
            ok();
          } catch (e) {
            fail(e);
          }
        },
        attach: (
          dbNameToAttach: string,
          alias: string,
          location: string | undefined,
          callback: () => void
        ) => {
          QuickSQLite.attach(options.name, dbNameToAttach, alias, location);

          callback();
        },
        detach: (alias, callback: () => void) => {
          QuickSQLite.detach(options.name, alias);

          callback();
        },
      };

      ok(connection);

      return connection;
    } catch (e) {
      fail(e);
    }
  },
};

export type FunctionOptions = {
  deterministic?: boolean,
  directonly?: boolean,
  innocuous?: boolean,
  subtype?: boolean,
};

export type QuickSQLiteConnection = {
  close: () => void;
  delete: () => void;
  attach: (dbNameToAttach: string, alias: string, location?: string) => void;
  detach: (alias: string) => void;
  transaction: (fn: (tx: Transaction) => Promise<void> | void) => Promise<void>;
  execute: (query: string, params?: any[]) => QueryResult;
  executeAsync: (query: string, params?: any[]) => Promise<QueryResult>;
  executeBatch: (commands: SQLBatchTuple[]) => BatchQueryResult;
  executeBatchAsync: (commands: SQLBatchTuple[]) => Promise<BatchQueryResult>;
  loadFile: (location: string) => FileLoadResult;
  loadFileAsync: (location: string) => Promise<FileLoadResult>;
  function: (name: string, fn: (...args: any[]) => void, options?: FunctionOptions) => void;
  aggregate: (name: string, aggregateOptions: {
    start?: any,
    step: (...args: any[]) => any,
    result?: (...args: any[]) => any,
    inverse?: (...args: any[]) => any,
  }, options?: FunctionOptions) => void;
};

export const open = (options: {
  name: string;
  location?: string;
}): QuickSQLiteConnection => {
  QuickSQLite.open(options.name, options.location);
  global.functions = [];
  global.aggregates = [];

  return {
    close: () => QuickSQLite.close(options.name),
    delete: () => QuickSQLite.delete(options.name, options.location),
    attach: (dbNameToAttach: string, alias: string, location?: string) =>
      QuickSQLite.attach(options.name, dbNameToAttach, alias, location),
    detach: (alias: string) => QuickSQLite.detach(options.name, alias),
    transaction: (fn: (tx: Transaction) => Promise<void> | void) =>
      QuickSQLite.transaction(options.name, fn),
    execute: (query: string, params?: any[] | undefined): QueryResult =>
      QuickSQLite.execute(options.name, query, params),
    executeAsync: (
      query: string,
      params?: any[] | undefined
    ): Promise<QueryResult> =>
      QuickSQLite.executeAsync(options.name, query, params),
    executeBatch: (commands: SQLBatchTuple[]) =>
      QuickSQLite.executeBatch(options.name, commands),
    executeBatchAsync: (commands: SQLBatchTuple[]) =>
      QuickSQLite.executeBatchAsync(options.name, commands),
    loadFile: (location: string) =>
      QuickSQLite.loadFile(options.name, location),
    loadFileAsync: (location: string) =>
      QuickSQLite.loadFileAsync(options.name, location),
    function: (name: string, fn: (...args: any[]) => any, fnOptions?: FunctionOptions) => {
      const key = `${options.name}.functions.${name}`;
      global[key] = fn;
      QuickSQLite.function(
        options.name,
        name,
        getLength(fn),
        !!fnOptions?.deterministic,
        !!fnOptions?.directonly,
        !!fnOptions?.innocuous,
        !!fnOptions?.subtype,
        fn,
        key
        );
      delete global[key];
      },
    aggregate: (name: string, aggregateOptions, fnOptions?: FunctionOptions) => {
      let argCount;
      argCount = Math.max(getLength(aggregateOptions.step), aggregateOptions.inverse ? getLength(aggregateOptions.inverse) : 0);
      if (argCount > 0) argCount -= 1;
      if (argCount > 100) throw new RangeError('User-defined functions cannot have more than 100 arguments');

      const key = `${options.name}.aggregates.${name}`;
      global[key] = aggregateOptions;
      QuickSQLite.aggregate(
        options.name,
        name,
        argCount,
        !!fnOptions?.deterministic,
        !!fnOptions?.directonly,
        !!fnOptions?.innocuous,
        !!fnOptions?.subtype,
        getFunctionOption (aggregateOptions, 'step', true),
        'start' in aggregateOptions ? aggregateOptions.start : null,
        getFunctionOption (aggregateOptions, 'inverse', false),
        getFunctionOption (aggregateOptions, 'result', false),
        key
        )
      delete global[`aggregates.${name}`];
      }
  };
};
