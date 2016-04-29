class EmitterTest extends Nexus.EventEmitter {
  constructor() {
    super();
    for(let i = 0; i < 4; i++)
      this.on('test', value => { console.log(`fired test ${i}!`); console.inspect(value); });
    for(let i = 0; i < 4; i++)
      this.on('returns-a-value', v => `${v + i}`);
  }
}

const test = new EmitterTest();

test.emit('test', { payload: 'test 1' })
  .then(_ => test.emit('test', { payload: 'test 2' }))
  .then(_ => console.log('first test done!'))
  .then(_ => test.emit('returns-a-value', 10))
  .then(values => { console.log('second test done, returned values are:'); console.inspect(values); });

