'use strict';

const path = require('node:path');
const fs = require('node:fs');
const { execSync } = require('node:child_process');

const argv = require('./minimist.cjs')(process.argv.slice(2));
const { clean, minify } = argv;
const withoutPack = argv['without-pack'];

const shell = (cmd, options) => execSync(cmd, { stdio: 'inherit', ...options });
const jsDir = path.join(__dirname, '../');

function installDeps() {
  shell('npm install', { cwd: jsDir });

  const babylonjsTypeDefPath = path.join(jsDir, 'node_modules/babylonjs/babylon.d.ts');
  const babylonjsPostTxtPath = path.join(__dirname, './patches/babylonjs-post.txt');
  const babylonjsTypeDefContent = fs.readFileSync(babylonjsTypeDefPath, { encoding: 'utf-8' });
  const babylonjsPostTxtContent = fs.readFileSync(babylonjsPostTxtPath, { encoding: 'utf-8' });
  fs.writeFileSync(babylonjsTypeDefPath, babylonjsTypeDefContent + babylonjsPostTxtContent);
}

if (clean === 'yes' || !fs.existsSync(path.join(jsDir, 'node_modules'))) {
  installDeps();
}
if (!withoutPack || withoutPack !== 'yes') {
  shell(`npx webpack --config webpack.config.cjs`, { cwd: jsDir });
}
