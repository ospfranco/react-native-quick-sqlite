/**
 * JSI BINDINGS DO NOT WORK WHEN CONNECTED TO THE CHROME DEBUGGER
 * Use flipper to debug your RN apps from now on
 */

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
interface QueryResult {
  status?: 0 | 1;
  insertId?: number;
  rowsAffected: number;
  message?: string;
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
}

/**
 * Allows the execution of bulk of sql commands
 * inside a transaction
 * If a single query must be executed many times with different arguments, its preferred
 * to declare it a single time, and use an array of array parameters.
 */
 type SQLBatchParams = [string, Array<any> | Array<Array<any>> | undefined];

/**
 * status: 0 or undefined for correct execution, 1 for error
 * message: if status === 1, here you will find error description
 * rowsAffected: Number of affected rows if status == 0
 */
interface BatchExecutionResult {
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
  ) => BatchExecutionResult;
  // backgroundExecuteSql: (dbName: string, query: string, params: any[]) => any;
}

declare var sqlite: ISQLite;

// API FOR TYPEORM
interface IConnectionOptions {
  name: string;
  location?: string; // not used, we are storing everything on the documents folder
}

interface IDBConnection {
  executeSql: (
    sql: string,
    args: any[],
    ok: (res: QueryResult) => void,
    fail: (msg: string) => void
  ) => void;
  executeSqlBatch: (commands: SQLBatchParams[], callback?: (res: BatchExecutionResult) => void) => void;
  close: (ok: (res: any) => void, fail: (msg: string) => void) => void;
}

export const openDatabase = (
  options: IConnectionOptions,
  ok: (db: IDBConnection) => void,
  fail: (msg: string) => void
) => {
  try {
    sqlite.open(options.name, options.location);

    const connection: IDBConnection = {
      executeSql: (
        sql: string,
        params: any[] | undefined,
        ok: (res: QueryResult) => void,
        fail: (msg: string) => void
      ) => {
        try {
          // console.warn(`[react-native-quick-sqlite], sql: `, sql, ` params: ` , params);
          let response = sqlite.executeSql(options.name, sql, params);

          // Add 'item' function to result object to allow the sqlite-storage typeorm driver to work
          if (response.rows != null) {
            response.rows.item = (idx: number) => response.rows._array[idx];
          }

          ok(response);
        } catch (e) {
          fail(e);
        }
      },
      executeSqlBatch: (commands: SQLBatchParams[], callback?: (res: BatchExecutionResult) => void) => {
        const response = sqlite.executeSqlBatch(options.name, commands);
        if (callback) callback(response);
      },
      close: (ok: any, fail: any) => {
        try {
          sqlite.close(options.name);
          ok();
        } catch (e) {
          fail(e);
        }
      },
    };

    ok(connection);
  } catch (e) {
    fail(e);
  }
};
