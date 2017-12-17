# Nexus.Scheduler

The `Scheduler` is the lowest-level multi-threading primitive in Nexus.js

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `threadId`  | string | The string ID of the thread executing the current function. |
| `concurrency` | number | The maximum number of threads Nexus.js can use. |

## Methods

| Signature | Description |
|----------| ----------- |
| `schedule(task: Function): void` | Schedule a new task on the thread pool. |
