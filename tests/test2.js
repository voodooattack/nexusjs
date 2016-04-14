let a = 0, b = 0;

function withdraw(v) {
  Console.log(`starting withdraw on thread '${Scheduler.threadId}' with a == ${a} and b == ${b}`);
  a -= v;
  b += v;
  Console.log(`ending withdraw on thread '${Scheduler.threadId}' with a == ${a} and b == ${b}`);
}

function deposit(v) {
  Console.log(`starting deposit on thread '${Scheduler.threadId}' with a == ${a} and b == ${b}`);
  b -= v;
  a += v;
  Console.log(`ending deposit on thread '${Scheduler.threadId}' with a == ${a} and b == ${b}`);
}

Scheduler.schedule(deposit.bind(null, 10));
Scheduler.schedule(withdraw.bind(null, 5));
Scheduler.schedule(withdraw.bind(null, 5));
Scheduler.schedule(deposit.bind(null, 10));
Scheduler.schedule(withdraw.bind(null, 5));
Scheduler.schedule(withdraw.bind(null, 5));
