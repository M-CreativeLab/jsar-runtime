uniform mat4 viewProjection;
uniform mat4 modelMatrix;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

#ifdef PARTICLES
in vec3 start_position;
in vec3 start_velocity;
uniform vec3 acceleration;
uniform float time;
#endif

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
out vec2 uvs;
#endif

#ifdef USE_VERTEX_COLORS
in vec4 color;
#endif
#ifdef USE_INSTANCE_COLORS
in vec4 instance_color;
#endif

out vec4 col;
flat out int instance_id;

void main() {
  // *** POSITION ***
  mat4 local2World = modelMatrix;

#ifdef USE_INSTANCE_TRANSFORMS
  mat4 transform;
  transform[0] = vec4(row1.x, row2.x, row3.x, 0.0);
  transform[1] = vec4(row1.y, row2.y, row3.y, 0.0);
  transform[2] = vec4(row1.z, row2.z, row3.z, 0.0);
  transform[3] = vec4(row1.w, row2.w, row3.w, 1.0);
  local2World *= transform;
#endif

  vec4 worldPosition = local2World * vec4(position, 1.);
  worldPosition /= worldPosition.w;
#ifdef PARTICLES
  worldPosition.xyz +=
      start_position + start_velocity * time + 0.5 * acceleration * time * time;
#endif
#ifdef USE_INSTANCE_TRANSLATIONS
  worldPosition.xyz += instance_translation;
#endif
  gl_Position = viewProjection * worldPosition;

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

  // *** UV ***
#ifdef USE_UVS
  uvs = texCoord;
#endif

  // *** COLOR ***
  col = vec4(1.0);
#ifdef USE_VERTEX_COLORS
  col *= color;
#endif
#ifdef USE_INSTANCE_COLORS
  col *= instance_color;
#endif
  instance_id = gl_InstanceID;
}
