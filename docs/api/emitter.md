# Nexus.EventEmitter

A promise-based event emitter.

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |

## Methods

| Signature | Description |
|----------| ----------- |
| `on(event: string, handler: Function): void` | Register a `handler` to handle `event` any number of times. |
| `once(event: string, handler: Function): void` | Register a `handler` to handle `event` exactly once. |
| `many(event: string, handler: Function, count: number): void` | Register a `handler` to handle `event` a `count` of times. |
| `off(event: string, handler: Function): void` | Remove `handler` for `event`. |
| `allOff(event: string): void` | Remove all handlers for `event`. |
| `emit(event: string, ...args: any[]): Promise<any[]>` | Emit `event` with arguments `...args`, returning a `Promise` that resolves to all values returned by the handlers. | 

