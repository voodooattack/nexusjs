const file = new FileSystem.File("test_file", FileSystem.OpenMode.Read);

file.readAsString().then(v => console.log(v));
