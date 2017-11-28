let x = 10;

export function inc() {
  x++;
}

export default { get x() { return x; } };