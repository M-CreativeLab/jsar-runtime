precision mediump float;

varying vec3 v_color;
varying float v_opacity;
varying vec2 v_texCoord;

void main() {
  // Calculate distance from center of quad (0.5, 0.5)
  vec2 coord = v_texCoord - vec2(0.5, 0.5);
  float dist = dot(coord, coord);
  
  // Gaussian falloff with proper normalization
  float alpha = exp(-dist * 8.0) * v_opacity;
  
  // Discard pixels with very low alpha to improve performance
  if (alpha < 0.01) {
    discard;
  }
  
  gl_FragColor = vec4(v_color * alpha, alpha);
}