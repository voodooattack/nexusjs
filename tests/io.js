const device = new Nexus.IO.FileSourceDevice('../../tests/utf16.txt');
const stream = new Nexus.IO.ReadableStream(device);

stream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-16LE", "UTF-8"));

const wdevice = new Nexus.IO.FileSinkDevice('utf8.txt');
const wstream = new Nexus.IO.WritableStream(wdevice);

stream.pipe(wstream).then(console.log('finished!'));
