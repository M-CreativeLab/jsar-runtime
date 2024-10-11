class Foo {
  foo: string = 'foo';
  constructor() {
    console.log(this.foo, 'from TypeScript');
  }
}

console.info('foo-script.ts loaded');
new Foo();
