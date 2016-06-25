// https://www.promisejs.org/generators/
function makeAsync(makeGenerator){
  return function () {
    const generator = makeGenerator(this, arguments);
    function handle(result){
      // result => { done: [Boolean], value: [Object] }
      if (result.done) return Promise.resolve(result.value);
      return Promise.resolve(result.value).then(function (res){
        return handle(generator.next(res));
      }, function (err){
        return handle(generator.throw(err));
      });
    }
    try {
      return handle(generator.next());
    } catch (ex) {
      return Promise.reject(ex);
    }
  }
}

function promised() {
  return new Promise((resolve, reject) => {
    setTimeout(resolve.bind(null, "Promised value!"), 500);
  });
}

const test = makeAsync(function * test () {
  const value = yield promised();
  console.log(value);
  return value;
});

test();
