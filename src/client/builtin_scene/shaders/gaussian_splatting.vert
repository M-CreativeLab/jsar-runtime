uniform mat4 viewProjection;
#ifdef MULTIVIEW
uniform mat4 viewProjectionR;
#endif
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec2 renderSize;
uniform float maxStdDev;
uniform float minAlpha;
uniform float maxPixelRadius;

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
out vec3 vNdc;

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
  // Default to outside the frustum so it's discarded if we return early
  gl_Position = vec4(0.0, 0.0, 2.0, 1.0);

  // Early alpha test
  if (splatOpacity < minAlpha) {
    return;
  }

  // Check for zero scales
  if (length(splatScale) < 0.001) {
    return;
  }

  // Transform splat center to world space then view space
  vec4 worldCenter = modelMatrix * vec4(splatPosition, 1.0);
  vec4 viewCenter = viewMatrix * worldCenter;
  
  // Discard splats behind the camera
  if (viewCenter.z >= 0.0) {
    return;
  }

  // Compute the clip space center of the splat
  vec4 clipCenter = projectionMatrix * viewCenter;
  
  // Discard splats outside near/far planes
  if (abs(clipCenter.z) >= clipCenter.w) {
    return;
  }

  // Compute NDC center of the splat
  vec3 ndcCenter = clipCenter.xyz / clipCenter.w;
  
  // Basic frustum culling - more generous bounds
  float clipXY = 1.5;
  float clip = clipXY * clipCenter.w;
  if (abs(clipCenter.x) > clip || abs(clipCenter.y) > clip) {
    return;
  }

  // Compute the 3D covariance matrix of the splat in view space
  // Transform quaternion to view space (simplified - assumes model matrix is mostly identity for rotation)
  vec4 viewQuaternion = splatRotation;
  mat3 RS = scaleQuaternionToMatrix(splatScale, viewQuaternion);
  mat3 cov3D = RS * transpose(RS);

  // Compute the Jacobian of the splat's projection at its center
  vec2 focal = 0.5 * renderSize * vec2(projectionMatrix[0][0], projectionMatrix[1][1]);
  float invZ = 1.0 / viewCenter.z;
  vec2 J1 = focal * invZ;
  vec2 J2 = -(J1 * viewCenter.xy) * invZ;
  mat3 J = mat3(
    J1.x, 0.0, J2.x,
    0.0, J1.y, J2.y,
    0.0, 0.0, 0.0
  );

  // Compute the 2D covariance by projecting the 3D covariance
  mat3 cov2D = transpose(J) * cov3D * J;
  float a = cov2D[0][0];
  float d = cov2D[1][1];
  float b = cov2D[0][1];

  // Add small blur amount for anti-aliasing
  float blurAmount = 0.3;
  a += blurAmount;
  d += blurAmount;
  float det = a * d - b * b;

  // Early exit if determinant is too small
  if (det <= 0.0) {
    return;
  }

  // Compute the eigenvalue and eigenvectors of the 2D covariance matrix
  float eigenAvg = 0.5 * (a + d);
  float eigenDelta = sqrt(max(0.0, eigenAvg * eigenAvg - det));
  float eigen1 = eigenAvg + eigenDelta;
  float eigen2 = eigenAvg - eigenDelta;

  vec2 eigenVec1 = normalize(vec2((abs(b) < 0.001) ? 1.0 : b, eigen1 - a));
  vec2 eigenVec2 = vec2(eigenVec1.y, -eigenVec1.x);

  float scale1 = position.x * min(maxPixelRadius, maxStdDev * sqrt(eigen1));
  float scale2 = position.y * min(maxPixelRadius, maxStdDev * sqrt(eigen2));

  // Compute the NDC coordinates for the ellipsoid's diagonal axes.
  vec2 pixelOffset = eigenVec1 * scale1 + eigenVec2 * scale2;
  vec2 ndcOffset = (2.0 / renderSize) * pixelOffset;
  vec3 ndc = vec3(ndcCenter.xy + ndcOffset, ndcCenter.z);
  gl_Position = vec4(ndc.xy * clipCenter.w, clipCenter.zw);

  // Pass data to fragment shader
  vRgba = vec4(splatColor, splatOpacity);
  vSplatUv = position.xy;
  vNdc = ndc;

#ifdef MULTIVIEW
  if (VIEW_ID != 0u) {
    // For right eye, we'd need to recalculate with viewProjectionR
    // For now, use the same calculation
  }
#endif
}
