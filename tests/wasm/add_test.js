const device = new Nexus.IO.FilePushDevice(Nexus.FileSystem.join(import.meta.dirname, 'add.wasm'));
const stream = new Nexus.IO.ReadableStream(device);

let buffer;

stream.on('data', async data => {
  const arr = new Uint8Array(data);
  const newBuffer = new Uint8Array(arr.byteLength + (buffer ? buffer.byteLength : 0));
  if (buffer)
    newBuffer.set(buffer, 0);
  newBuffer.set(arr, buffer ? buffer.byteLength : 0);
  buffer = newBuffer;
});

stream.on('end', async () => {
  try {
    console.log(`compiling wasm, binary size: ${buffer.byteLength} bytes`);
    const memory = new WebAssembly.Memory({ initial: 256, maximum: 256  });
    const table = new WebAssembly.Table({ initial: 0, maximum: 0, element: 'anyfunc' });
    const module = await WebAssembly.compile(buffer);
    const importObject = { env: { memory, table, memoryBase: 0, tableBase: 0 } };
    const exports = await WebAssembly.instantiate(module, importObject);
    console.log('exports.add(3,3) =', exports.exports.add(3, 3));
  } catch (e) {
    console.error(e);
  }
});

stream.resume();