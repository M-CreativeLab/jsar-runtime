import * as THREE from 'three';

var camera, scene, renderer;
var walter;
var hemiLight, dirLight, backLight, isUp, frrrr, isMove;

var container = {
  width: 0,
  height: 0
}
var mouse = {
  x: {
    current: 0,
    previous: 0,
    calc: 0
  },
  y: {
    current: 0,
    previous: 0,
    calc: 0
  }
}

function init(cb) {

  container.width = window.innerWidth;
  container.height = window.innerHeight;

  camera = new THREE.PerspectiveCamera(65, container.width / container.height, 1, 2000);
  camera.position.z = 2000;
  camera.position.y = 400;
  camera.lookAt(new THREE.Vector3(0, 0, 0));

  scene = new THREE.Scene();

  const gl = navigator.gl;
  navigator.xr.requestSession('immersive-ar', {}).then((session) => {
    console.info('XR session started:', session);
    const baseLayer = new XRWebGLLayer(session, gl);
    session.updateRenderState({ baseLayer });

    renderer = new THREE.WebGLRenderer({
      canvas: {
        addEventListener() { },
      },
      context: gl,
    });
    renderer.shadowMap.enabled = true;
    renderer.xr.enabled = true;
    renderer.xr.setReferenceSpaceType('local');
    renderer.xr.setSession(session);
    cb();
  }, (err) => {
    console.warn('Failed to start XR session:', err);
    cb(err);
  });

  // window.addEventListener('load', function () {
  //   document.addEventListener('mousemove', mousemove, false);
  //   window.addEventListener('resize', onWindowResize, false);
  //   document.addEventListener('mouseup', mouseup, false);
  //   document.addEventListener('mousedown', mousedown, false);
  //   document.addEventListener('touchend', touchend, false);
  //   document.addEventListener('touchmove', touchmove, false);
  // });
}

function onWindowResize() {
  container.width = window.innerWidth;
  container.height = window.innerHeight;
  camera.aspect = container.width / container.height;
  camera.updateProjectionMatrix();
  renderer.setSize(window.innerWidth, window.innerHeight);
}

function mousemove(e) {
  mouse.x.current = e.clientX;
  mouse.y.current = e.clientY;
  mouse.x.calc = mouse.x.current - (container.width / 2);
  mouse.y.calc = mouse.y.current - (container.height / 2);
}


function touchend(e) {
  if (isUp) {
    isUp = false;
  } else {
    mousedown(e);
  }
}

function touchmove(e) {
  if (e.touches.length === 1) {
    e.preventDefault();
    mouse.x.current = e.touches[0].pageX,
      mouse.y.current = e.touches[0].pageY;
    mouse.x.calc = mouse.x.current - (container.width / 2);
    mouse.y.calc = mouse.y.current - (container.height / 2);
  }
}

function mouseup(e) {
  isUp = false;
  console.log(isUp)

}

function mousedown(e) {
  isUp = true;
  console.log(isUp)

}

function addLights() {
  hemiLight = new THREE.HemisphereLight(0xffffff, 0xffffff, 0.6);

  dirLight = new THREE.DirectionalLight(0xffffff, .8);
  dirLight.position.set(200, 200, 200);
  dirLight.castShadow = true;
  // dirLight.shadowDarkness = .1;

  backLight = new THREE.DirectionalLight(0xffffff, .4);
  backLight.position.set(-200, 200, 50);
  // backLight.shadowDarkness = .1;
  // backLight.castShadow = true;

  // scene.add(backLight);
  scene.add(hemiLight);
  scene.add(dirLight);
}


function createWalter() {
  walter = new Walter();
  scene.add(walter.threegroup);
}


