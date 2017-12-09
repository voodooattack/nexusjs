// import { h } from 'https://unpkg.com/preact/dist/preact.esm.js';
const server = new Nexus.Net.HTTP.Server();

function createInputStream() {
  const fileDevice = new Nexus.IO.FilePushDevice('enwik8');
  const inStream = new Nexus.IO.ReadableStream(fileDevice);
  inStream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-8", "UTF-16LE"));
  inStream.on('error', e => console.error(e));
  return inStream;
}

server.on('connection', async (connection, peer) => {
  const startTime = Date.now();
  const { response } = connection;
  response
    .status(200)
    .set('Server', 'nexus.js/0.1.1')
    .set('Content-Type', 'text/xml');
  const outStream = new Nexus.IO.WritableStream(response);
  console.log(`converting 100MB to UTF-16 and piping to client ${
    peer.address}:${peer.port} with chunked encoding`);
  const inStream = createInputStream();
  inStream.pipe(outStream);
  await inStream.resume();
  console.log(`finished in ${(Date.now() - startTime) / 1000} seconds!`);
});

const ip = '127.0.0.1';
const port = 10000;

server.bind(ip, port);
server.listen();

console.log(`HTTP server listening at ${ip}:${port}`);
