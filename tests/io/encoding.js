async function start() {
  const startTime = Date.now();
  try {
    const device = new Nexus.IO.FilePushDevice('enwik8');
    const stream = new Nexus.IO.ReadableStream(device);

    stream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-8", "UTF-16LE"));

    const wstreams = [0,1,2,3]
      .map(i => new Nexus.IO.WritableStream(new Nexus.IO.FileSinkDevice('enwik16-' + i)));

    console.log('piping...');

    stream.pipe(...wstreams);

    console.log('streaming...');

    await stream.resume();

    await stream.close();
    await Promise.all(wstreams.map(stream => stream.close()));

    console.log(`finished in ${(Date.now() - startTime) / 1000} seconds!`);
  } catch (e) {
    console.error('An error occurred: ', e);
  }
}

start().catch(console.error);