function Walter() {

  this.threegroup = new THREE.Group();

  this.informalSmokingMat = "#ffc107";
  this.informalLegsMMat = "#755b0b";
  this.informalZipperMat = "#755b0b";
  this.informalShoesMat = "#907637";

  this.formalSmokingMat = "#333";
  this.formalLegsMMat = "#222";
  this.formalZipperMat = "white";
  this.formalShoesMat = "#585858";


  this.hatMat = new THREE.MeshLambertMaterial({
    color: "#333",
    shading: THREE.FlatShading,
  });

  this.skinMat = new THREE.MeshLambertMaterial({
    color: "#e0bea5",
    shading: THREE.FlatShading,
  });

  this.pupilaMat = new THREE.MeshLambertMaterial({
    color: "#333",
    shading: THREE.FlatShading,
  });

  this.lipMat = new THREE.MeshLambertMaterial({
    color: "#333",
    shading: THREE.FlatShading,
  });

  this.eyeMat = new THREE.MeshLambertMaterial({
    color: "white",
    shading: THREE.FlatShading
  });

  this.bearMat = new THREE.MeshLambertMaterial({
    color: "#bb7344",
    shading: THREE.FlatShading
  });

  this.zipperMat = new THREE.MeshLambertMaterial({
    color: this.formalZipperMat,
    shading: THREE.FlatShading
  });

  this.smokingMat = new THREE.MeshLambertMaterial({
    color: this.formalSmokingMat,
    shading: THREE.FlatShading
  });

  this.legsMMat = new THREE.MeshLambertMaterial({
    color: this.formalLegsMMat,
    shading: THREE.FlatShading
  });

  this.shoesMat = new THREE.MeshLambertMaterial({
    color: this.formalShoesMat,
    shading: THREE.FlatShading
  });


  //head
  var head = new THREE.BoxGeometry(300, 350, 280);
  this.head = new THREE.Mesh(head, this.skinMat);
  this.head.position.x = 0;
  this.head.position.y = 160;
  this.head.position.z = 400;


  //glasses
  var glass = new THREE.BoxGeometry(120, 78, 10);
  //Retinas Left
  this.glassLeft = new THREE.Mesh(glass, this.eyeMat);
  this.glassLeft.position.x = -80;
  this.glassLeft.position.y = 4;
  this.glassLeft.position.z = 160;
  //Retinas Right
  this.glassRight = new THREE.Mesh(glass, this.eyeMat);
  this.glassRight.position.x = 80;
  this.glassRight.position.y = 4;
  this.glassRight.position.z = 160;

  //glass middle
  var glassu = new THREE.BoxGeometry(40, 10, 10);
  //Retinas Left
  this.glassu = new THREE.Mesh(glassu, this.pupilaMat);
  this.glassu.position.x = 0;
  this.glassu.position.y = 5;
  this.glassu.position.z = 155;

  //Retinas
  var retina = new THREE.BoxGeometry(25, 25, 5);
  //Retinas Left
  this.retinaLeft = new THREE.Mesh(retina, this.pupilaMat);
  this.retinaLeft.position.x = -80;
  this.retinaLeft.position.y = 5;
  this.retinaLeft.position.z = 168;
  //Retinas Right
  this.retinaRight = new THREE.Mesh(retina, this.pupilaMat);
  this.retinaRight.position.x = 80;
  this.retinaRight.position.y = 5;
  this.retinaRight.position.z = 168;

  //beard
  var beard = new THREE.BoxGeometry(140, 130, 10);
  this.beard = new THREE.Mesh(beard, this.bearMat);
  this.beard.position.x = 0;
  this.beard.position.z = 160;
  this.beard.position.y = -140;

  //mout
  var mout = new THREE.BoxGeometry(90, 60, 50);
  this.mout = new THREE.Mesh(mout, this.skinMat);
  this.mout.position.x = 0;
  this.mout.position.z = 155;
  this.mout.position.y = -130;

  //lip
  var lip = new THREE.BoxGeometry(40, 20, 50);
  this.lip = new THREE.Mesh(lip, this.lipMat);
  this.lip.position.x = 0;
  this.lip.position.z = 162;
  this.lip.position.y = -120;

  //Hat
  var hatTop = new THREE.BoxGeometry(320, 120, 290);
  this.hatTop = new THREE.Mesh(hatTop, this.hatMat);
  this.hatTop.position.x = 0;
  this.hatTop.position.z = 0;
  this.hatTop.position.y = 180;

  var hatBottom = new THREE.BoxGeometry(400, 40, 380);
  this.hatBottom = new THREE.Mesh(hatBottom, this.hatMat);
  this.hatBottom.position.x = 0;
  this.hatBottom.position.z = 0;
  this.hatBottom.position.y = 100;

  //body

  var body = new THREE.BoxGeometry(300, 250, 600);
  this.body = new THREE.Mesh(body, this.smokingMat);
  this.body.position.x = 0;
  this.body.position.y = -220;
  this.body.position.z = 100;

  //arms

  var arm = new THREE.BoxGeometry(50, 250, 100);

  this.armLeft = new THREE.Mesh(arm, this.smokingMat);
  this.armLeft.position.x = -170;
  this.armLeft.position.y = 0;
  this.armLeft.position.z = 200;

  this.armRight = new THREE.Mesh(arm, this.smokingMat);
  this.armRight.position.x = 170;
  this.armRight.position.y = 0;
  this.armRight.position.z = 200;


  // hands

  var hand = new THREE.BoxGeometry(50, 50, 50);

  this.handLeft = new THREE.Mesh(hand, this.skinMat);
  this.handLeft.position.x = -170;
  this.handLeft.position.y = -150;
  this.handLeft.position.z = 220;

  this.handRight = new THREE.Mesh(hand, this.skinMat);
  this.handRight.position.x = 170;
  this.handRight.position.y = -150;
  this.handRight.position.z = 220;

  //zipper

  var zipper = new THREE.BoxGeometry(80, 250, 10);
  this.zipper = new THREE.Mesh(zipper, this.zipperMat);
  this.zipper.position.x = 0;
  this.zipper.position.y = 0;
  this.zipper.position.z = 300;

  //legs

  var legs = new THREE.BoxGeometry(200, 300, 300);
  this.legs = new THREE.Mesh(legs, this.smokingMat);
  this.legs.position.x = 0;
  this.legs.position.y = -200;
  this.legs.position.z = 100;

  //legsMiddle

  var legsM = new THREE.BoxGeometry(10, 130, 5);
  this.legsM = new THREE.Mesh(legsM, this.legsMMat);
  this.legsM.position.x = 0;
  this.legsM.position.y = -280;
  this.legsM.position.z = 248;

  //shoes

  var shoes = new THREE.BoxGeometry(200, 50, 400);
  this.shoes = new THREE.Mesh(shoes, this.shoesMat);
  this.shoes.position.x = 0;
  this.shoes.position.y = -400;
  this.shoes.position.z = 100;


  // group elements

  this.head.add(this.hatTop);
  this.head.add(this.hatBottom);

  this.head.add(this.glassu);
  this.head.add(this.glassLeft);
  this.head.add(this.glassRight);
  this.head.add(this.retinaLeft);
  this.head.add(this.retinaRight);
  this.head.add(this.beard);
  this.head.add(this.mout);
  this.head.add(this.lip);

  this.body.add(this.armLeft);
  this.body.add(this.armRight);
  this.body.add(this.zipper);
  this.body.add(this.handLeft);
  this.body.add(this.handRight);
  this.body.add(this.legs);
  this.body.add(this.legsM);
  this.body.add(this.shoes);

  this.threegroup.add(this.head);
  this.threegroup.add(this.body);


  this.update = function () {
    //move body
    this.bodyRY = calc(mouse.x.calc, -400, 400, -Math.PI / 20, Math.PI / 20);
    this.bodyRX = calc(mouse.y.calc, -400, 400, -Math.PI / 90, Math.PI / 90);
    //move head
    this.headRY = calc(mouse.x.calc, -200, 200, -Math.PI / 4, Math.PI / 4);
    this.headRX = calc(mouse.y.calc, -200, 200, -Math.PI / 4, Math.PI / 4);

    this.rotate(10);
  }

  this.rotate = function (speed) {

    if (isUp) {

      if (this.beard.scale.y < 2) {
        this.beard.scale.y += 0.02;
        this.beard.position.y -= 1.3;
        this.body.position.z -= 2;
      }

      this.body.material.color = new THREE.Color(this.informalSmokingMat);
      this.legsM.material.color = new THREE.Color(this.informalLegsMMat);
      this.zipper.material.color = new THREE.Color(this.informalZipperMat);
      this.shoes.material.color = new THREE.Color(this.informalShoesMat);

      this.zipper.scale.x = this.zipper.scale.x = 0.2;
      this.hatTop.scale.x = this.hatBottom.scale.x = 0;
      this.hatTop.scale.y = this.hatBottom.scale.y = 0;
      this.hatTop.scale.z = this.hatBottom.scale.z = 0;
      this.lip.scale.x = 0.5;

      this.retinaLeft.rotateZ(0.1);
      this.retinaRight.rotateZ(-0.1);
      this.handLeft.rotateY(0.1);
      this.handRight.rotateY(-0.1);

    } else {

      this.beard.position.set(0, -140, 160);
      this.beard.scale.y = 1;
      this.body.position.z = 100;

      this.body.material.color = new THREE.Color(this.formalSmokingMat);
      this.legsM.material.color = new THREE.Color(this.formalLegsMMat);
      this.zipper.material.color = new THREE.Color(this.formalZipperMat);
      this.shoes.material.color = new THREE.Color(this.formalShoesMat);

      this.zipper.scale.x = this.zipper.scale.x = 1;
      this.hatTop.scale.x = this.hatBottom.scale.x = 1;
      this.hatTop.scale.y = this.hatBottom.scale.y = 1;
      this.hatTop.scale.z = this.hatBottom.scale.z = 1;
      this.lip.scale.x = 1;

      this.retinaLeft.rotation.z = 0;
      this.retinaRight.rotation.z = 0;
      this.handLeft.rotation.y = 0;
      this.handRight.rotation.y = 0;


    }

    this.body.rotation.y += (this.bodyRY - this.body.rotation.y) / speed;
    this.body.rotation.x += (this.bodyRX - this.body.rotation.x) / speed;
    this.head.scale.x = this.head.scale.y = this.head.scale.z = 1;
    this.head.rotation.y += (this.headRY - this.head.rotation.y) / speed;
    this.head.rotation.x += (this.headRX - this.head.rotation.x) / speed;
  }

}


function calc(v, vmin, vmax, tmin, tmax) {
  var nv = Math.max(Math.min(v, vmax), vmin);
  var dv = vmax - vmin;
  var pc = (nv - vmin) / dv;
  var dt = tmax - tmin;
  var tv = tmin + (pc * dt);
  return tv;
}

function loop() {
  try {
    throw new Error('This is an error');
    renderer.render(scene, camera);
    walter.update();
  } catch (err) {
    console.info('Caught error:', err.stack);
  }
}

try {
  init(() => {
    renderer.setAnimationLoop(loop);
  });
  addLights();
  createWalter();

  {
    const box = new THREE.Box3();
    scene.traverse(object => {
      if (object instanceof THREE.Mesh || object instanceof THREE.Group) {
        box.expandByObject(object);
      }
    });
    const size = box.getSize(new THREE.Vector3());
    const scale = 1 / Math.max(size.x, size.y, size.z);
    console.info('Scaling scene by', scale);
    scene.scale.set(scale, scale, scale);
  }
} catch (err) {
  console.error(err);
}