uniform sampler2D sdfTexture;
uniform float sdfSpread;      // Distance field spread (radius used during generation)
uniform float sdfThickness;  // Text thickness (0.5 = normal, < 0.5 = thinner, > 0.5 = thicker)
uniform float sdfSoftness;   // Edge softness (0.0 = crisp, higher = softer)
uniform vec4 textColor;      // Text color with alpha
uniform float opacity;       // Additional opacity multiplier

in vec2 vUv;
layout(location = 0) out vec4 outColor;

void main() {
  // Sample the SDF texture
  float sdfValue = texture(sdfTexture, vUv).r;
  
  // Convert from [0,1] back to signed distance
  float distance = (sdfValue - 0.5) * 2.0 * sdfSpread;
  
  // Calculate alpha based on distance and thickness
  float alpha = smoothstep(-sdfSoftness, sdfSoftness, distance - (sdfThickness - 0.5) * sdfSpread);
  
  // Apply text color and opacity
  outColor = vec4(textColor.rgb, textColor.a * alpha * opacity);
  
  // Apply color mapping
  outColor.rgb = color_mapping(outColor.rgb);
}