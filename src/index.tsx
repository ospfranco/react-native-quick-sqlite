interface RNSequelResponse<T> {
  error?: Error,
  result?: T
}

export function openDb(dbName: string): RNSequelResponse<string> {
  try {
    // @ts-ignore
    global.sequel_open(dbName);
    return {
      result: dbName
    }
  } catch (error) {
    return {
      error
    }
  }
}

export function closeDb(dbName: string): boolean {
  // @ts-ignore
  return global.sequel_close(dbName);
}

export function execSQL(dbName: string, sql: string): any {
  // @ts-ignore
  return global.sequel_execSQL(dbName, sql);
}

export function deleteDb(dbName: string): boolean {
  // @ts-ignore
  return global.sequel_delete(dbName);
}

export function asyncExecSQL(dbName: string, sql: string): Promise<any> {
  // @ts-ignore
  return global.sequel_asyncExecSQL(dbName, sql);
}