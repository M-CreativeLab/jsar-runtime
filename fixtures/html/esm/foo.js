import helloDefault from './exports-as-default.js';
import { hello } from './exports-as-named.js';

console.log('foobar in esm/foo.ts');
helloDefault('foo');
hello('foo');

// import { some } from './bar/some';
// import { sum, sumWithSome } from './bar/ops';
// console.log(some);
// console.log(sum(some, 10) === sumWithSome(10));
// console.log(sum);
