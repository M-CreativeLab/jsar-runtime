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
in vec2 instanceTexUvOffsetR;
in vec2 instanceTexUvScale;
in uint instanceLayerIndex;
in uint instanceUseSDFTexture;
out vec2 vInstanceTexUvOffsetR;
out float vInstanceLayerIndex;
out float vInstanceTextureEnabled;
out float vInstanceUseSDFTexture;
#endif

#ifdef USE_INSTANCE_SDF
in vec2 instanceDimensions;
in vec4 instanceBorderRadius;
in uint instanceBorderStyle;
out vec2 vInstanceTexCoord;
out vec2 vInstanceDimensions;
out vec4 vInstanceBorderRadius;
out float vInstanceBorderStyle;
flat out int vInstanceId;
#endif
#endif

out vec4 col;
flat out int instance_id;

void main()
{
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
  if (VIEW_ID == 0u)
  {
    gl_Position = viewProjection * worldPosition;
  }
  else
  {
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
  vInstanceTexUvOffsetR = instanceTexUvOffsetR;
  vInstanceLayerIndex = float(instanceLayerIndex);
  vInstanceUseSDFTexture = instanceUseSDFTexture;

#ifdef MULTIVIEW
  // In multiview, select texture coordinates based on VIEW_ID
  if (VIEW_ID == 0u)
  {
    // Left eye
    uvs = instanceTexUvOffset + instanceTexUvScale * uvs;
  }
  else
  {
    // Right eye
    uvs = instanceTexUvOffsetR + instanceTexUvScale * uvs;
  }
#else
  // Non-multiview: use left eye coordinates (backward compatibility)
  uvs = instanceTexUvOffset + instanceTexUvScale * uvs;
#endif

  // Set if texture is enabled
  float threshold = 1e-5;
  vInstanceTextureEnabled = step(threshold, abs(instanceTexUvScale.x)) *
                            step(threshold, abs(instanceTexUvScale.y));
#endif

  // Instance SDF
#ifdef USE_INSTANCE_SDF
  vInstanceTexCoord = texCoord;
  vInstanceDimensions = instanceDimensions;
  vInstanceBorderRadius = instanceBorderRadius;
  vInstanceBorderStyle = float(instanceBorderStyle);
  vInstanceId = gl_InstanceID;
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
