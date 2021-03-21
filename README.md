# react-native-quick-sqlite

First a story. You start developing your mobile app, you go for your usual state management framework (redux/mobx/recoil/etc). Everything works great, but, as months go by, 80% of your code is just moving stuff from the server into your app (request stuff, normalize it, untangle it, put it somewhere). You keep re-inventing the wheel over and over again. Then you also start getting some complaints, the app has become too slow. As you start reaching thousands of objects your simple persistance to strings on disk starts breaking down, everything from app start to rendering large lists is super slow.

You then realize you need to use some big boy database tools, yet your options are somewhat limited. You could go with something like [realmDB](https://realm.io) or [firebase](https://firebase.google.com/docs/firestore) or [AWS Amplify](https://aws.amazon.com/amplify/) which sounds good, until you realize you are no longer in control of your backend and your database. You could go with [WatermelonDB](https://github.com/Nozbe/WatermelonDB) which is slightly better, but even though it claims to be lazy, its API effectively blocks your UI from rendering.

**react-native-quick-sqlite** aims to solve all of the issues once and for all.

1. **It is the FASTEST**, not only because it's lazy (like 🍉), not only because it is written in C++, not only because it uses SQLite, but also because it uses react-native JSI bindings which removes almost all the overhead of javascript <-> C++ communication. It has both a sync and async API.
2. **It is RELATIONAL**, don't mess with noSQL, it will bite you in the ass sooner or later, applications nowadays are more complex than ever, you need to join, query and sort complex sets of data.
3. **It SYNCS**, via a simple sync mechanism (like 🍉), do not waste time/code mutating/storing/transmiting state, most importantly, it let's you in control of your application, no cloud vendor lock, do it via REST, or graphQL or sockets, use PostgreSQL, use mySQL, whatever you want.
4. **It sits on the right level of abstraction**, if you really need to optimize queries you can hand write performance-tuned SQL, otherwise you can build simple queries with (or even re-use your server queries) with typeORM.

## Installation

```sh
yarn add react-native-quick-sqlite
```

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

react-native-quick-sqlite is dual licensed, this open source version is SSPL, it is the license created by mongoDB, similar to AGPLv3, the TL:DR: is, you can use it in your project without a cost, however you cannot offer it as a service without fully open sourcing it, basically you cannot monetize this project as a service to third parties.

The other possibility is to buy a license from me for you to use this project with a more permissive MIT license, reach out if you want to discuss. I can also add features, provide consulting service and/or support for a fee.
