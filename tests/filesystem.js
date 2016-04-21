const file = new Nexus.FileSystem.File("../../tests/filesystem.js", Nexus.FileSystem.OpenMode.Read);

console.log(new Uint8Array(file.readAsBufferSync()));

console.log(Nexus.FileSystem.stat("../../tests/filesystem.js").lastModified);
