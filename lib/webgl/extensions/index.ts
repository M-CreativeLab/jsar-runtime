import OES_element_index_uint from './oes_element_index_uint';
import OES_standard_derivatives from './oes_standard_derivatives';
import OES_texture_float_linear from './oes_texture_float_linear';
import OVR_multiview2 from './ovr_multiview';
// import OCULUS_multiview from './oculus_multiview';

export function getExtension(_gl: WebGLRenderingContext, name: string) {
  switch (name) {
    case 'OES_element_index_uint':
      return new OES_element_index_uint();
    case 'OES_standard_derivatives':
      return new OES_standard_derivatives();
    case 'OES_texture_float_linear':
      return new OES_texture_float_linear();
    case 'OVR_multiview2':
      return new OVR_multiview2();
    // TODO: enable OCULUS_multiview?
    // case 'OCULUS_multiview':
    //   return new OCULUS_multiview();
    default:
      return null;
  }
}
