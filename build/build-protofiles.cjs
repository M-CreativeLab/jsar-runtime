const { execSync } = require('node:child_process');
const os = require('node:os');
const path = require('node:path');

const isWindows = os.platform() === 'win32';
let protocCommand = path.join(
  __dirname,
  '../thirdparty/libs',
  isWindows ? 'Windows/x86_64/bin/protoc.exe' : 'Darwin/bin/protoc');
if (process.env.USE_SYSTEM_PROTOC === 'yes') {
  protocCommand = 'protoc';
}

console.info(`     platform: "${os.platform()}"`);
console.info(`      dirname: "${__dirname}"`);
console.info(`protocCommand: "${protocCommand}"`);

const protoDir = path.join(__dirname, '../proto');
const protoFile = 'transmute_vgom.proto';

// Generate the proto
execSync(`${protocCommand} --cpp_out=. --csharp_out=. ${protoFile}`, {
  stdio: 'inherit',
  cwd: protoDir,
});
