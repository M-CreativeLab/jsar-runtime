import { some } from './some.js';

export function sum(a, b) {
  return a + b;
}

export function sumWithSome(a) {
  return sum(a, some);
}
