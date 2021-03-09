interface RNSequelResponse<T> {
  error?: Error,
  result?: T
}



export function openDb(dbName: string): RNSequelResponse<string> {
  try {
    // @ts-ignore
    sqlite.open(dbName);
    
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
    sqlite.close(dbName);
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
    sqlite.delete(dbName);
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
    let result = sqlite.execSQL(dbName, sql);
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
    let result = await sqlite.asyncExecSQL(dbName, sql);
    return { result }
  } catch(error) {
    return { error }
  }
}

// TODO add batch method? I'm sure it is necessary for some cases, but can be done later