import { Model3dViewerOptions } from "../model3d";


export default function getTemplate(modelUrl, options: Model3dViewerOptions = {}): string {
  let playAnimationSource;
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