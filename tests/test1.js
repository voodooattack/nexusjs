let x = 0;

function task(n) {
  console.log(`task ${n} on thread '${Scheduler.threadId}' with ++x == ${++x} and ${Scheduler.concurrency} active threads`);
  Scheduler.schedule(task.bind(null, n));
}

for(let i = 0; i < 1024; i++) {
  Scheduler.schedule(task.bind(null, i));
}
