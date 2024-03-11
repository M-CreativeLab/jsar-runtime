import OES_element_index_uint from './oes_element_index_uint';
import OES_standard_derivatives from './oes_standard_derivatives';
import OES_texture_float from './oes_texture_float';
import OES_texture_float_linear from './oes_texture_float_linear';

export function getExtension(gl: WebGLRenderingContext, name: string) {
  if (name === 'OES_element_index_uint') {
    return new OES_element_index_uint();
  } else if (name === 'OES_standard_derivatives') {
    return new OES_standard_derivatives();
  } else if (name === 'OES_texture_float') {
    return new OES_texture_float();
  } else if (name === 'OES_texture_float_linear') {
    return new OES_texture_float_linear();
  } else {
    return null;
  }
}
