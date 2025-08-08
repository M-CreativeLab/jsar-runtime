// Base quad attributes (per vertex)
attribute vec2 a_position;

// Instanced attributes (per splat)
attribute vec3 a_splatPosition;
attribute vec3 a_splatColor;
attribute float a_splatOpacity;
attribute vec3 a_splatScale;
attribute vec4 a_splatRotation;

uniform mat4 u_mvpMatrix;
uniform mat4 u_viewMatrix;
uniform vec2 u_quadSize;

varying vec3 v_color;
varying float v_opacity;
varying vec2 v_texCoord;

// Rotate a vector by a quaternion
vec3 rotateByQuaternion(vec3 v, vec4 q) {
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main() {
  // Pass texture coordinate from base quad position
  v_texCoord = a_position + vec2(0.5, 0.5);
  
  // Create billboard quad in world space
  vec3 localPos = vec3(a_position * u_quadSize, 0.0);
  
  // Apply splat scaling
  localPos *= a_splatScale;
  
  // Apply splat rotation
  vec3 rotatedPos = rotateByQuaternion(localPos, a_splatRotation);
  
  // Translate to splat position
  vec3 worldPos = a_splatPosition + rotatedPos;
  
  gl_Position = u_mvpMatrix * vec4(worldPos, 1.0);
  
  v_color = a_splatColor;
  v_opacity = a_splatOpacity;
}