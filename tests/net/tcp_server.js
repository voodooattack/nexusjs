const acceptor = new Nexus.Net.TCP.Acceptor();
acceptor.on('connection', (socket, endpoint) => {
  console.log(`connection from ${endpoint.address}:${endpoint.port}`);
  const stream = new Nexus.IO.WritableStream(socket);
  const buffer = new Uint8Array(13);
  const message = 'Hello World!\n';
  for(let i = 0; i < 13; i++)
    buffer[i] = message.charCodeAt(i);
  console.log('sending greeting!');
  stream.write(buffer);
  socket.on('data', buffer => console.log(`got message: ${String.fromCharCode.apply(null, new Uint8Array(buffer))}`));
  socket.resume().catch(e => console.log(`client ${endpoint.address}:${endpoint.port} disconnected!`));
});
acceptor.bind('127.0.0.1', 1984);
acceptor.listen();
