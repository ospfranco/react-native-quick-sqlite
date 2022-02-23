// IMPORTANT!!!!!!!!!!!
// JSI BINDINGS DO NOT WORK WHEN CONNECTED TO THE DEBUGGER
// This is a low level api
type QueryResult = {
  rows?: { // if status is undefined or 0 this object will contain the query results
    _array: any[];
    length: number;
    item: (idx: number) => any;
  };
  insertId?: number;
  rowsAffected: number;
  status?: 0 | 1; // 0 or undefined for correct execution
  message?: string; // if status === 1, here you will find error description
}

interface ISQLite {
  open: (dbName: string, location?: string) => any;
  close: (dbName: string) => any;
  executeSql: (
    dbName: string,
    query: string,
    params: any[] | undefined
  ) => QueryResult;
  // backgroundExecuteSql: (dbName: string, query: string, params: any[]) => any;
}

declare global {
  var sqlite: ISQLite;
}

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
        ok: any,
        fail: any
      ) => {
        try {
          // console.warn(`[react-native-quick-sqlite], sql: `, sql, ` params: ` , params);
          let response = sqlite.executeSql(options.name, sql, params);
          if(response.rows) {
            // enhance object to allow the sqlite-storage typeorm driver to work
            response.rows.item = (idx: number) => response.rows._array[idx];
          }
          ok(response);
        } catch (e) {
          fail(e);
        }
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
