type Model3dViewerOptions = {
  playAnimation?: boolean;
};

export default function createViewer(modelUrl: string, options: Model3dViewerOptions = {}) {
  let playAnimationSource = '';
  if (options.playAnimation === true) {
    playAnimationSource = `
    const animations = scene.animationGroups
      .filter(ag => ag.name.startsWith('model.'));
    if (animations.length > 0) {
      animations[0].start(true);
    }
    `;
  }
  return `
<xsml>
  <head>
    <title>Builtin Model 3D Viewer</title>
    <link id="my" rel="mesh" href="${modelUrl}" />
  </head>
  <space>
    <mesh ref="my" id="model" />
  </space>
  <script>
  spatialDocument.addEventListener('spaceReady', () => {
    const scene = spatialDocument.scene;
    ${playAnimationSource}
  });
  </script>
</xsml>
  `;
}