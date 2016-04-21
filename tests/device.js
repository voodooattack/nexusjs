const reader = new Nexus.IO.FileSourceDevice('nexus');

reader.read(100).then(v => console.log(new Uint8Array(v)));
