const server = new Nexus.Net.HTTP.Server();

server.on('connection', async (connection, peer) => {
  const startTime = Date.now();
  const { response } = connection;
  response
    .status(200)
    .set('Content-Type', 'text/xml');
  const fileDevice = new Nexus.IO.FilePushDevice('enwik8');
  const inStream = new Nexus.IO.ReadableStream(fileDevice);
  const outStream = new Nexus.IO.WritableStream(response);
  inStream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-8", "UTF-16LE"));
  inStream.on('error', e => console.inspect(e));
  console.log(`converting 100MB to UTF-16 in real-time and piping to client ${
    peer.address}:${peer.port} with chunked encoding`);
  await inStream.pipe(outStream);
  console.log(`finished in ${(Date.now() - startTime) / 1000} seconds!`);
});

server.bind('127.0.0.1', 10000);
server.listen();

console.log('HTTP server listening at 127.0.0.1:10000');
