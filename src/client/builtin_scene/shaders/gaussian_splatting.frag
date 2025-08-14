precision mediump float;

uniform float maxStdDev;
uniform float minAlpha;

in vec4 vRgba;
in vec2 vSplatUv;

out vec4 fragColor;

void main() {
  // Calculate distance from center - improved Gaussian falloff
  float dist = dot(vSplatUv, vSplatUv);
  if (dist > (maxStdDev * maxStdDev)) {
    discard;
  }

  // Better Gaussian falloff for 3D Gaussian Splatting
  float alpha = exp(-0.5 * dist) * vRgba.a; // Adjusted for sharper falloff

  // Discard pixels with very low alpha
  if (alpha < minAlpha) {
    discard;
  }

  fragColor = vec4(vRgba.rgb, alpha);
}
