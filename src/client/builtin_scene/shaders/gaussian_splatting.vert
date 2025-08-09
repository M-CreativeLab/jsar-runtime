// Base quad attributes (per vertex)
in vec2 position;

// Instanced attributes (per splat)
in vec3 splatPosition;
in vec3 splatColor;
in float splatOpacity;
in vec3 splatScale;
in vec4 splatRotation;

uniform mat4 viewProjection;
#ifdef MULTIVIEW
uniform mat4 viewProjectionR;
#endif
uniform mat4 modelMatrix;
uniform vec2 uQuadSize;

out vec3 vColor;
out float vOpacity;
out vec2 vTexCoord;

// Rotate a vector by a quaternion
vec3 rotateByQuaternion(vec3 v, vec4 q) {
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main() {
  // Pass texture coordinate from base quad position
  vTexCoord = position + vec2(0.5, 0.5);
  
  // Create billboard quad in world space
  vec3 localPos = vec3(position * uQuadSize, 0.0);
  
  // Apply splat scaling
  localPos *= splatScale;
  
  // Apply splat rotation
  vec3 rotatedPos = rotateByQuaternion(localPos, splatRotation);
  
  // Translate to splat position
  vec3 worldPos = splatPosition + rotatedPos;
  
  // Apply model matrix then view-projection matrix
  vec4 modelPos = modelMatrix * vec4(worldPos, 1.0);
  gl_Position = viewProjection * modelPos;
  
  vColor = splatColor;
  vOpacity = splatOpacity;
}