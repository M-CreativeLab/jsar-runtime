precision mediump float;

in vec4 vRgba;
in vec2 vSplatUv;

out vec4 fragColor;

void main() {
  // Calculate distance from center - improved Gaussian falloff
  float dist = dot(vSplatUv, vSplatUv);
  
  // Better Gaussian falloff for 3D Gaussian Splatting
  float alpha = exp(-0.5 * dist) * vRgba.a;
  
  // Discard pixels with very low alpha
  if (alpha < 0.001) {
    discard;
  }
  
  fragColor = vec4(vRgba.rgb, alpha);
}