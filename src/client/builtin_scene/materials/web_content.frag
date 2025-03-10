#ifdef USE_INSTANCE_TEXTURE
uniform sampler2DArray instanceTexAltas;
in float vInstanceLayerIndex;
in float vInstanceTextureEnabled;
#endif

#ifdef USE_UVS
in vec2 uvs;
#endif

in vec4 col;
layout(location = 0) out vec4 outColor;

void main() {
  outColor = col;

#ifdef USE_INSTANCE_TEXTURE
  if (vInstanceTextureEnabled == 1.0) {
    vec4 textureColor =
        texture(instanceTexAltas, vec3(uvs, vInstanceLayerIndex));
    outColor = mix(outColor, textureColor, textureColor.a);
  }
#endif
}
