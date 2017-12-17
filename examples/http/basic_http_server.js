/** vt100 console escape codes */

const Reset = "\x1b[0m";
const Bright = "\x1b[1m";
const Dim = "\x1b[2m";
const Underscore = "\x1b[4m";
const Blink = "\x1b[5m";
const Reverse = "\x1b[7m";
const Hidden = "\x1b[8m";

const FgBlack = "\x1b[30m";
const FgRed = "\x1b[31m";
const FgGreen = "\x1b[32m";
const FgYellow = "\x1b[33m";
const FgBlue = "\x1b[34m";
const FgMagenta = "\x1b[35m";
const FgCyan = "\x1b[36m";
const FgWhite = "\x1b[37m";

const BgBlack = "\x1b[40m";
const BgRed = "\x1b[41m";
const BgGreen = "\x1b[42m";
const BgYellow = "\x1b[43m";
const BgBlue = "\x1b[44m";
const BgMagenta = "\x1b[45m";
const BgCyan = "\x1b[46m";
const BgWhite = "\x1b[47m";

class NotFoundError extends Error {
  constructor(path) {
    super(`Not Found: '${path}'`);
    this.code = 404;
  }
}

class InternalServerError extends Error {
  constructor(message) {
    super(`Internal Server Error: ${message}`);
    this.code = 500;
  }
}

/** Figure out the mime type from the file extension */
function mimeType(path) {
  const ext = path.substr(path.lastIndexOf('.') + 1);
  const mimeTypes = {
    '/': 'text/html',
    htm: 'text/html',
    html: 'text/html',
    css: 'text/css',
    png: 'image/png',
    jpg: 'image/jpeg',
    svg: 'image/svg',
    ttf: 'font/ttf',
    woff: 'font/woff',
    woff2: 'font/woff',
    js: 'application/javascript',
    json: 'application/json'
  };
  return mimeTypes[ext] || 'application/octet-stream';
}

/**
 * Creates an input stream from a path.
 * @param path
 * @returns {Promise<ReadableStream>}
 */
async function createInputStream(path) {
  if (path.startsWith('/')) // If it starts with '/', omit it.
    path = path.substr(1);
  if (path.startsWith('.')) // If it starts with '.', reject it.
    throw new NotFoundError(path);
  if (path === '/' || !path) // If it's empty, set to index.html.
    path = 'index.html';
  /**
   * `import.meta.dirname` and `import.meta.filename` replace the old CommonJS `__dirname` and `__filename`.
   */
  const filePath = Nexus.FileSystem.join(import.meta.dirname, 'site', path);
  try {
    // Stat the target path.
    const {type} = await Nexus.FileSystem.stat(filePath);
    if (type === Nexus.FileSystem.FileType.Directory) // If it's a directory, return its 'index.html'
      return createInputStream(Nexus.FileSystem.join(path, 'index.html'));
    else if (type === Nexus.FileSystem.FileType.Unknown || type === Nexus.FileSystem.FileType.NotFound)
      // If it's not found, throw NotFound.
      throw new NotFoundError(path);
  } catch(e) {
    if (e.code)
      throw e;
    throw new NotFoundError(path);
  }
  try {
    // First, we create a device.
    const fileDevice = new Nexus.IO.FilePushDevice(filePath);
    // Then we return a new ReadableStream created using our source device.
    return new Nexus.IO.ReadableStream(fileDevice);
  } catch(e) {
    throw new InternalServerError(e.message);
  }
}

const urlPattern = new RegExp("^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?");

/**
 * Parse a URL, returning its parts.
 * @param url
 * @returns {*}
 */
function parseURL(url) {
  const matches = url.match(urlPattern);
  if (matches)
    return {
      scheme: matches[2],
      authority: matches[4],
      path: matches[5],
      query: matches[7],
      fragment: matches[9]
    };
  else
    return undefined;
}

/**
 * Connections counter.
 */
let connections = 0;

/**
 * Create a new HTTP server.
 * @type {Nexus.Net.HTTP.Server}
 */
const server = new Nexus.Net.HTTP.Server();

