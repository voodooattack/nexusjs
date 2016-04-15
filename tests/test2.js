let a = 0, b = 0;

function withdraw(v) {
  console.log(`starting withdraw on thread '${Scheduler.threadId}' with a == ${a} and b == ${b}`);
  a -= v;
  b += v;
  console.log(`ending withdraw on thread '${Scheduler.threadId}' with a == ${a} and b == ${b}`);
}

function deposit(v) {
  console.log(`starting deposit on thread '${Scheduler.threadId}' with a == ${a} and b == ${b}`);
  b -= v;
  a += v;
  console.log(`ending deposit on thread '${Scheduler.threadId}' with a == ${a} and b == ${b}`);
}

Scheduler.schedule(deposit.bind(null, 10));
Scheduler.schedule(withdraw.bind(null, 5));
Scheduler.schedule(withdraw.bind(null, 5));
Scheduler.schedule(deposit.bind(null, 10));
Scheduler.schedule(withdraw.bind(null, 5));
Scheduler.schedule(withdraw.bind(null, 5));
