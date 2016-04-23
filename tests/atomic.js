const arr = [];
const arr2 = []
for(let i = 0; i < 1000; i++)
  arr.push(i);

Promise.all(arr.map(i =>
  Promise.resolve().then(() => arr2.push(i)))
).then(() => console.log(arr2.length));

