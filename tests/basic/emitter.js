class EmitterTest extends Nexus.EventEmitter {
  constructor() {
    super();
    for(let i = 0; i < 4; i++)
      this.on('test', _ => console.log(`fired test ${i}!`));
  }
}

const test = new EmitterTest();

test.emit('test')
  .then(_ => test.emit('test'))
  .then(_ => console.log('all fired!'));
