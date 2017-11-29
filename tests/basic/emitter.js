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

async function start() {
  await test.emit('test', { payload: 'test 1' });
  console.log('first test done!');
  await test.emit('test', { payload: 'test 2' });
  console.log('second test done!');
  const values = await test.emit('returns-a-value', 10);
  console.log('third test done, returned values are:'); console.inspect(values);
}

start().catch(console.error);



