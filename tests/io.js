const device = new Nexus.IO.FileSourceDevice('Makefile');
const stream = new Nexus.IO.ReadableStream(device);

stream.pushFilter(new Nexus.IO.EncodingConversionFilter("UTF-8", "UTF-8"));
stream.pushFilter(new Nexus.IO.UTF8StringFilter());

const p = stream.read(100000);

p.then(v => console.log("buffer: " + v));
