# About

Nexus.js is a new take on server-side JavaScript, with the main goals being absolute performance and a modern, promise-based interface.

Some may shy away from a multi-threaded JavaScript implementation, preferring the security of a single-threaded event loop.

But, for some tasks – machine learning and live streaming included – multi-threading is a must, and this is why Nexus.js exists.

In a [recent benchmark](https://dev.to/voodooattack/introducing-nexusjs-a-multi-threaded-javascript-run-time-3g6), Nexus.js broke all the rules and served 1,000 requests per second on an old i7 machine. The aim of this project is to break such records methodically, and with regularity. 

Please take a moment to read about the Nexus.js architecture [here](architecture.md).

## Further Reading

You can read more on Nexus.js and the progress of development in the following articles:

* [Introduction](https://medium.com/@voodooattack/multi-threaded-javascript-introduction-faba95d3bd06)
* [Input/Output (I/O API Demonstration)](https://medium.com/@voodooattack/concurrent-javascript-part-ii-input-output-19c6dd3c6709)
* [Events (Promise-Based Concurrent Events On A Multi-threaded Scale)](https://medium.com/@voodooattack/concurrent-javascript-part-iii-events-7cba62f385b8)
* [Madness (Performance Comparison)](https://medium.com/@voodooattack/concurrent-javascript-part-iv-madness-edc1b8c7cc40)
* [The Mantra (Questions and Answers)](https://medium.com/@voodooattack/concurrent-javascript-part-v-the-mantra-bbdafcac2349)
* [Server (TCP API and Stress/Stability Testing)](https://medium.com/@voodooattack/concurrent-javascript-vi-server-9bb626f7cae1)
* [A year's Absence (An apology for disappearing for an entire year)](https://medium.com/p/concurrent-javascript-a-years-absence-ea5ae93d3b91) 

New Series:

* [Introducing Nexus.js: A multi-threaded JavaScript run-time (Overview of what's been implemented so far, and a benchmark where Nexus serves 1,000 HTTP requests per second)](https://dev.to/voodooattack/introducing-nexusjs-a-multi-threaded-javascript-run-time-3g6)

