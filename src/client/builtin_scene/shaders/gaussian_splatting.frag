precision mediump float;

varying vec3 vColor;
varying float vOpacity;
varying vec2 vTexCoord;

void main() {
  // Calculate distance from center of quad (0.5, 0.5)
  vec2 coord = vTexCoord - vec2(0.5, 0.5);
  float dist = dot(coord, coord);
  
  // Gaussian falloff with proper normalization
  float alpha = exp(-dist * 8.0) * vOpacity;
  
  // Discard pixels with very low alpha to improve performance
  if (alpha < 0.01) {
    discard;
  }
  
  gl_FragColor = vec4(vColor * alpha, alpha);
}