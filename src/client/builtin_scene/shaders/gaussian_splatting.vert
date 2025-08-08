// Base quad attributes (per vertex)
attribute vec2 position;

// Instanced attributes (per splat)
attribute vec3 splatPosition;
attribute vec3 splatColor;
attribute float splatOpacity;
attribute vec3 splatScale;
attribute vec4 splatRotation;

uniform mat4 uMvpMatrix;
uniform mat4 uViewMatrix;
uniform vec2 uQuadSize;

varying vec3 vColor;
varying float vOpacity;
varying vec2 vTexCoord;

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
  
  gl_Position = uMvpMatrix * vec4(worldPos, 1.0);
  
  vColor = splatColor;
  vOpacity = splatOpacity;
}