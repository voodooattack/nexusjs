const acceptor = new Nexus.Net.TCP.Acceptor();
acceptor.on('connection', (socket, endpoint) => {
  console.log(`connection from ${endpoint.address}:${endpoint.port}`);
  const rstream = new Nexus.IO.ReadableStream(socket);
  const wstream = new Nexus.IO.WritableStream(socket);
  const buffer = new Uint8Array(13);
  console.log(rstream);
  const message = 'Hello World!\n';
  for(let i = 0; i < 13; i++)
    buffer[i] = message.charCodeAt(i);
  rstream.pushFilter(new Nexus.IO.UTF8StringFilter());
  rstream.on('data', buffer => console.log(`got message: ${buffer}`));
  rstream.resume().catch(e => console.log(`client ${endpoint.address}:${endpoint.port} disconnected!`));
  console.log('sending greeting!');
  wstream.write(buffer);
});
acceptor.bind('127.0.0.1', 1984);
acceptor.listen();
console.log('server ready');
