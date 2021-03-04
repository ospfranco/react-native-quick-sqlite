export function openDb(name: string): boolean {
  // @ts-ignore
  return global.sequel_open(name);
}

export function closeDb(name: string): boolean {
  // @ts-ignore
  return global.sequel_close(name);
}

export function execSQL(sql: string): any {
  // @ts-ignore
  return global.sequel_execSQL(sql);
}