uniform mat4 viewProjection;
#ifdef MULTIVIEW
uniform mat4 viewProjectionR;
#endif
uniform mat4 modelMatrix;
uniform vec2 renderSize;
uniform float maxStdDev;
uniform float minAlpha;

// Base quad attributes (per vertex)
in vec3 position;

// Instanced attributes (per splat)
in vec3 splatPosition;
in vec3 splatColor;
in float splatOpacity;
in vec3 splatScale;
in vec4 splatRotation;

out vec4 vRgba;
out vec2 vSplatUv;

// Helper functions for quaternion operations
vec3 quatVec(vec4 q, vec3 v) {
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

// Convert scale and quaternion to 3x3 matrix
mat3 scaleQuaternionToMatrix(vec3 scale, vec4 quat) {
  vec4 q = normalize(quat);
  float x = q.x, y = q.y, z = q.z, w = q.w;
  
  mat3 rotMat = mat3(
    1.0 - 2.0 * (y*y + z*z), 2.0 * (x*y - w*z), 2.0 * (x*z + w*y),
    2.0 * (x*y + w*z), 1.0 - 2.0 * (x*x + z*z), 2.0 * (y*z - w*x),
    2.0 * (x*z - w*y), 2.0 * (y*z + w*x), 1.0 - 2.0 * (x*x + y*y)
  );
  
  return mat3(
    rotMat[0] * scale.x,
    rotMat[1] * scale.y,
    rotMat[2] * scale.z
  );
}

void main() {
  // Early alpha test
  if (splatOpacity < minAlpha) {
    gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
    return;
  }

  // Check for zero scales
  if (length(splatScale) < 0.001) {
    gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
    return;
  }

  // Transform splat center to world space
  vec4 worldCenter = modelMatrix * vec4(splatPosition, 1.0);
  
  // Project center to clip space to get the splat's screen position
  vec4 clipCenter = viewProjection * worldCenter;
  
  // Discard splats outside near/far planes
  if (abs(clipCenter.z) >= clipCenter.w || clipCenter.w <= 0.0) {
    gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
    return;
  }

  // Compute NDC center
  vec3 ndcCenter = clipCenter.xyz / clipCenter.w;
  
  // Basic frustum culling
  if (abs(ndcCenter.x) > 1.5 || abs(ndcCenter.y) > 1.5) {
    gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
    return;
  }

  // Create rotation-scale matrix for the splat
  mat3 RS = scaleQuaternionToMatrix(splatScale, splatRotation);
  
  // Apply splat transformation to the quad vertex
  vec3 localOffset = RS * vec3(position.xy * maxStdDev, 0.0);
  
  // Transform offset to world space (simplified - should be in view space for proper perspective)
  vec3 worldOffset = (modelMatrix * vec4(localOffset, 0.0)).xyz;
  
  // Project the offset point
  vec4 offsetPoint = viewProjection * vec4(worldCenter.xyz + worldOffset, 1.0);
  
  // Compute final position
  if (offsetPoint.w > 0.0) {
    gl_Position = offsetPoint;
  } else {
    gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
    return;
  }

  // Pass data to fragment shader
  vRgba = vec4(splatColor, splatOpacity);
  vSplatUv = position.xy;

#ifdef MULTIVIEW
  if (VIEW_ID != 0u) {
    // For right eye, we'd need to recalculate with viewProjectionR
    // For now, use the same calculation
  }
#endif
}
