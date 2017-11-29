async function start() {
  const startTime = Date.now();

  const device = new Nexus.IO.FilePushDevice('enwik8');
  const stream = new Nexus.IO.ReadableStream(device);

  stream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-8", "UTF-16LE"));

  const wstreams = [0,1,2,3].map(i => new Nexus.IO.WritableStream(new Nexus.IO.FileSinkDevice('enwik16-' + i)));

  await stream.pipe(...wstreams);

  console.log(`finished in ${(Date.now() - startTime) / 1000} seconds!`);
}

start().catch(console.error);