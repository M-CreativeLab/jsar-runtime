import path from 'path';
import fs from 'fs';
import OSS from 'ali-oss';

const args = process.argv.slice(2);
const [ version, filename ] = args;

if (!version || !version.startsWith('amidala-v')) {
  throw new Error('a valid version must be "amidala-v*", such as "amidala-v1.0.0"')
}
if (!filename) {
  throw new Error('a valid filename must be provided')
}

const ossFilename = path.posix.join(version, filename);
const artifactFilename = new URL(path.join('../', filename), import.meta.url);
console.log(`ossFilename: ${ossFilename}`);
console.log(`artifactFilename: ${artifactFilename}`);

const store = new OSS({
  region: 'oss-cn-hangzhou',
  accessKeyId: process.env.ALIYUN_ACCESS_KEY_ID,
  accessKeySecret: process.env.ALIYUN_ACCESS_KEY_SECRET,
  bucket: process.env.ALIYUN_BUCKET,
});

console.info(`downloading ${ossFilename}...`);
const resp = await store.get(
  `web-assets/yodaos-jsar/runtime-artifacts/${ossFilename}`
);
console.info(resp);
if (resp.content) {
  console.info(`saving ${resp.content?.byteLength}bytes => (${artifactFilename})`);
  fs.writeFileSync(artifactFilename, resp.content);
}
