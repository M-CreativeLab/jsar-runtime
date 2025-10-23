precision highp float;

uniform float maxStdDev;
uniform float minAlpha;

in vec4 vRgba;
in vec2 vSplatUv;

out vec4 fragColor;

void main() {
  vec4 rgba = vRgba;

  // Calculate distance from center - improved Gaussian falloff
  float z = dot(vSplatUv, vSplatUv);
  if (z > (maxStdDev * maxStdDev)) {
    discard;
  }

  // Better Gaussian falloff for 3D Gaussian Splatting
  const float falloff = 1.0;
  rgba.a *= mix(1.0, exp(-1.5 * z), falloff);

  // Discard pixels with very low alpha
  if (rgba.a < minAlpha) {
    discard;
  }

  fragColor = rgba;
}
