import OES_element_index_uint from './oes_element_index_uint';
import OES_standard_derivatives from './oes_standard_derivatives';

export function getExtension(gl: WebGLRenderingContext, name: string) {
  if (name === 'OES_element_index_uint') {
    return new OES_element_index_uint();
  } else if (name === 'OES_standard_derivatives') {
    return new OES_standard_derivatives();
  }
  return null;
}
