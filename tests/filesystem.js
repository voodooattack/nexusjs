const file = new FileSystem.File("../../tests/filesystem.js", FileSystem.OpenMode.Read);

file.readAsString('UTF8').then(v => console.log("~" + v + "~")).catch(e => console.log("error " + e));
