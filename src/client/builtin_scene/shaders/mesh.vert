uniform mat4 viewProjection;
#ifdef MULTIVIEW
uniform mat4 viewProjectionR;
#endif
uniform mat4 modelMatrix;

in vec3 position;
in vec3 normal;
in vec2 texCoord;
out vec3 pos;

#ifdef USE_NORMALS
uniform mat4 normalMatrix;
out vec3 nor;

#ifdef USE_TANGENTS
in vec4 tangent;
out vec3 tang;
out vec3 bitang;
#endif
#endif

#ifdef USE_UVS
uniform mat3 textureTransformation;
out vec2 uvs;
#endif

#ifdef USE_VERTEX_COLORS
in vec4 color;
#endif

#ifdef USE_INSTANCE_TRANSFORMS
in mat4 instanceTransform;
#endif

#ifdef USE_INSTANCE_COLORS
in vec4 instanceColor;
#endif

#ifdef USE_UVS
#ifdef USE_INSTANCE_TEXTURE
in vec2 instanceTexUvOffset;
in vec2 instanceTexUvScale;
in uint instanceLayerIndex;
out float vInstanceLayerIndex;
out float vInstanceTextureEnabled;
#endif
#endif

out vec4 col;
flat out int instance_id;

void main() {
  // *** POSITION ***
  mat4 local2World = modelMatrix;

#ifdef USE_INSTANCE_TRANSFORMS
  local2World *= instanceTransform;
#endif

  vec4 worldPosition = local2World * vec4(position, 1.);
  worldPosition /= worldPosition.w;

#ifdef PARTICLES
  worldPosition.xyz +=
      start_position + start_velocity * time + 0.5 * acceleration * time * time;
#endif

#ifdef MULTIVIEW
  if (VIEW_ID == 0u) {
    gl_Position = viewProjection * worldPosition;
  } else {
    gl_Position = viewProjectionR * worldPosition;
  }
#else
  gl_Position = viewProjection * worldPosition;
#endif

  pos = worldPosition.xyz;

  // *** NORMAL ***
#ifdef USE_NORMALS

#ifdef USE_INSTANCE_TRANSFORMS
  mat3 normalMat = mat3(transpose(inverse(local2World)));
#else
  mat3 normalMat = mat3(normalMatrix);
#endif
  nor = normalize(normalMat * normal);

#ifdef USE_TANGENTS
  tang = normalize(normalMat * tangent.xyz);
  bitang = normalize(cross(nor, tang) * tangent.w);
#endif

#endif

  // UV
#ifdef USE_UVS
  vec3 transformedUv = textureTransformation * vec3(texCoord, 1.0);
  uvs = transformedUv.xy;

  // Instance Texture
#ifdef USE_INSTANCE_TEXTURE
  uvs = instanceTexUvOffset + instanceTexUvScale * uvs;
  vInstanceLayerIndex = float(instanceLayerIndex);

  float threshold = 1e-5;
  vInstanceTextureEnabled = step(threshold, abs(instanceTexUvScale.x)) *
                            step(threshold, abs(instanceTexUvScale.y));
#endif
#endif

  // *** COLOR ***
  col = vec4(1.0);
#ifdef USE_VERTEX_COLORS
  col *= color;
#endif
#ifdef USE_INSTANCE_COLORS
  col *= instanceColor;
#endif
  instance_id = gl_InstanceID;
}
