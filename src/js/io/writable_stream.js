(function() {
  const eventsKey = Symbol();
  class EventEmitter2 {
    constructor() {
      this[eventsKey] = {};
    }
    on(event, functor) {
      this[eventsKey][event] = this[eventsKey][event] || [];
      this[eventsKey][event].push({ functor, count: Infinity });
    }
    once(event, functor) {
      this[eventsKey][event] = this[eventsKey][event] || [];
      this[eventsKey][event].push({ functor, count: 1 });
    }
    many(event, functor, count) {
      this[eventsKey][event] = this[eventsKey][event] || [];
      this[eventsKey][event].push({ functor, count });
    }
    emit(event, ...args) {
      if (this[eventsKey][event]) {
        return Promise.all(
          this[eventsKey][event].filter(e => e.count-- > 0)
            .map(e => e.functor)
            .map(functor => Promise.resolve(functor(...args)))
        );
      } else
        return Promise.resolve([]);
    }
    off(event, target) {
      if (this[eventsKey][event])
        this[eventsKey][event] = this[eventsKey][event].filter(e => e.functor !== target);
    }
    allOff(event) {
      delete this[eventsKey][event];
    }
  }
  const deviceKey = Symbol(), filtersKey = Symbol();
  class WritableStream extends EventEmitter2 {
    constructor(device) {
      super();
      this[deviceKey] = device;
      this[filtersKey] = [];
      device.on('error', e => this.emit('error', e));
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
    close() {
      return this.device.close();
    }
  }
  return WritableStream;
})();
