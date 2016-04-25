const device = new Nexus.IO.FileSourceDevice('../../tests/utf16.txt');
const stream = new Nexus.IO.ReadableStream(device);

stream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-16LE", "UTF-8"));
stream.pushFilter(new Nexus.IO.UTF8StringFilter());

stream.read().then(v => console.log("buffer: " + v), e => console.error("error: " + e));
