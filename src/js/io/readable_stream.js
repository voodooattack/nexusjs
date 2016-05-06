(function() {
  const deviceKey = Symbol(), filtersKey = Symbol();
  return class ReadableStream {
    constructor(device) {
      this[deviceKey] = device;
      this[filtersKey] = [];
      if (device.type !== 'pull' && device.type !== 'push')
        throw new TypeError('invalid device type');
    }
    get filters() { return this[filtersKey]; }
    get device() { return this[deviceKey]; }
    get eof() { return this.device.eof; }
    read() {
      if (this.device.type === 'push')
        throw new TypeError('can not perform read operation on PushSourceDevice');
      return this.device.read.apply(this.device, arguments).then(data =>
        this.filters.reduce((prev, next) => prev.then(next.process.bind(next)), Promise.resolve(data))
      );
    }
    readSync() {
      if (this.device.type === 'push')
        throw new TypeError('can not perform sync read operation on PushSourceDevice');
      const data = this.device.readSync.apply(this.device, arguments);
      this.filters.forEach(f => data = f.processSync(data));
      return data;
    }
    pipe(...targets) {
      if (this.device.type === 'pull') {
        return this.read().then(data =>
          Promise.all(targets.map(target => target.write(data))));
      } else if (this.device.type === 'push') {
        const chain = data =>
          this.filters.reduce((prev, next) => prev.then(next.process.bind(next)), Promise.resolve(data))
                      .then(data => Promise.all(targets.map(target => target.write(data))));
        this.device.on('data', chain);
        return this.device.resume().then(v => { this.device.off('data', chain); return v; });
      }
    }
    pushFilter(...filters) {
      filters.forEach(f => this.filters.push(f));
    }
    popFilter() {
      return this.filters.pop();
    }
  }
})();
