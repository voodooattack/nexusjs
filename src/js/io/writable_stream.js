(function() {
  const deviceKey = Symbol(), filtersKey = Symbol();
  return class WritableStream {
    constructor(device) {
      this[deviceKey] = device;
      this[filtersKey] = [];
    }
    get filters() { return this[filtersKey]; }
    get device() { return this[deviceKey]; }
    get eof() { return this.device.eof; }
    write(data) {
      return this.filters.reduce((prev, next) => prev.then(next.process.bind(next)), Promise.resolve(data))
        .then(this.device.write.bind(this.device));
    }
    writeSync(data) {
      this.filters.forEach(f => data = f.processSync(data));
      return this.device.writeSync(data);
    }
    pushFilter(...filters) {
      filters.forEach(f => this.filters.push(f));
    }
    popFilter() {
      return this.filters.pop();
    }
  };
})();
