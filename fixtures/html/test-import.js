function testImport() {
  console.log('testImport function loaded successfully');
}

// Make function globally available
if (typeof window !== 'undefined') {
  window.testImport = testImport;
} else if (typeof global !== 'undefined') {
  global.testImport = testImport;
}