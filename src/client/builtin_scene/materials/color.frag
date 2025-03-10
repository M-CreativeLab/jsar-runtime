uniform vec4 surfaceColor;

#ifdef USE_TEXTURE
uniform sampler2D tex;
uniform mat3 textureTransformation;
#endif

#ifdef USE_UVS
in vec2 uvs;
#endif

in vec4 col;
layout(location = 0) out vec4 outColor;

void main() {
  // Set background color
  outColor = surfaceColor;

  // Multiply by color
  outColor *= col;

#ifdef USE_TEXTURE
  vec4 textureColor = texture(tex, (textureTransformation * vec3(uvs, 1.0)).xy);
  outColor = mix(outColor, textureColor, textureColor.a);
#endif

#ifdef DEBUG_UV0
  outColor = vec4((textureTransformation * vec3(uvs, 1.0)).xy, 1.0,
                  1.0); // Debug texture coordinates
#endif
}
