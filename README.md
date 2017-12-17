Nexus.js — Multi-threaded I/O for JavaScript.
=======

[![Join the chat at https://gitter.im/voodooattack/nexusjs](https://badges.gitter.im/voodooattack/nexusjs.svg)](https://gitter.im/voodooattack/nexusjs?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Nexus.js is a multi-threaded JavaScript run-time built on top of JavaScriptCore (Webkit) with a focus on high performance and dynamic scaling above all else.

Nexus.js uses an asynchronous, non-blocking I/O model, and a thread-pool scheduler to make the most of modern hardware concurrency.

Nexus.js is Promise-based and embraces ES6 in full; and as a result, it is not compatible with Node.js APIs.

Visit the homepage at [nexusjs.com](http://www.nexusjs.com).

## Building

Please check out the [documentation](docs/building.md) for a guide on how to build Nexus.js.

## Documentation

The early documentation is available at [nexusjs.com](http://www.nexusjs.com). It will change frequently as new features are added, so keep an eye out!
 
Also, you can look into the [tests directory](tests/) and the [examples directory](examples/) for an insight into how to use it.

## Native add-ons

While still a big topic for debate, native add-ons should be very feasible in the future, once a proper ABI is chosen. Please discuss this [here](https://github.com/voodooattack/nexusjs/issues/4).

## Contributing

All pull requests, suggestions, and questions are welcome.

## Read more

You can read more on Nexus.js and the progress of development in the following articles:

* [Introduction](https://medium.com/@voodooattack/multi-threaded-javascript-introduction-faba95d3bd06)
* [Input/Output (I/O API Demonstration)](https://medium.com/@voodooattack/concurrent-javascript-part-ii-input-output-19c6dd3c6709)
* [Events (Promise-Based Concurrent Events On A Multi-threaded Scale)](https://medium.com/@voodooattack/concurrent-javascript-part-iii-events-7cba62f385b8)
* [Madness (Performance Comparison)](https://medium.com/@voodooattack/concurrent-javascript-part-iv-madness-edc1b8c7cc40)
* [The Mantra (Questions and Answers)](https://medium.com/@voodooattack/concurrent-javascript-part-v-the-mantra-bbdafcac2349)
* [Server (TCP API and Stress/Stability Testing)](https://medium.com/@voodooattack/concurrent-javascript-vi-server-9bb626f7cae1)
* [A year's Absence (My apology for disappearing for an entire year)](https://medium.com/p/concurrent-javascript-a-years-absence-ea5ae93d3b91) 

New Series:

* [Introducing Nexus.js: A multi-threaded JavaScript run-time (Overview of what's been implemented so far, and a benchmark where Nexus serves 1,000 HTTP requests per second)](https://dev.to/voodooattack/introducing-nexusjs-a-multi-threaded-javascript-run-time-3g6)


## FAQ

* Will you implement `require()`?

> Not likely. Nexus.js will use the Promise-based `import(...)` API for dynamic loading, and otherwise use the `import` and `export` keywords for normal module loading.
> `require()` can still be implemented by a third-party in pure JavaScript of course, it just won't come built-in.

* Why are you avoiding `require()`? Are you planning on breaking all backward-compatibility with Node.js?

> Yes. I know the decision is harsh, but it will be better in the long run.
> It will make porting libraries harder, but the result will be a pure ES6 ecosystem with ES6 modules at its core.
> This is necessary because Nexus.js is multi-threaded, and most Node.js libraries use globals in one form or another, which means they'd be broken anyway.
> While accessing globals concurrently will not corrupt them or crash the program, it will produce unexpected behaviour in any event-loop based code. Since it assumes a single-threaded environment.

* How does concurrent access to variables work? Do you use a `mutex` for every variable?

> No, please [read the documentation](docs/architecture.md), and see [Locking in WebKit](https://webkit.org/blog/6161/locking-in-webkit/), it explains it better than I ever could.

* Can Nexus.js libraries override globals?

> The globals are created on-demand in every context that accesses them, and this makes it impossible to replace them.
> For example, `Nexus.EventEmitter` exists in every context, but if you replace it in a library it will not affect the `Nexus.EventEmitter` available in a different library, or in the main context.
>
> I do plan on offering certain hooks for transpiling utilities and the such. If you're using Babel to transpile JSX for an isomorphic (universal) application, you need not worry.

