precision mediump float;

in vec3 vColor;
in float vOpacity;
in vec2 vTexCoord;

out vec4 fragColor;

void main() {
  // Calculate distance from center of quad (0.5, 0.5)
  vec2 coord = vTexCoord - vec2(0.5, 0.5);
  float dist = dot(coord, coord);

  // Gaussian falloff with proper normalization
  float alpha = exp(-dist * 8.0) * vOpacity;

  // Discard pixels with very low alpha to improve performance
  if(alpha < 0.001) {
    discard;
  }

  fragColor = vec4(vColor, alpha);
}