precision highp float;

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
  out float l1,
  out float l2,
  out vec2 v1,
  out vec2 v2
) {
  float trace = a + d;
  float det   = a*d - b*b;
  float mid = 0.5 * trace;
  float disc = mid*mid - det;
  if (disc < 0.0) disc = 0.0;
  float root = sqrt(disc);
  l1 = mid + root;
  l2 = mid - root;

  vec2 v;
  if (abs(b) > 1e-12) {
    if (abs(l1 - a) > abs(b)) {
      v = vec2(l1 - d, b);
    } else {
      v = vec2(b, l1 - a);
    }
  } else {
    v = (a >= d) ? vec2(1,0) : vec2(0,1);
  }

  float len2 = dot(v,v);
  if (len2 < 1e-24) {
    v = vec2(1,0);
  }
  v1 = v * inversesqrt(max(len2, 1e-24));
  v2 = vec2(-v1.y, v1.x);
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
  vec3 viewCenter = (viewMatrix * vec4(splatPosition, 1.0)).xyz;
  vec4 clipCenter = projectionMatrix * vec4(viewCenter, 1.0);

  // Discard splats behind the camera
  if(viewCenter.z >= 0.0) {
    return;
  }

  // Discard splats outside near/far planes
  if (abs(clipCenter.z) >= clipCenter.w) {
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
  float b = 0.5 * (cov2D[0][1] + cov2D[1][0]);

  a = max(a, 0.0);
  d = max(d, 0.0);

  // Add small blur amount for anti-aliasing
  float blurAmount = 0.3;
  a += blurAmount;
  d += blurAmount;

  // Compute the eigenvalue and eigenvectors of the 2D covariance matrix
  float l1, l2;
  vec2 ev1, ev2;
  eigenDecomposeSym2(a, b, d, l1, l2, ev1, ev2);

  l1 = max(l1, 0.0);
  l2 = max(l2, 0.0);

  float s1 = sqrt(l1);
  float s2 = sqrt(l2);

  float radius1 = min(maxPixelRadius, maxStdDev * s1);
  float radius2 = min(maxPixelRadius, maxStdDev * s2);
  vec2 pixelOffset = ev1 * (position.x * radius1) + ev2 * (position.y * radius2);

  // Compute the NDC coordinates for the ellipsoid's diagonal axes.
  vec2 ndcOffset = pixelOffset / (0.5 * scaledRenderSize);
  vec2 ndcCenter = clipCenter.xy / clipCenter.w;
  vec3 ndc = vec3(ndcCenter + ndcOffset, clipCenter.z / clipCenter.w);

  // Pass data to fragment shader
  vRgba = vec4(splatColor, splatOpacity);
  vSplatUv = position.xy * maxStdDev;
  vNdc = ndc;
  gl_Position = vec4(ndc.xy * clipCenter.w, clipCenter.zw);
}
