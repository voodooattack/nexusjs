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
        for(let i = 0; i < length; i++) {
          if (event[i].count-- > 0) {
            events.push(
              Promise.resolve(event[i].functor.apply(void 0, args))
            );
          }
        }
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
  class ReadableStream extends EventEmitter2 {
    constructor(device) {
      super();
      this[deviceKey] = device;
      this[filtersKey] = [];
      if (device.type !== 'pull' && device.type !== 'push') {
        throw new TypeError('invalid device type');
      }
      if (device.type === 'push') {
        device.on('data', buffer => {
          return this.filters.reduce((prev, next) => prev.then(next.process.bind(next)), Promise.resolve(buffer))
            .then(buffer => this.emit('data', buffer), e => this.emit('error', e));
        });
        device.on('end', async () => {
          try {
            const result = await this.filters.reduce((prev, next) => prev.then(next.process.bind(next)), Promise.resolve(null));
            if (result !== null) await this.emit('data', result);
          }
          catch (e) {
            await this.emit('error', e);
          } finally  {
            await this.emit('end');
          }
        });
        device.on('error', e => this.emit('error', e));
      }
    }
    get filters() { return this[filtersKey]; }
    get device() { return this[deviceKey]; }
    get eof() { return this.device.eof; }
    async resume() {
      if (this.device.type === 'pull')
      {
        try {
          while (!this.device.eof) {
            let buffer = await this.device.read(8 * 1024 * 1024);
            await this.emit('data', buffer);
          }
        }
        catch (e)
        {
          await this.emit('error', e);
        } finally {
          await this.emit('end');
        }
        return this;
      } else
        return new Promise((resolve, reject) => {
          this.once('end', () => resolve(this));
          this.once('error', reject);
          this.device.resume().catch(e => this.emit('error', e));
        });
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
        let disconnectAll;
        const onData = async buffer => {
          try {
            await Promise.all(targets.map(target => target.write(buffer)));
          } catch (e) {
            await this.emit('error', e);
          }
        };
        const onEnd = async () => {
          await Promise.all(targets.map(target => target.write(null)));
        };
        disconnectAll = async () => {
          this.off('data', onData);
          this.off('end', onEnd);
        };
        this.on('data', onData);
        this.once('end', onEnd);
        return disconnectAll;
      }
    }
    pushFilter(...filters) {
      this.filters.push(...filters);
    }
    popFilter() {
      return this.filters.pop();
    }
    close() {
      return this.device.close();
    }
  }
  return ReadableStream;
})();
