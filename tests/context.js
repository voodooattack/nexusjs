const ctx = new Context({ val1: 1 });

ctx.eval("console.log('sandboxed value: ' + val1); module.exports = { testValue: { test: 10 } }; ");

const exports = ctx.exports;

console.log("exports.testValue.test == " + exports.testValue.test);
