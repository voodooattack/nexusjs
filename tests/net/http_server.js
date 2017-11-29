const server = new Nexus.Net.HTTP.Server();

server.on('connection', connection => {
  const { request, response } = connection;
  console.inspect(request);
  request.on('header', (size, extensions, error) => {
    console.log('header: ');
    console.inspect({ size, extensions, error });
  });
  request.on('body', (remaining, body, error) => {
    console.log('body: ');
    console.inspect({ remaining, body, error });
    return remaining;
  });
  const device = new Nexus.IO.FilePushDevice('enwik8');
  const htmlStream = new Nexus.IO.ReadableStream(device);
  const wstream = new Nexus.IO.WritableStream(response);

  htmlStream.pipe(wstream);
});

server.bind('127.0.0.1', 10000);
server.listen();
console.log('server ready');
