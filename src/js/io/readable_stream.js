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
      }
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
  class ReadableStream extends EventEmitter2 {
    constructor(device) {
      super();
      this[deviceKey] = device;
      this[filtersKey] = [];
      if (device.type !== 'pull' && device.type !== 'push')
        throw new TypeError('invalid device type');
      if (device.type === 'push') {
        device.on('data', buffer =>
          this.filters.reduce((prev, next) => prev.then(next.process.bind(next)), Promise.resolve(buffer))
            .then(buffer => this.emit('data', buffer)));
        device.on('end', () => this.emit('end'));
        device.on('error', e => this.emit('error', e));
      }
    }
    get filters() { return this[filtersKey]; }
    get device() { return this[deviceKey]; }
    get eof() { return this.device.eof; }
    resume() {
      if (this.device.type === 'pull')
        throw new TypeError('can not perform resume operation on PullSourceDevice');
      return this.device.resume();
    }
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
      let data = this.device.readSync.apply(this.device, arguments);
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
        this.device.on('end', async () => {
          await Promise.all(targets.map(t => t.write(null)));
          await this.emit('end');
        });
        this.device.on('error', () => this.emit('error'));
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
  return ReadableStream;
})();
