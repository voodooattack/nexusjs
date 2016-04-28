class EmitterTest extends Nexus.EventEmitter {
  constructor() {
    super();
    for(let i = 0; i < 4; i++)
      this.on('test', value => { console.log(`fired test ${i}!`); console.inspect(value); });
  }
}

const test = new EmitterTest();

test.emit('test', { payload: 'test 1' })
  .then(_ => test.emit('test', { payload: 'test 2' }))
  .then(_ => console.log('all fired!'));
