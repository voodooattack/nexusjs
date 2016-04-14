(function() {
  const resolveValueKey = Symbol(), rejectValueKey = Symbol(),
        subscribersKey = Symbol(), broadcastResolveKey = Symbol(),
        broadcastRejectKey = Symbol(), taskKey = Symbol(), stateKey = Symbol();
  const PENDING = 0, RESOLVED = 1, REJECTED = 2;
  return class Promise {
    constructor(executor) {
      if (typeof executor !== 'function') throw new TypeError('not a function');
      this[subscribersKey] = [];
      this[stateKey] = PENDING;
      const broadcastResolve = this[broadcastResolveKey] = function(value) {
        if (value === this)
          throw new TypeError('a promise cannot be resolved with itself');
        if (this[stateKey] !== PENDING)
          return;
        if (value instanceof Promise)
        {
          if (value[stateKey] !== PENDING) {
            if (value[resolveValueKey])
            {
              Scheduler.schedule(this[broadcastResolveKey].bind(this, value[resolveValueKey]));
            } else if (value[rejectValueKey]) {
              Scheduler.schedule(this[broadcastRejectKey].bind(this, value[rejectValueKey]));
            }
          } else {
            value[subscribersKey].push({
              resolve: this[broadcastResolveKey].bind(this),
              reject: this[broadcastRejectKey].bind(this),
              promise: this
            });
          }
          return;
        }
        this[resolveValueKey] = value;
        this[stateKey] = RESOLVED;
        this[subscribersKey].forEach(({ resolve, reject, promise }) => {
          if (resolve) {
            promise[taskKey] = Scheduler.schedule(function() {
              promise[broadcastResolveKey](resolve(value));
            });
          }
        });
      };
      const broadcastReject = this[broadcastRejectKey] = function(value) {
        if (this[stateKey] !== PENDING)
          return;
        this[rejectValueKey] = value;
        this[stateKey] = REJECTED;
        this[subscribersKey].forEach(({ resolve, reject, promise }) => {
          if (reject) {
            promise[taskKey] = Scheduler.schedule(function() {
              promise[broadcastResolveKey](reject(value));
            });
          }
        });
      };
      this[taskKey] = Scheduler.schedule(
        executor.bind(null, broadcastResolve.bind(this), broadcastReject.bind(this))
      );
    }
    then(resolve, reject) {
      if (this[resolveValueKey] || this[rejectValueKey])
      {
        if (this[resolveValueKey] && resolve) {
          return Promise.resolve(this[resolveValueKey]).then(resolve);
        } else if (this[rejectValueKey] && reject) {
          return Promise.reject(this[rejectValueKey]).then(reject);
        } else {
          return Promise.reject(new TypeError('invalid arguments'));
        }
      } else {
        const promise = new Promise(function() {});
        this[subscribersKey].push({ resolve, reject, promise });
        return promise;
      }
    }
    catch(handler) {
      return this.then(undefined, handler);
    }
    static resolve(value) {
      return new Promise((resolve, reject) => resolve(value));
    }
    static reject(value) {
      return new Promise((resolve, reject) => reject(value));
    }
    static all(collection) {
      function allOrReject(resolve, reject) {
        const filtered = collection.filter(v => v[stateKey] !== PENDING);
        const rejection = filtered.find(v => v[stateKey] === REJECTED);
        if (rejection) {
          return reject(rejection[rejectValueKey]);
        } else if (filtered.length === collection.length) {
          return resolve(collection.map(v => v instanceof Promise ? v[resolveValueKey] : v));
        } else {
          Scheduler.schedule(allOrReject.bind(this, resolve, reject));
        }
      }
      return new Promise(allOrReject);
    }
    static race(collection) {
      function firstOrReject(resolve, reject) {
        const filtered = collection.filter(v => v[stateKey] !== PENDING);
        const rejection = filtered.find(v => v[stateKey] === REJECTED);
        if (rejection) {
          return reject(rejection[rejectValueKey]);
        } else if (filtered.length) {
          return resolve(filtered[0]);
        } else {
          Scheduler.schedule(firstOrReject.bind(this, resolve, reject));
        }
      }
      return new Promise(firstOrReject);
    }
  }
})();
