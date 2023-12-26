'use strict';

const path = require('path');
const fs = require('fs');
const { execSync } = require('child_process');

const argv = require('./minimist')(process.argv.slice(2));
const clean = argv.clean;
const shell = (cmd, options) => execSync(cmd, { stdio: 'inherit', ...options });
const jsframeworkDir = path.join(__dirname, '../Source/jsframework');

if (clean === 'yes') {
  console.log('Clean installing the npm dependencies');
  shell('npm ci', { cwd: jsframeworkDir });

  const babylonjsTypeDefPath = path.join(jsframeworkDir, 'node_modules/babylonjs/babylon.d.ts');
  const babylonjsPostTxtPath = path.join(__dirname, './patches/babylonjs-post.txt');
  const babylonjsTypeDefContent = fs.readFileSync(babylonjsTypeDefPath, { encoding: 'utf-8' });
  const babylonjsPostTxtContent = fs.readFileSync(babylonjsPostTxtPath, { encoding: 'utf-8' });
  fs.writeFileSync(babylonjsTypeDefPath, babylonjsTypeDefContent + babylonjsPostTxtContent);
}

shell(`npx webpack --config webpack.config.js`, {
  cwd: path.join(jsframeworkDir, 'runtime'),
});
shell('node ./bundle-cpp-gen.js --out ../Source/transmute/jsframework.h');
