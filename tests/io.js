const reader = new Nexus.IO.FileSourceDevice('Makefile');

const promise1 = reader.seek(0, 'begin').then(v => reader.read(1000000));

promise1.then(v => console.log("p1: got output from device: length == ", new Uint8Array(v).length)).catch(e => console.error(e));
