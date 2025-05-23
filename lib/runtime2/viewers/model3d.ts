import xsmlTemplate from './templates/xsml_template';
import threeTemplate from './templates/three_template';


export type Model3dViewerOptions = {
  playAnimation?: boolean;
};

export default function createViewer(modelUrl: string, options: Model3dViewerOptions = {}) {
  return threeTemplate(modelUrl, options);
  return xsmlTemplate(modelUrl, options);
}