// A server error means an error occurred while the server was listening to connections.
// We can mostly ignore such errors, we display them anyway.
server.on('error', e => {
  console.error(FgRed + Bright + 'Server Error: ' + e.message + '\n' + e.stack, Reset);
});

/**
 * Listen to connections.
 */
server.on('connection', async (connection, peer) => {
  // Start with a connection ID of 0, increment with every new connection.
  const connId = connections++;
  // Record the start time for this connection.
  const startTime = Date.now();
  // Destructuring is supported, why not use it?
  const { request, response } = connection;
  // Parse the URL parts.
  const { path } = parseURL(request.url);
  // Here we'll store any errors that occur during the connection.
  const errors = [];
  // inStream is our ReadableStream file source, outStream is our response (device) wrapped in a WritableStream.
  let inStream, outStream;
  try {
    // Log the request.
    console.log(`> #${FgCyan + connId + Reset} ${Bright + peer.address}:${peer.port + Reset} ${
      FgGreen + request.method + Reset} "${FgYellow}${path}${Reset}"`, Reset);
    // Set the 'Server' header.
    response.set('Server', `nexus.js/${Nexus.version}`);
    // Create our input stream.
    inStream = await createInputStream(path);
    // Create our output stream.
    outStream = new Nexus.IO.WritableStream(response);
    // Hook all `error` events, add any errors to our `errors` array.
    inStream.on('error', e => { errors.push(e); });
    request.on('error', e => { errors.push(e); });
    response.on('error', e => { errors.push(e); });
    outStream.on('error', e => { errors.push(e); });
    // Set content type and request status.
    response
      .set('Content-Type', mimeType(path))
      .status(200);
    // Hook input to output(s).
    const disconnect = inStream.pipe(outStream);
    try {
      // Resume our file stream, this causes the stream to switch to HTTP chunked encoding.
      // This will return a promise that will only resolve after the last byte (HTTP chunk) is written.
      await inStream.resume();
    } catch (e) {
      // Capture any errors that happen during the streaming.
      errors.push(e);
    }
    // Disconnect all the callbacks created by `.pipe()`.
    return disconnect();
  } catch(e) {
    // If an error occurred, push it to the array.
    errors.push(e);
    // Set the content type, status, and write a basic message.
    response
      .set('Content-Type', 'text/plain')
      .status(e.code || 500)
      .send(e.message || 'An error has occurred.');
  } finally {
    // Close the streams manually. This is important because we may run out of file handles otherwise.
    if (inStream)
      await inStream.close();
    if (outStream)
      await outStream.close();
    // Close the connection, has no real effect with keep-alive connections.
    await connection.close();
    // Grab the response's status.
    let status = response.status();
    // Determine what colour to output to the terminal.
    const statusColors = {
      '200': Bright + FgGreen, // Green for 200 (OK),
      '404': Bright + FgYellow, // Yellow for 404 (Not Found)
      '500': Bright + FgRed // Red for 500 (Internal Server Error)
    };
    let statusColor = statusColors[status];
    if (statusColor)
      status = statusColor + status + Reset;
    // Log the connection (and time to complete) to the console.
    console.log(`< #${FgCyan + connId + Reset} ${Bright + peer.address}:${peer.port + Reset} ${
      FgGreen + request.method + Reset} "${FgYellow}${path}${Reset}" ${status} ${(Date.now() - startTime)}ms` +
      (errors.length ? " " + FgRed + Bright + errors.map(error => error.message).join(', ') + Reset : Reset));
  }
});

/**
 * IP and port to listen on.
 */
const ip = '0.0.0.0', port = 3000;
/**
 * Whether or not to set the `reuse` flag. (optional, default=false)
 */
const portReuse = true;
/**
 * Maximum allowed concurrent connections. Default is 128 on my system. (optional, system specific)
 * @type {number}
 */
const maxConcurrentConnections = 1000;
/**
 * Bind the selected address and port.
 */
server.bind(ip, port, portReuse);
/**
 * Start listening to requests.
 */
server.listen(maxConcurrentConnections);
/**
 * Happy streaming!
 */
console.log(FgGreen + `Nexus.js/${Nexus.version} HTTP server listening at ${ip}:${port}` + Reset);
