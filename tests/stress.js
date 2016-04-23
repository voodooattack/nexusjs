let x = 0;

function task(n) {
  console.log(`task ${n} on thread '${Nexus.Scheduler.threadId}' with ++x == ${++x} and ${Nexus.Scheduler.concurrency} active threads`);
  Nexus.Scheduler.schedule(task.bind(null, n));
}

for(let i = 0; i < 10000; i++) {
  Nexus.Scheduler.schedule(task.bind(null, i));
}
