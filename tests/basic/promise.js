{
  const test = new Promise((resolve, reject) => reject(new Error('test')));

  test.then(v => console.log(`1 ${v}`));
  test.then(v => { console.log(`2 ${v}`); return "test1!!"; }).then(test => console.log(test));
  test.catch(e => { console.log(`e ${e}`); return 'bozo'; }).then(bozo => console.log(bozo));
}
{
  const p2 = new Promise(function(resolve, reject) {
    resolve(1);
  });

  p2.then(function(value) {
    console.log(value); // 1
    return value + 1;
  }).then(function(value) {
    return new Promise((resolve, reject) => reject(value));
  }).catch(e => console.error("ERROR " + e));

  p2.then(function(value) {
    console.log(value); // 1
  });
}
{
  const p1 = Promise.resolve(3);
  const p2 = 1337;
  const p3 = new Promise(function(resolve, reject) {
    resolve("foo");
  });

  Promise.all([p1, p2, p3]).then(function(values) {
    console.log(values); // [3, 1337, "foo"]
  });
}
{
  var p1 = new Promise(function(resolve, reject) {
    const i = setTimeout(resolve, 100, "one");
  });
  var p2 = new Promise(function(resolve, reject) {
    const i = setTimeout(resolve, 200, "two");
  });
  var p3 = new Promise(function(resolve, reject) {
    const i = setTimeout(resolve, 300, "three");
  });
  var p4 = new Promise(function(resolve, reject) {
    const i = setTimeout(resolve, 400, "four");
  });
  var p5 = new Promise(function(resolve, reject) {
    reject("rejected!");
  });

  Promise.race([p1, p2, p3, p4, p5]).then(function(value) {
    console.log(value);
  }, function(reason) {
    console.log(reason);
  });
}
{
  const p1 = Promise.reject("test unhandled rejection").then(v => console.log(v));
}
