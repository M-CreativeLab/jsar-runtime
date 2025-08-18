type Model3dViewerOptions = {
  playAnimation?: boolean;
};

export default function createViewer(modelUrl: string, options: Model3dViewerOptions = {}) {
  return `
<!DOCTYPE html>
<html>
  <head>
    <title>Builtin Model 3D Viewer</title>
  </head>
  <body>
    <model src="${modelUrl}" ${options.playAnimation ? 'autoplay' : ''}></model>
  </body>
</html>
  `;
}
