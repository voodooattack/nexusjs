let a = 0, b = 0;

function withdraw(v) {
  console.log(`starting withdraw on thread '${Nexus.Scheduler.threadId}' with a == ${a} and b == ${b}`);
  a -= v;
  b += v;
  console.log(`ending withdraw on thread '${Nexus.Scheduler.threadId}' with a == ${a} and b == ${b}`);
}

function deposit(v) {
  console.log(`starting deposit on thread '${Nexus.Scheduler.threadId}' with a == ${a} and b == ${b}`);
  b -= v;
  a += v;
  console.log(`ending deposit on thread '${Nexus.Scheduler.threadId}' with a == ${a} and b == ${b}`);
}

Nexus.Scheduler.schedule(deposit.bind(null, 10));
Nexus.Scheduler.schedule(withdraw.bind(null, 5));
Nexus.Scheduler.schedule(withdraw.bind(null, 5));
Nexus.Scheduler.schedule(deposit.bind(null, 10));
Nexus.Scheduler.schedule(withdraw.bind(null, 5));
Nexus.Scheduler.schedule(withdraw.bind(null, 5));
