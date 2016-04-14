let x = 0;

function task(n) {
  Console.log(`task ${n} on thread '${scheduler.threadId}' with ++x == ${++x}`);
  Scheduler.schedule(task.bind(this, n));
}

for(let i = 0; i < 8; i++) {
  Scheduler.schedule(task.bind(null, i));
}
