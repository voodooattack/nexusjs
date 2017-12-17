# Nexus.IO

A namespace holding all Input/Output primitives.

## Interfaces
| Name | Implements | Description
| ----- | ---- | ---- |
| [`Device`](#nexusiodevice)| [`Nexus.EventEmitter`](emitter.md) | Base `Device` class. Implements [`Nexus.EventEmitter`](emitter.md) |
| [`Filter`](#nexusiofilter) | – | Responsible for manipulating data in a stream. |
| [`SourceDevice`](#nexusiosourcedevice) | `Nexus.IO.Device` | Base `SourceDevice` class. |
| [`SeekableDevice`](#nexusioseekabledevice) | `Nexus.IO.SourceDevice` | Base `SeekableDevice` class. |
| [`PullSourceDevice`](#nexusiopullsourcedevice) | `Nexus.IO.SourceDevice`, `Nexus.IO.SeekableDevice` | Base `PullSourceDevice` class. |
| [`PushSourceDevice`](#nexusiopushsourcedevice) | `Nexus.IO.SourceDevice`, `Nexus.IO.SeekableDevice` | Base `PushSourceDevice` class. |
| [`DualSeekableDevice`](#nexusiodualseekabledevice) | `Nexus.IO.SourceDevice` | Base `DualSeekableDevice` class: a bidirectional seekable device. |
| [`BidirectionalPushDevice`](#nexusiobidirectionalpushdevice) | `Nexus.IO.PushSourceDevice`, `Nexus.IO.SinkDevice` | Base `BidirectionalPushDevice` class: a bidirectional push device. |
| [`SinkDevice`](#nexusiosinkdevice) | `Nexus.IO.Device` | Base `SinkDevice` class. |

## Members
| Name | Implements | Description |
|----------| ---- | ----------- |
| [`FilePullDevice`](#nexusiofilepulldevice) | `Nexus.IO.PullSourceDevice` | Provides file I/O functionality. |
| [`FilePushDevice`](#nexusiofilepushdevice) | `Nexus.IO.PushSourceDevice` | An event-based file source device. |
| [`FileSinkDevice`](#nexusiofilesinkdevice) | `Nexus.IO.SinkDevice` | Responsible for consuming data in a stream. |
| [`ReadbaleStream`](#nexusioreadablestream) | [`Nexus.EventEmitter`](emitter.md) | Input stream class. |
| [`WritableStream`](#nexusiowritablestream) | [`Nexus.EventEmitter`](emitter.md) | Output stream class. |
| [`EncodingConversionFilter`](#nexusioencodingconversionfilter) | `Nexus.IO.Filter` | `Filter` for converting between encodings in a stream. |
| [`UTF8StringFilter`](#nexusioutf8stringfilter) | `Nexus.IO.Filter` | Utility `Filter` for converting UTF-8 buffers from strings and vice-versa. |


## Methods
| Signature | Description |
|----------| ----------- |

# Nexus.IO.Device

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `ready` | `boolean` | Whether or not the `Device` is ready. 

## Methods
| Signature | Description |
|----------| ----------- |
| `close(): Promise<void>` | Close the `Device`.

# Nexus.IO.SourceDevice

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `eof` | `boolean` | Whether or not end of file has been reached.
| `type` | `"pull" | "push"` | `SourceDevice` type.

## Methods
| Signature | Description |
|----------| ----------- |

# Nexus.IO.PushSourceDevice

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `state` | `"paused" | "resumed"` | `PushSourceDevice` state.

## Methods
| Signature | Description |
|----------| ----------- |
| `resume(): Promise<this>` | Resume reading. |
| `pause(): Promise<this>` | Pause reading. |
| `reset(): Promise<this>` | Reset reading position and pause the stream. |


## Events
| Event | Handler Signature | Description |
|----------| ----------- | ---- |
| `"data"` | `onData(data: ArrayBuffer): Promise<any>` | Fired whenever a read has been made.
| `"end"` | `onEnd(void): Promise<any>` | Fired when the source ends.
| `"error"` | `onError(error: Error): Promise<any>` | Fired whenever an error occurs.

# Nexus.IO.PullSourceDevice

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |

## Methods
| Signature | Description |
|----------| ----------- |
| `read(length: number): Promise<ArrayBuffer>` | Read `length` number of bytes, returning a promise. |
| `readSync(length: number): ArrayBuffer` | Read `length` number of bytes. |

# Nexus.IO.SeekableDevice

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |

## Methods
| Signature | Description |
|----------| ----------- |
| `seek(offset: number, origin: "begin" | "current" | "end"): Promise<number>` | Seek to `offset` from `origin`, returns a promise that resolves to the actual device offset. |
| `seekSync(offset: number, origin: "begin" | "current" | "end"): number` | Seek to `offset` from `origin`, returning the device offset. |

# Nexus.IO.DualSeekableDevice

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |

## Methods
| Signature | Description |
|----------| ----------- |
| `readSeek(offset: number, origin: "begin" | "current" | "end"): Promise<number>` | Seek the `read head` to `offset` from `origin`, returns a promise that resolves to the actual device read offset. |
| `readSeekSync(offset: number, origin: "begin" | "current" | "end"): number` | Seek the `read head` to `offset` from `origin`, returning the device read offset. |
| `writeSeek(offset: number, origin: "begin" | "current" | "end"): Promise<number>` | Seek the `write head` to `offset` from `origin`, returns a promise that resolves to the actual device write offset. |
| `writeSeekSync(offset: number, origin: "begin" | "current" | "end"): number` | Seek the `write head` to `offset` from `origin`, returning the device write offset. |

# Nexus.IO.SinkDevice

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |

## Methods
| Signature | Description |
|----------| ----------- |
| `write(buffer: ArrayBuffer|TypedArray): Promise<number>` | Write `buffer`, returning a promise that resolves with the number of bytes written on completion. |
| `writeSync(buffer: ArrayBuffer): number` | Write `buffer`, returning the byte length written. |

# Nexus.IO.Filter

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |

## Methods
| Signature | Description |
|----------| ----------- |
| `process(input: ArrayBuffer): Promise<ArrayBuffer>` | Process a buffer, returning a new buffer containing the output data. |
| `processSync(input: ArrayBuffer): ArrayBuffer` | Process a buffer, returning a new buffer containing the output data. |

# Nexus.IO.FilePullDevice

## Constructor
| Signature | Description |
|----------| ----------- |
| `new Nexus.IO.FilePullDevice(path: string)` | Construct using a file path.

# Nexus.IO.FilePushDevice

## Constructor
| Signature | Description |
|----------| ----------- |
| `new Nexus.IO.FilePullDevice(path: string)` | Construct using a file path.

# Nexus.IO.FileSinkDevice

## Constructor
| Signature | Description |
|----------| ----------- |
| `new Nexus.IO.FileSinkDevice(path: string)` | Construct using a file path.
| `new Nexus.IO.FileSinkDevice(fd: number)` | Construct using a file descriptor. Accepts `0` for `stdin`, `1` for `stdout`, and `2` for `stderr`

# Nexus.IO.ReadableStream

## Constructor
| Signature | Description |
|----------| ----------- |
| `new Nexus.IO.ReadableStream(source: SourceDevice)` | Construct using a source device. Device must be one of `pull` or `push` types.

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `device` | `SourceDevice` | The source device. |
| `filters` | `Filter[]` | Stream filters. |
| `eof` | `boolean` | End of file flag. |

## Methods
| Signature | Description |
|----------| ----------- |
| `read(length: number): Promise<ArrayBuffer>` | Read `length` bytes. `SourceDevice` must be `PullSourceDevice`. |
| `readSync(length: number): ArrayBuffer` | Read `length` bytes. `SourceDevice` must be `PullSourceDevice`. |
| `pipe(...targets: WritableStream[]): Function` | Pipe to `targets`, returns a `disconnect(): void` function.
| `pushFilters(...filters: Filter[]): void` | Add `filters` to the stream.
| `popFilter(): void` | Remove the most recent filter added to the stream.
| `close(): Promise<void>` | Close the source device.
| `resume(): Promise<this>` | Begin or resume streaming.
| `pause(): Promise<void>` | Pause streaming. Not supported for `PullSourceDevice`s.
| `reset(): Promise<void>` | Reset source device. Not supported for `PullSourceDevice`s.

## Events
| Event | Handler Signature | Description |
|----------| ----------- | ---- |
| `"data"` | `onData(data: ArrayBuffer): Promise<any>` | Fired after data has arrived and been processed through the filters.
| `"end"` | `onEnd(void): Promise<any>` | Fired when the streaming ends.
| `"error"` | `onError(error: Error): Promise<any>` | Fired whenever an error occurs.

# Nexus.IO.WritableStream

## Constructor
| Signature | Description |
|----------| ----------- |
| `new Nexus.IO.WritableStream(sink: SinkDevice)` | Construct using a sink device.

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `device` | `SinkDevice` | The sink device. |
| `filters` | `Filter[]` | Stream filters. |

## Methods
| Signature | Description |
|----------| ----------- |
| `write(buffer: ArrayBuffer|TypedArray): Promise<void>` | Write `buffer` to sink device. |
| `writeSync(buffer: ArrayBuffer|TypedArray): void` | Write `buffer` to sink device. (Synchronous) |
| `pushFilters(...filters: Filter[]): void` | Add `filters` to the stream.
| `popFilter(): void` | Remove the most recent filter added to the stream.
| `close(): Promise<void>` | Close the sink device.

## Events
| Event | Handler Signature | Description |
|----------| ----------- | ---- |
| `"error"` | `onError(error: Error): Promise<any>` | Fired whenever an error occurs.

# Nexus.IO.EncodingConversionFilter

## Constructor
| Signature | Description |
|----------| ----------- |
| `new Nexus.IO.EncodingConversionFilter(source: string, target: string)` | Construct using a `source` encoding and a `target` encoding.

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |

## Methods
| Signature | Description |
|----------| ----------- |