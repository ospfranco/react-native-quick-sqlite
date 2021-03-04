# react-native-sequel

Let me paint a picture for you, you start developing your mobile app, you go for your usual state management framework (redux/mobx/recoil/etc). Everything looks fine at first, but as the months go by 80% of your code is just moving stuff from the server into your app (request stuff, normalize it, untangle it, put it somewhere), you keep re-inventing the wheel over and over again. Then you are also start getting some complaints, the app is too slow, as you start reaching thousands of objects your simple persistance to strings on disks start breaking down, everything from app start to rendering lists slow downs to a crawl.

You then realize you need to use some big boy database tools, yet your options are somewhat limited. You could go with something like [realmDB]() or [firebase]() which sounds good, until you realize you need to hand over the keys of the kingdom to a third party. You could go with [WatermelonDB]() which is slightly better, but it has many pitfalls, even though it claims to be lazy it's API effectively blocks your UI from rendering, forcing you to change how you code.

**React-native-sequel** aims to solve all of the issues once and for all.

1) **It is the FASTEST (and lazy)**, not only because it is written in C++, it uses react-native JSI bindings which completely removes almost all the overhead of javascript <-> C++ communication. It has both a sync and async API, which allows you to fetch data from disk on demand (at blazing speeds!).
2) **It is relational (SQLite under the hood, ACID complaint)**, don't mess with noSQL, it will bite you in the ass sooner or later, applications nowadays are more complex than ever, you need to join, query and sort complex sets of data.
3) **It provides a sane sync mechanism**, do not waste your time mutating/storing state, most importantly, it let's you in control of your application.
4) **It sits on the right level of abstraction**, if you really need to optimize queries you can hand write performance-tuned SQL, otherwise you can build simple queries with (or even re-use your server queries) with typeORM.

## Installation

```sh
yarn add react-native-sequel
```

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

React-native-sequel is dual licensed, this open source version is SSPL, it is the license created by mongoDB, similar to AGPLv3, the TL:DR: is, you can use it in your project without a cost, however you cannot offer it as a service without fully open sourcing it, basically you cannot monetize this project as a service to third parties.

The other possibility is to buy a license from me for you to use this project with a more permissive MIT license, reach out if you want to discuss. I can also add features, provide consulting service or provide support for a fee.

