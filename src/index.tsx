// declare function openDb(name: string): boolean;

export function openDb(name: string): boolean {
  // @ts-ignore
  return global.sequel_open(name)
}