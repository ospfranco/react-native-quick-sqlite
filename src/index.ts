// IMPORTANT!!!!!!!!!!!
// JSI BINDINGS DO NOT WORK WHEN CONNECTED TO THE DEBUGGER
// This is a low level api
interface ISQLite {
  open: (dbName: string, location?: string) => any;
  close: (dbName: string) => any;
  executeSql: (
    dbName: string,
    query: string,
    params: any[] | undefined
  ) => {
    rows: {
      _array: any[];
      length: number;
      item: (idx: number) => any;
    };
    insertId?: number;
    rowsAffected: number;
  };
  // backgroundExecuteSql: (dbName: string, query: string, params: any[]) => any;
}

// Make it globally available
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
    ok: (res: any) => void,
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
          // enhance object to allow the sqlite-storage typeorm driver to work
          response.rows.item = (idx: number) => response.rows._array[idx];

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
