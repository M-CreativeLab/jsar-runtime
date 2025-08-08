function lib1Function() {
  console.log('lib1Function called successfully');
}

if (typeof window !== 'undefined') {
  window.lib1Function = lib1Function;
} else if (typeof global !== 'undefined') {
  global.lib1Function = lib1Function;
}