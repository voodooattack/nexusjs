(function() {
  const exportsKey = Symbol(), contextKey = Symbol();
  return class Module {
    get exports() { return this[exportsKey]; }
    set exports(value) { this[exportsKey] = value; }
    constructor(absolutePath) {
//       if (absolutePath) {
//         const context = this[contextKey] = new Nexus.Context();
//         const file = new Nexus.FileSystem.File(absolutePath, Nexus.FileSystem.OpenMode.Read);
//         const source = file.readAsStringSync();
//         context.eval(source, absolutePath, 1);
//         this.exports = context.exports;
//       } else
        this.exports = {};
    }
  }
})();
