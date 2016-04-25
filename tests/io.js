const device = new Nexus.IO.FileSourceDevice('enwik8');
const stream = new Nexus.IO.ReadableStream(device);

stream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-8", "UTF-16LE"));

const wdevice = new Nexus.IO.FileSinkDevice('enwik16');
const wstream = new Nexus.IO.WritableStream(wdevice);

stream.pipe(wstream).then(v => console.log('finished!'));
