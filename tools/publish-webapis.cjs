const fs = require('node:fs');
const path = require('node:path');
const { execSync } = require('node:child_process');
const minimist = require('minimist');

try {
  execSync('git --version', { stdio: 'ignore' });
  execSync('npm --version', { stdio: 'ignore' });
} catch (error) {
  console.error('Error: git and npm must be installed and available in the PATH.');
  process.exit(1);
}

const args = minimist(process.argv.slice(2));

if (args.help) {
  console.log(`
Usage: node publish-webapis.cjs [options]

Options:
  --release   Use the base version from package.json
  --publish   Publish the package to the npm registry
  --help      Show this help message
`);
  process.exit(0);
}

const isRelease = args.release;
const shouldPublish = args.publish;

const packageJson = require('../package.json');
const version = packageJson.version;

const commitHash = execSync('git rev-parse --short HEAD').toString().trim();
const newVersion = isRelease ? version : `${version}-${commitHash}`;

const webApisPackageJson = {
  name: '@yodaos-jsar/web-apis',
  version: newVersion,
  main: 'index.d.ts',
  types: 'index.d.ts',
  files: ['index.d.ts', 'transmute-private.d.ts'],
  publishConfig: {
    access: 'public',
    registry: 'https://registry.npmjs.org/'
  }
};

const webApisDir = path.join(__dirname, '../build/output/web-apis');
if (fs.existsSync(webApisDir)) {
  fs.rmSync(webApisDir, { recursive: true, force: true });
}
fs.mkdirSync(webApisDir, { recursive: true });

const webApisPackageJsonPath = path.join(webApisDir, 'package.json');
fs.writeFileSync(webApisPackageJsonPath, JSON.stringify(webApisPackageJson, null, 2));

const transmuteWebApisPath = path.join(__dirname, '../types/transmute-webapis.d.ts');
const transmutePrivatePath = path.join(__dirname, '../types/transmute-private.d.ts');
const webApisIndexPath = path.join(webApisDir, 'index.d.ts');
const webApisPrivatePath = path.join(webApisDir, 'transmute-private.d.ts');
fs.copyFileSync(transmuteWebApisPath, webApisIndexPath);
fs.copyFileSync(transmutePrivatePath, webApisPrivatePath);

const readmeContent = `
# @yodaos-jsar/web-apis

This package contains the TypeScript definitions for the Web APIs used in the YodaOS JSAR runtime.

## Installation

\`\`\`sh
npm install @yodaos-jsar/web-apis
\`\`\`

## Usage

To use the types provided by this package, configure the types package in your project's tsconfig.json:

\`\`\`json
{
  "compilerOptions": {
    "types": ["@yodaos-jsar/web-apis"]
  }
}
\`\`\`
`;
const readmePath = path.join(webApisDir, 'README.md');
fs.writeFileSync(readmePath, readmeContent);

if (shouldPublish) {
  execSync('npm publish', { cwd: webApisDir, stdio: 'inherit' });
} else {
  execSync('npm pack', { cwd: webApisDir, stdio: 'inherit' });
}
