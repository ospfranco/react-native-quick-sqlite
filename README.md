# react-native-quick-sqlite

The **fastest** SQLite implementation for react-native.

![Frame 2](https://user-images.githubusercontent.com/1634213/127499575-aed1d0e2-8a93-42ab-917e-badaab8916f6.png)

This package is meant to be a (more or less) drop-in replacement for [react-native-sqlite-storage](https://github.com/andpor/react-native-sqlite-storage) and other libraries inspired by it (ex. [react-native-sqlite2](https://github.com/craftzdog/react-native-sqlite-2)).

Unlike previous implementations `quick-sqlite` uses [JSI bindings](https://formidable.com/blog/2019/jsi-jsc-part-2/), JSI removes almost all the overhead of intercommunication between JavaScript code and the native platform (using a single C++ codebase).

## GOTCHAS

### JSI bindings are not avaiable when connected to the chrome debugger

This is a limitation with JSI and the JavaScript engines implementing it, RN 0.64 completely dropped support for connecting to the chrome/firefox/safari debugger, from now on you should use [Flipper](https://github.com/facebook/flipper).

### TODOs

- [ ] **Everything is saved on the application's home folder**. I got lazy and ignore any passed path parameter, if you need this feel free to open a PR. 
- [ ] **When parsing the SQLite rows the strings are parsed as ASCII strings instead of UTF-8**, which will explode if you are using other languages. 

I have however ran out of time to fix this issues myself (and I'm a c++ noob), so please do submit a PR to fix this issues if you need them.

### The API is NOT 100% the same as sqlite-storage

Namely, sqlite-storage returns an `item` function on the query result, it takes an index number and returns an object, I simply return an array, because creating deeply nested structures from C++ is somewhat cumbersome.

# You should use TypeORM

The recommended way to use this package is to use [TypeORM](https://github.com/typeorm/typeorm) with [patch-package](https://github.com/ds300/patch-package). TypeORM already has a ReactNative Driver that hardcodes sqlite-storage. In the `example` project on the `patch` folder you can a find a [patch for TypeORM](https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch), it basically just replaces all the `react-native-sqlite-storage` strings in TypeORM with `react-native-quick-sqlite`, and does a tiny change for the way it accesses the resulting rows from the SQL execution.

However you can also directly call sqlite

## Installation and API

Install it with

```bash
yarn add react-native-quick-sqlite
```

If using TypeORM, follow the instructions on the TypeORM, the apply the patch file from the example project.

If not using TypeORM, the package exposes the following API:

```typescript
interface ISQLite {
  open: (dbName: string) => any;
  close: (dbName: string) => any;
  executeSql: (
    dbName: string,
    query: string,
    params: any[] | undefined
  ) => {
    rows: any[];
    insertId?: number;
  };
}

// It is globally available in a variable called `sqlite`
sqlite.open({...options})
```

The JSI bindings expose this `sqlite` object in the global context, so you can directly call it from anywhere in the javascript context. The methods `throw` when an execution error happens, so `try ... catch` them.

## License

react-native-quick-sqlite is licensed under MIT.

Follow me on [twitter](https://twitter.com/ospfranco)!
