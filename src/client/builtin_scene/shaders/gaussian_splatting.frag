precision mediump float;

varying vec3 v_color;
varying float v_opacity;
varying vec2 v_texCoord;

void main() {
  // Gaussian falloff
  vec2 coord = gl_PointCoord - vec2(0.5, 0.5);
  float dist = dot(coord, coord);
  float alpha = exp(-dist * 4.0) * v_opacity;
  
  gl_FragColor = vec4(v_color * alpha, alpha);
}