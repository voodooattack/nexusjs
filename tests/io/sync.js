const fileSource = new Nexus.IO.FilePullDevice(import.meta.filename);
const buffer = fileSource.readSync(1024);
const decoder = new Nexus.IO.UTF8StringFilter();
const text = decoder.processSync(buffer);

console.log(text);