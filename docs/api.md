# API

Since Nexus.js is ES6 module-based, it does not pollute the package namespace with built-in packages. All built-in APIs are accessible via the `Nexus` global.

Note: the API is lazy-loaded on demand.

## Members
| Member | Type | Description |
|----------| ---- | ----------- |
| `Scheduler` | (Instance) | [Nexus.Scheduler](api/scheduler.md) |
| `EventEmitter` | (Class) | [Nexus.EventEmitter](api/emitter.md) |
| `FileSystem` | (Namespace) | [Nexus.FileSystem](api/fs.md) |
| `IO` | (Namespace) | [Nexus.IO](api/io.md) |
| `Net` | (Namespace) | [Nexus.Net](api/net.md) |

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `version`  | string | The version of the Nexus.js binary. |
| `script` | string | The full path to the entry point script used to invoke the Nexus.js process. |


## Other Globals

* [WebAssembly](./api/wasm.md)

