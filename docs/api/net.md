# Nexus.Net

A namespace holding all the networking primitives.

## Interfaces
| Name | Implements | Description
| ----- | ---- | ---- |
| [`SocketDevice`](#nexusnetsocketdevice) | [`Nexus.IO.BidirectionalPushDevice`](io.md#nexusiobidirectionalpushdevice) | Bidirectional socket device. |
| [`Acceptor`](#nexusnetacceptor) | [`Nexus.EventEmitter`](emitter.md) | Base `Acceptor` class. |
| [`PeerInfo`](#nexusnetpeerinfo) | – | Object holding peer information. |

## Members
| Name | Implements | Description
| ----- | ---- | ---- |
| [`UDPSocket`](#nexusnetudpsocket) | `Nexus.Net.SocketDevice`| Bidirectional UDP socket device. |
| [`TCP`](#nexusnettcp) | (Namespace) | TCP namespace. |
| [`HTTP`](#nexusnethttp) | (Namespace) | HTTP namespace. |

# Nexus.Net.SocketDevice

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `available` | number | number of bytes available to read. |

## Methods
| Signature | Description |
|----------| ----------- |
| `connect(ip: string, port: number): Promise<SocketDevice>` | Connect to `ip` at `port`. |
| `close(): void` | Close the socket.
| `cancel(): void` | Cancel the connection.

# Nexus.Net.UDPSocket

## Constructor
| Signature | Description |
|----------| ----------- |
| `new Nexus.IO.UDPSocketDevice()` | Default constructor.

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |

## Methods
| Signature | Description |
|----------| ----------- |

# Nexus.Net.PeerInfo

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `address` | `string` | The remote IP address of the connection. | 
| `port` | `number` | The remote port of the connection. | 

# Nexus.Net.Acceptor

## Methods
| Signature | Description |
|----------| ----------- |
| `bind(address: string, port: number)` | Bind an `address` and a `port`.
| `listen(maxConcurrentConnections?: number)` | Listen to connections. `maxConcurrentConnections` specifies the maximum number of concurrent connections (optional, system specific)

# Nexus.Net.TCP

## Members
| Name | Implements | Description
| ----- | ---- | ---- |
| [`Acceptor`](#nexusnettcpacceptor) | [`Nexus.Net.Acceptor`](#nexusnetacceptor) | `TCPAcceptor` class. Responsible for listening for and accepting new TCP connections.

# Nexus.Net.TCP.Acceptor

## Events

| Event | Handler Signature | Description |
|----------| ----------- | ---- |
| `"connection"` | `onConnection(connection: SocketDevice, peer: PeerInfo): Promise<any>` | Fired whenever a new connection has been accepted.
| `"error"` | `onError(error: Error): Promise<any>` | Fired whenever an error occurs.

# Nexus.Net.HTTP

This namespace holds interfaces pertaining to the HTTP server/client functionality.

## Members
| Name | Implements | Description
| ----- | ---- | ---- |
| [`Server`](#nexusnethttpserver) | [`Nexus.Net.TCP.Acceptor`](#nexusnettcpacceptor) | `HTTPAcceptor` class. Responsible for listening for and accepting new HTTP connections.
| [`Connection`](#nexusnethttpconnection) | `Nexus.Net.SocketDevice` | A HTTP connection.
| [`Request`](#nexusnethttprequest) | [`Nexus.IO.PushSourceDevice`](io.md#nexusiopushsourcedevice) | A `Request` object implementing `Nexus.IO.PushSourceDevice`.
| [`Response`](#nexusnethttpresponse) | [`Nexus.IO.SinkDevice`](io.md#nexusiosinkdevice) | A `Response` object implementing `Nexus.IO.SinkDevice`.

# Nexus.Net.HTTP.Server

## Events

| Event | Handler Signature | Description |
|----------| ----------- | ---- |
| `"connection"` | `onConnection(connection: Nexus.Net.HTTP.Connection, peer: PeerInfo): Promise<any>` | Fired whenever a new connection has been accepted. After the basic HTTP headers have been parsed.
| `"error"` | `onError(error: Error): Promise<any>` | Fired whenever an error occurs.

# Nexus.Net.HTTP.Connection

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `request` | `Nexus.Net.HTTP.Request` | The `request` object. | 
| `response` | `Nexus.Net.HTTP.Response` | The `response` object. | 

# Nexus.Net.HTTP.Request

The request object of a HTTP connection, implements a [`Nexus.IO.PushSourceDevice`](io.md#nexusiopushsourcedevice).

## Properties
| Property | Type | Description |
|----------| ---- | ----------- |
| `method` | `string` | The HTTP method. | 
| `version` | `string` | The HTTP version. | 
| `headers` | `{ name: string, value: string}[]` | An array containing the HTTP headers in `{ name, value}` pairs. | 

## Events

| Event | Handler Signature | Description |
|----------| ----------- | ---- |
| `"data"` | `onData(data: ArrayBuffer): Promise<any>` | Fired when data has arrived.
| `"end"` | `onEnd(void): Promise<any>` | Fired when the request ends.
| `"error"` | `onError(error: Error): Promise<any>` | Fired whenever an error occurs.

# Nexus.Net.HTTP.Response

The response object of a HTTP connection, implements a [`Nexus.IO.SinkDevice`](io.md#nexusiosinkdevice).

Piping directly to a `Nexus.Net.HTTP.Response` object will cause it to enter chunked-encoding mode.

## Methods

| Signature | Description |
|----------| ----------- |
| `status(code?: number): number|this` | Set the status code to `code`, returning `this`. If `code` is not passed, returns the current status code. |
| `set(name: string, value: string): this` | Sets the header `name` to `value`.
| `send(body: string): this` | Send a string response.

## Events

| Event | Handler Signature | Description |
|----------| ----------- | ---- |
| `"error"` | `onError(error: Error): Promise<any>` | Fired whenever an error occurs.

