const promises = [];
const startTime = Date.now();

for(let i = 0; i < 4; i++) {
  const device = new Nexus.IO.FileSourceDevice('enwik8');
  const stream = new Nexus.IO.ReadableStream(device);

  stream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-8", "UTF-16LE"));

  const wdevice = new Nexus.IO.FileSinkDevice('enwik16-' + i);
  const wstream = new Nexus.IO.WritableStream(wdevice);

  promises.push(stream.pipe(wstream));
}

Promise.all(promises).then(v => console.log(`finished in ${(Date.now() - startTime) / 1000} seconds!`));
