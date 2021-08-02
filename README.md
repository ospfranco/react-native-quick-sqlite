<h1 align="center">React Native Quick SQLite</h1>

<h3 align="center">The **fastest** SQLite implementation for react-native.</h3>

![Frame 2](https://user-images.githubusercontent.com/1634213/127499575-aed1d0e2-8a93-42ab-917e-badaab8916f6.png)

<div align="center">
  <pre align="center">
    <a href="https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch">Copy typeORM patch-package from example dir</a>
    npm i react-native-quick-sqlite typeorm
    npx pod-install
    <a href="https://dev.to/vinipachecov/setup-typeorm-with-react-native-50c4">Enable decorators and configure babel</a>
  </pre>
  <a align="center" href="https://github.com/ospfranco?tab=followers">
    <img src="https://img.shields.io/github/followers/ospfranco?label=Follow%20%40ospfranco&style=social" />
  </a>
  <br />
  <a align="center" href="https://twitter.com/ospfranco">
    <img src="https://img.shields.io/twitter/follow/ospfranco?label=Follow%20%40ospfranco&style=social" />
  </a>
</div>

Quick SQLite uses [JSI bindings](https://formidable.com/blog/2019/jsi-jsc-part-2/), removes all the overhead of intercommunication between JavaScript code and C++ code, making CRUDing entities from SQLite super fast!

Big ❤️ to [react-native-sqlite-storage](https://github.com/andpor/react-native-sqlite-storage) and [react-native-sqlite2](https://github.com/craftzdog/react-native-sqlite-2) for paving the way, this library should offer a similar API.

## GOTCHAS

- **JSI bindings are not available when connected to the web browser debugger**, from now on you should use [Flipper](https://github.com/facebook/flipper).
- Your app will now include C++ files, this means you will need to install the NDK on your machine for android.
- [ ] TODO: **Everything is saved on the application's home folder**. I got lazy and ignore any passed path parameter, if you need this feel free to open a PR. 
- [x] TODO: **When parsing the SQLite rows the strings are parsed as ASCII strings instead of UTF-8**, I'm not a C++ expert, I don't know if this is done correctly, can C++ guru check my work? :D
- [ ] TODO: **The API is NOT 100% the same as sqlite-storage**, sqlite-storage returns an `item` function on the query result, it takes an index number and returns an object, I simply return an array, because creating deeply nested structures from C++ is somewhat cumbersome.

# Use TypeORM

The recommended way to use this package is to use [TypeORM](https://github.com/typeorm/typeorm) with [patch-package](https://github.com/ds300/patch-package). TypeORM already has a sqlite-storage driver. In the `example` project on the `patch` folder you can a find a [patch for TypeORM](https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch), it basically just replaces all the `react-native-sqlite-storage` strings in TypeORM with `react-native-quick-sqlite`, and does a tiny change for the way it accesses the resulting rows from the SQL execution.

Follow the instructions to make TypeORM work with rn (enable decorators, configure babel, etc), then apply the patch via patch-package and you should be good to go.

If you are particularly masochistic you can hand write SQL and query the database directly yourself.

## Low level API

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

`sqlite` is a globally registered object, so you can directly call it from anywhere in your javascript. The methods `throw` when an execution error happens, so `try ... catch` them.

## License

react-native-quick-sqlite is licensed under MIT.
