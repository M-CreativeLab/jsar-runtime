'use strict';

const path = require('node:path');
const fs = require('node:fs');
const { execSync } = require('node:child_process');

const argv = require('./minimist.cjs')(process.argv.slice(2));
const { clean } = argv;
const withoutPack = argv['without-pack'];

const shell = (cmd, options) => execSync(cmd, { stdio: 'inherit', ...options });
const jsDir = path.join(__dirname, '../');

function installDeps() {
  shell('npm ci', { cwd: jsDir });
}

if (clean === 'yes' || !fs.existsSync(path.join(jsDir, 'node_modules'))) {
  installDeps();
}
if (!withoutPack || withoutPack !== 'yes') {
  shell(`npx webpack --config webpack.config.cjs`, { cwd: jsDir });
}
