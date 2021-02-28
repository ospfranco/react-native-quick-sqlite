export function openDb(name: string): boolean {
  // @ts-ignore
  return global.sequel_open(name)
}

export function initDb(): boolean {
  // @ts-ignore
  return global.sequel_init();
}