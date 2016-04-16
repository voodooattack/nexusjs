(function() {
  const loaderKey = Symbol();
  return class Module {
    constructor(pathOrPackage) {
      if (pathOrPackage) {
        this.exports = System.importSync(pathOrPackage);
      } else
        this.exports = {};
    }
  }
})();
