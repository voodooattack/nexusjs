(function() {
  const globalKey = Symbol();
  return class Loader {
    constructor(globalObject) {
      this[globalKey] = globalObject || {};
    }
    resolve(name, referrer) {
      return new Promise((resolve, reject) {
        
      });
    }
    import(name, referrer) {
      return new Promise((resolve, reject) {

      });
    }
  }
})();
