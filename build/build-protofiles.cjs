const { execSync } = require('child_process');
const os = require('os');
const fs = require('fs');
const path = require('path');

const isWindows = os.platform() === 'win32';
const protocCommand = isWindows
  ? '..\\..\\Thirdparty\\Library\\Windows\\x86_64\\bin\\protoc'
  : '../../Thirdparty/Library/Darwin/bin/protoc';

const protoDir = path.join(__dirname, '../Source/proto');
const protoFile = './transmute_vgom.proto';
const cppOut = './';
const csharpOut = isWindows
  ? '..\\..\\TransmuteUnityFramework\\Assets\\UPM\\Scripts\\Proto\\'
  : '../../TransmuteUnityFramework/Assets/UPM/Scripts/Proto/';

// Generate the proto
execSync(`${protocCommand} --cpp_out=${cppOut} --csharp_out=${csharpOut} ${protoFile}`, {
  stdio: 'inherit',
  cwd: protoDir,
});
console.info(`Outputs cpp => ${cppOut}`);
console.info(`Outputs c# => ${csharpOut}`);

// Copy the source to devtools
fs.copyFileSync(
  path.join(protoDir, protoFile),
  path.join(__dirname, '../Source/jsframework/devtools/vscode-extension/src/vgom-browser-impl/transmute_vgom.proto')
);
