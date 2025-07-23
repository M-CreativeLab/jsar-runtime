attribute vec3 a_position;
attribute vec3 a_color;
attribute float a_opacity;
attribute vec3 a_scale;
attribute vec4 a_rotation;

uniform mat4 u_mvpMatrix;
uniform mat4 u_viewMatrix;

varying vec3 v_color;
varying float v_opacity;
varying vec2 v_texCoord;

vec3 rotateByQuaternion(vec3 v, vec4 q) {
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main() {
  // Apply gaussian scaling and rotation
  vec3 scaledPos = a_position * a_scale;
  vec3 rotatedPos = rotateByQuaternion(scaledPos, a_rotation);
  
  gl_Position = u_mvpMatrix * vec4(rotatedPos, 1.0);
  
  v_color = a_color;
  v_opacity = a_opacity;
  v_texCoord = vec2(0.5, 0.5); // Center of gaussian
  
  // Point size for gaussian splatting
  gl_PointSize = 10.0;
}