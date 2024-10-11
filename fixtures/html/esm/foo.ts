export class Foo {
  foo: string = 'foo';
  constructor() {
    console.log(this.foo, 'from TypeScript');
  }
}

console.info('foo.ts loaded');
