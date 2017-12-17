# Architecture

Nexus.js is built with multi-threading in mind, and as such it adopts a cooperative multi-threaded model.
Everything revolves around one primitive: the `Task`. Tasks are scheduled to a lock-free priority queue by various actions, including:

- Returning a `Promise`.
- Calling `Nexus.Scheduler.schedule()` with a callback.
- Calling `setTimeout` or `setInterval`.
- Using I/O streams.
- Using a `Nexus.EventEmitter` to handle events.

The `Scheduler` is responsible for managing and emptying the priority-queue. 

The following diagram illustrates how a Nexus.js process might look like during a session of heavy I/O processing.

```text
+--------------------+
|                    |       +------------+  +------------+
|                    +---+---> Thread 1   +--> JavaScript +---+
|      Scheduler     |   |   +------------+  +------------+   |
|                    |   +---> Thread 2   +--> JavaScript |   |
|                    |   |   +------------+  +------------+   |
+---------^----------+   +---> Thread 3   +--> I/O        |   |
          |              |   +------------+  +------------+   |
          |              +---> Thread 4   +--> JavaScript +---+
+---------+----------+   |   +------------+  +------------+   |
|       Task 1       |   +---> Thread 5   +--> I/O        +---+
+--------------------+   |   +------------+  +------------+   |
|       Task 2       |   +---> Thread 6   +--> I/O        |   |
+--------------------+   |   +------------+  +------------+   |
|       Task 3       |   +---> Thread 7   +--> JavaScript +---+
+--------------------+   |   +------------+  +------------+   |
|        ...         |   +---> Thread 8   +--> I/O        |   |
+--------------------+       +------------+  +------------+   |
|                    |                                        |
|                    |                                        |
|                    |                                        |
|      Priority      |                                        |
|      Queue         |                                        |
|                    |                                        |
|                    <----------------------------------------+
|                    |
+--------------------+
```

As you can see, here we have 8 threads spooled up and working to process JavaScript and I/O callbacks. There is no distinction between JavaScript and native I/O handlers as far as the `Scheduler` is concerned. All tasks will execute on all cores until the queue is empty, at which point the program will gracefully exit and return control to the operating system.

## Dynamic Balancing

The `Scheduler` uses a simple algorithm to balance threads, which exhibits the following behaviour:

- If the number of queued tasks is greater than zero, and the number of active threads is less than the maximum number of threads, start a thread.
- If the number of queued tasks is zero, and the I/O service has no tasks to poll, exit thread.
- If the I/O service stops, exit all threads.

What this means in essence, is that the `Scheduler` will dynamically balance threads depending on the workload it is presented with.

Let's look at a web server, for instance:

- At startup, and after the initial loading is done, it will spool one or two active threads.
- Depending on your implementation: it may remain idle in that state until requests begin to arrive.
- If, for instance, it is hit by 10 requests per second, the maximum number of threads will be started to serve requests.
- Once the requests stop coming, the extra threads will exit and it will return to the default state of one or two active threads on standby.

## Concurrency

The number of maximum threads can be controlled with the command-line flag: `concurrency`.

Example: `nexus --concurrency=4 program.js` will start `program.js` with a maximum of 4 processing threads. 

Note that JavaScriptCore may start its own garbage-collection threads in the background.

## Concurrent Variable Access

Unlike the V8 JavaScript engine used by Node.js, JavaScriptCore does not lock the entire virtual machine to all threads when you call into it in parallel.

Instead, it locks specific execution contexts, where a context is a stack pointer and a list of packaged closures.

This provides a much more granular locking behaviour, allowing completely unrelated execution contexts to run JavaScript in parallel, and at full speed.

When two different execution contexts that share a closure variable are entered in parallel, one will acquire a lock, and the other will wait for it to finish executing. 

The result is that all variables behave atomically in Nexus.js. Concurrent access to a global closure variable will not corrupt the variable, but will cause contention, which may affect the performance of your application negatively.

This is why the developer must be mindful of common synchronisation pitfalls:

- Incrementing or decrementing a counter should pose no problem and take no special considerations.
- If you try appending to an array in parallel, the items will be appended; but the order of insertion will be indeterminate thanks to the nature of multi-threading.
- Using promises with no closure variables will prevent most synchronisation pitfalls.

For further reading on the subject, please see this article: [Locking in WebKit](https://webkit.org/blog/6161/locking-in-webkit/).