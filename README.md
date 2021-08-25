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
<br />

Quick SQLite uses [JSI](https://formidable.com/blog/2019/jsi-jsc-part-2), removes all the overhead of intercommunication between JavaScript code and C++ code, making CRUDing entities from SQLite super fast!

Big ❤️ to [react-native-sqlite-storage](https://github.com/andpor/react-native-sqlite-storage) and [react-native-sqlite2](https://github.com/craftzdog/react-native-sqlite-2), this library also provides a WebSQL interface.

## GOTCHAS

- **It's not possible to use the browser debugger with JSI**, use [Flipper](https://github.com/facebook/flipper), on android Flipper also has an integrated database explorer you can use to debug your sqlite database, [you will have to configure your database path though](https://fbflipper.com/docs/setup/plugins/databases/).
![130516553-15c18d0f-65ad-44cf-8235-a4d6f41859e2](https://user-images.githubusercontent.com/1634213/130755919-7539d3dd-7d30-4234-9965-bfef2450ab0a.png)
- Your app will now include C++, you will need to install the NDK on your machine for android. (unless you know how to generate an AAR, feel free to open a PR)
- If you want to run the example project on android, you will have to change the paths on the android/CMakeLists.txt file, they are already there, just uncomment them.

## Use TypeORM

The recommended way to use this package is to use [TypeORM](https://github.com/typeorm/typeorm) with [patch-package](https://github.com/ds300/patch-package). TypeORM already has a sqlite-storage driver. In the `example` project on the `patch` folder you can a find a [patch for TypeORM](https://github.com/ospfranco/react-native-quick-sqlite/blob/main/example/patches/typeorm%2B0.2.31.patch), it basically just replaces all the `react-native-sqlite-storage` strings in TypeORM with `react-native-quick-sqlite`.

Follow the instructions to make TypeORM work with rn (enable decorators, configure babel, etc), then apply the patch via patch-package and you should be good to go.

## Low level API

It is also possible to directly execute SQL against the db:

```typescript
interface ISQLite {
  open: (dbName: string, location?: string) => any;
  close: (dbName: string, location?: string) => any;
  executeSql: (
    dbName: string,
    query: string,
    params: any[] | undefined
  ) => {
    rows: any[];
    insertId?: number;
  };
}
```

In your code

```typescript
// If you want to register the (globalThis) types for the low level API do an empty import
import 'react-native-quick-sqlite';

// `sqlite` is a globally registered object, so you can directly call it from anywhere in your javascript
// The methods `throw` when an execution error happens, so try/catch them
try {
  sqlite.open('myDatabase', 'databases');
} catch (e) {
  console.log(e); // [react-native-quick-sqlite]: Could not open database file: ERR XXX
}
```

## JSI Cheatsheet

If you want to learn how to make your own JSI module and also get a reference guide for all things C++/JSI you can buy my [JSI/C++ Cheatsheet](http://ospfranco.gumroad.com/l/IeeIvl)

## License

react-native-quick-sqlite is licensed under MIT.
