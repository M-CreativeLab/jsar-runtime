const { CameraManager } = require('./targets/darwin/webcam-server.node');

const mgr = new CameraManager((frame) => {
    console.log('CameraManager is ready', frame.height, frame.width, frame.data);
});
console.log(mgr);
mgr.start();

setTimeout(() => {
    mgr.stop();
}, 3000);
