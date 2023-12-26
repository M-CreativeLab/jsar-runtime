const { execSync } = require('node:child_process');
const os = require('node:os');
const path = require('node:path');

const isWindows = os.platform() === 'win32';
const protocCommand = isWindows
  ? '..\\thirdparty\\libs\\Windows\\x86_64\\bin\\protoc'
  : '../thirdparty/libs/Darwin/bin/protoc';

const protoDir = path.join(__dirname, '../proto');
const protoFile = './transmute_vgom.proto';

// Generate the proto
execSync(`${protocCommand} --cpp_out=./ --csharp_out=./ ${protoFile}`, {
  stdio: 'inherit',
  cwd: protoDir,
});
