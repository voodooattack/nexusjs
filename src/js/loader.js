(function() {
  const globalKey = Symbol();
  return class Loader {
    constructor(globalObject) {
      this[globalKey] = globalObject || {};
    }
    import(name, referrer) {
      return new Promise((resolve, reject) {
        
      });
    }
  }
})();
