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

export function closeDb(dbName: string): RNSequelResponse<string> {
  try {
    // @ts-ignore
    global.sequel_close(dbName);
    return {
      result: dbName
    }
  } catch(error) {
    return {error}
  }
}

export function deleteDb(dbName: string): RNSequelResponse<string> {
  try {
    // @ts-ignore
    global.sequel_delete(dbName);
    return {
      result: dbName
    }
  } catch(error) {
    return {error}
  }
}

export function execSQL(dbName: string, sql: string): RNSequelResponse<any[]> {
  try {
    // @ts-ignore
    let result = global.sequel_execSQL(dbName, sql);
    return {
      result
    }
  } catch(error) {
    return {
      error
    }
  }
}

export async function asyncExecSQL(dbName: string, sql: string): Promise<RNSequelResponse<any[]>> {
  try {
    // @ts-ignore
    let result = await global.sequel_asyncExecSQL(dbName, sql);
    return { result }
  } catch(error) {
    return { error }
  }
}