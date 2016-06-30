const server = new Nexus.Net.HTTP.Server();

server.on('connection', connection => {
  console.inspect(connection.request);
});

server.bind('127.0.0.1', 10000);
server.listen();
console.log('server ready');
