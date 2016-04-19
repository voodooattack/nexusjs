const file = new FileSystem.File("../../tests/filesystem.js", FileSystem.OpenMode.Read);

console.log(new Uint8Array(file.readAsBufferSync()));
