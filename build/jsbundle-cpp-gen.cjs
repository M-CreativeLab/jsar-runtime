const fs = require('node:fs');
const path = require('node:path');

const argv = require('./minimist.cjs')(process.argv.slice(2));
const templateConfigureFilename = path.join(__dirname, '../src/runtime/jsbundle.h.in');
console.log(templateConfigureFilename);
const templateConfigureFile = fs.readFileSync(templateConfigureFilename, 'utf8');

// Read JSFRAMEWORK_SOURCE from a file or define it here
const sourceFilename = path.join(__dirname, './output/jsar-runtime.js');
const JSFRAMEWORK_SOURCE = fs.readFileSync(sourceFilename, 'utf8');
const TOTAL_LENGTH = JSFRAMEWORK_SOURCE.length;
let MAX_SPLIT_SIZE = 12000;

// Set MAX_SPLIT_SIZE to a bigger value when not on Windows
if (process.platform !== 'win32') {
  MAX_SPLIT_SIZE = 100000;
}

const NUM_SEGMENTS = Math.floor(TOTAL_LENGTH / MAX_SPLIT_SIZE) + 1;

let JSBUNDLE_PARTS = '';
for (let SEGMENT_INDEX = 0; SEGMENT_INDEX < NUM_SEGMENTS; SEGMENT_INDEX++) {
  const START_INDEX = SEGMENT_INDEX * MAX_SPLIT_SIZE;
  const REMAINING_LENGTH = TOTAL_LENGTH - START_INDEX;
  let SPLIT_SIZE;

  if (REMAINING_LENGTH <= 0) {
    break;
  } else if (REMAINING_LENGTH > MAX_SPLIT_SIZE) {
    SPLIT_SIZE = MAX_SPLIT_SIZE;
  } else {
    SPLIT_SIZE = REMAINING_LENGTH;
  }

  const CURRENT_SEGMENT = JSFRAMEWORK_SOURCE.slice(START_INDEX, START_INDEX + SPLIT_SIZE);
  JSBUNDLE_PARTS += `\nu8R\"RAW(${CURRENT_SEGMENT.replace(/\$/g, '$$$$')})RAW\"`;
}

const cppSourceText = templateConfigureFile.replace('@JSBUNDLE_PARTS@', JSBUNDLE_PARTS);
if (argv.out) {
  fs.writeFileSync(argv.out, cppSourceText, 'utf8');
} else {
  process.stdout.write(cppSourceText);
}
