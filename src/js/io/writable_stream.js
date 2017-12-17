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
    emit(e, ...args) {
      const event = this[eventsKey][e];
      if (event) {
        const events = [], length = event.length;
        for(let i = 0; i < length; i++)
          if (event[i].count-- > 0)
            events.push(
              Promise.resolve(event[i].functor.apply(void 0, args))
            );
        return Promise.all(events);
        return Promise.all(events);
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
