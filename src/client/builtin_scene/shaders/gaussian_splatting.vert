precision highp float;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec2 renderSize;
uniform float maxStdDev;
uniform float minAlpha;
uniform float maxPixelRadius;
uniform float clipXY;

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

vec4 mat3ToQuat(mat3 m) {
  float trace = m[0][0] + m[1][1] + m[2][2];
  vec4 q;

  if(trace > 0.0) {
    float s = sqrt(trace + 1.0) * 2.0;
    q.w = 0.25 * s;
    q.x = (m[2][1] - m[1][2]) / s;
    q.y = (m[0][2] - m[2][0]) / s;
    q.z = (m[1][0] - m[0][1]) / s;
  } else if((m[0][0] > m[1][1]) && (m[0][0] > m[2][2])) {
    float s = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2.0;
    q.w = (m[2][1] - m[1][2]) / s;
    q.x = 0.25 * s;
    q.y = (m[0][1] + m[1][0]) / s;
    q.z = (m[0][2] + m[2][0]) / s;
  } else if(m[1][1] > m[2][2]) {
    float s = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2.0;
    q.w = (m[0][2] - m[2][0]) / s;
    q.x = (m[0][1] + m[1][0]) / s;
    q.y = 0.25 * s;
    q.z = (m[1][2] + m[2][1]) / s;
  } else {
    float s = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2.0;
    q.w = (m[1][0] - m[0][1]) / s;
    q.x = (m[0][2] + m[2][0]) / s;
    q.y = (m[1][2] + m[2][1]) / s;
    q.z = 0.25 * s;
  }
  return normalize(q);
}

mat3 quatToMat(vec4 q) {
    // assumes q normalized
  float x = q.x, y = q.y, z = q.z, w = q.w;
  float xx = x * x, yy = y * y, zz = z * z;
  float xy = x * y, xz = x * z, yz = y * z;
  float wx = w * x, wy = w * y, wz = w * z;
  return mat3(1.0 - 2.0 * (yy + zz), 2.0 * (xy - wz), 2.0 * (xz + wy), 2.0 * (xy + wz), 1.0 - 2.0 * (xx + zz), 2.0 * (yz - wx), 2.0 * (xz - wy), 2.0 * (yz + wx), 1.0 - 2.0 * (xx + yy));
}

void eigenDecomposeSym2(
  float a,
  float b,
  float d,
  float det,
  out float l1,
  out float l2,
  out vec2 v1,
  out vec2 v2
) {
  float trace = a + d;
  float mid = 0.5 * trace;
  float disc = max(0.0, mid * mid - det);
  float root = sqrt(disc);
  l1 = mid + root;
  l2 = mid - root;
  
  v1 = normalize(vec2((abs(b) < 0.001) ? 1.0 : b, l1 - a));
  v2 = vec2(-v1.y, v1.x);
}

void main() {
  // Default to outside the frustum so it's discarded if we return early
  gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
  vRgba = vec4(splatColor, splatOpacity);

  // Early alpha test
  if (splatOpacity < minAlpha) {
    return;
  }

  // Check for zero scales
  bvec3 zeroScales = equal(splatScale, vec3(0.0));
  if (all(zeroScales)) {
    return;
  }

  // Transform splat center to world space then view space
  vec3 viewCenter = quatVec(renderToViewQuat, center) + renderToViewPos;
  // vec3 viewCenter = (viewMatrix * vec4(splatPosition, 1.0)).xyz;
  vec4 clipCenter = projectionMatrix * vec4(viewCenter, 1.0);

  // Discard splats behind the camera
  if (viewCenter.z >= 0.0) {
    return;
  }

  // Discard splats outside near/far planes
  if (abs(clipCenter.z) >= clipCenter.w) {
    return;
  }

  float clip = clipXY * clipCenter.w;
  if (abs(clipCenter.x) > clip || abs(clipCenter.y) > clip) {
    return;
  }

  // Compute the 3D covariance matrix of the splat
  mat3 viewRotationMatrix = mat3(viewMatrix);
  mat3 splatRotationMatrix = quatToMat(normalize(splatRotation));
  mat3 S = mat3(splatScale.x, 0.0, 0.0, 0.0, splatScale.y, 0.0, 0.0, 0.0, splatScale.z);

  mat3 RS = viewRotationMatrix * (splatRotationMatrix * S);
  mat3 cov3D = RS * transpose(RS);

  // Compute the Jacobian of the splat's projection at its center
  float focalAdjustment = 1.0;
  vec2 scaledRenderSize = renderSize * focalAdjustment;
  vec2 focal = 0.5 * scaledRenderSize * vec2(projectionMatrix[0][0], projectionMatrix[1][1]);
  float invZ = 1.0 / viewCenter.z;

  vec2 J1 = focal * invZ;
  vec2 J2 = -(J1 * viewCenter.xy) * invZ;
  mat3 J = mat3(J1.x, 0.0, J2.x, 0.0, J1.y, J2.y, 0.0, 0.0, 0.0);

  // Compute the 2D covariance by projecting the 3D covariance
  mat3 cov2D = transpose(J) * cov3D * J;
  float a = cov2D[0][0];
  float d = cov2D[1][1];
  float b = cov2D[0][1];

  a = max(a, 0.0);
  d = max(d, 0.0);

  // Store the original determinant for later
  float origDet = a * d - b * b;

  // Add small blur amount for anti-aliasing
  float blurAmount = 0.15;
  a += blurAmount;
  d += blurAmount;

  // Compute the determinant of the 2D covariance matrix after adjustment
  float det = a * d - b * b;

  // Compute anti-aliasing intensity scaling factor
  float blurAdjust = sqrt(max(0.0, origDet / det));
  vRgba.a *= blurAdjust;
  if (vRgba.a < minAlpha) {
    return;
  }

  // Compute the eigenvalue and eigenvectors of the 2D covariance matrix
  float l1, l2;
  vec2 ev1, ev2;
  eigenDecomposeSym2(a, b, d, det, l1, l2, ev1, ev2);

  float radius1 = min(maxPixelRadius, maxStdDev * sqrt(l1));
  float radius2 = min(maxPixelRadius, maxStdDev * sqrt(l2));
  vec2 pixelOffset = ev1 * (position.x * radius1) + ev2 * (position.y * radius2);

  // Compute the NDC coordinates for the ellipsoid's diagonal axes.
  vec2 ndcOffset = pixelOffset / (0.5 * scaledRenderSize);
  vec3 ndcCenter = clipCenter.xyz / clipCenter.w;
  vec3 ndc = vec3(ndcCenter.xy + ndcOffset, ndcCenter.z);

  // Pass data to fragment shader
  vSplatUv = position.xy * maxStdDev;
  vRgba.rgb *= vRgba.a; // Premultiply alpha
  vNdc = ndc;
  gl_Position = vec4(ndc.xy * clipCenter.w, clipCenter.zw);
}
