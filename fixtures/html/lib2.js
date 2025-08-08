function lib2Function() {
  console.log('lib2Function called successfully');
}

if (typeof window !== 'undefined') {
  window.lib2Function = lib2Function;
} else if (typeof global !== 'undefined') {
  global.lib2Function = lib2Function;
}