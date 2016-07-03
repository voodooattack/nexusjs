const server = new Nexus.Net.HTTP.Server();

server.on('connection', connection => {
  console.inspect(connection.request);
  const rstream = new Nexus.IO.ReadableStream(connection.request);
  const wstream = new Nexus.IO.WritableStream(connection.response);
  rstream.pipe(wstream);
});

server.bind('127.0.0.1', 10001);
server.listen();
console.log('server ready');
