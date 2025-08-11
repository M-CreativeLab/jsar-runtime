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

vec3 quatVec(vec4 q, vec3 v) {
  vec3 t = 2.0 * cross(q.xyz, v);
  return v + q.w * t + cross(q.xyz, t);
}

vec4 quatQuat(vec4 q1, vec4 q2) {
  return vec4(
    q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
    q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
    q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
  );
}

void decomposeViewMatrix(
  mat4 viewMatrix,
  out vec3 viewPosition,
  out vec4 viewQuaternion
) {
  mat3 viewRotMatrix = mat3(viewMatrix);
  mat3 worldRotMatrix = transpose(viewRotMatrix);
  float det = determinant(viewRotMatrix);

  if (det > 0.0) 
  {
    viewQuaternion = mat3ToQuat(worldRotMatrix);
  }
  else
  {
    worldRotMatrix[2] = -worldRotMatrix[2];
    viewQuaternion = mat3ToQuat(worldRotMatrix);
  }

  viewPosition = viewMatrix[3].xyz;
}

mat3 scaleQuaternionToMatrix(vec3 s, vec4 q) {
  // Compute the matrix of scaling by s then rotating by q
  return mat3(
      s.x * (1.0 - 2.0 * (q.y * q.y + q.z * q.z)),
      s.x * (2.0 * (q.x * q.y + q.w * q.z)),
      s.x * (2.0 * (q.x * q.z - q.w * q.y)),
      s.y * (2.0 * (q.x * q.y - q.w * q.z)),
      s.y * (1.0 - 2.0 * (q.x * q.x + q.z * q.z)),
      s.y * (2.0 * (q.y * q.z + q.w * q.x)),
      s.z * (2.0 * (q.x * q.z + q.w * q.y)),
      s.z * (2.0 * (q.y * q.z - q.w * q.x)),
      s.z * (1.0 - 2.0 * (q.x * q.x + q.y * q.y))
  );
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

  vec3 renderToViewPos;
  vec4 renderToViewQuat;
  decomposeViewMatrix(viewMatrix, renderToViewPos, renderToViewQuat);

  // Transform splat center to world space then view space
  vec3 worldCenter = (modelMatrix * vec4(splatPosition, 1.0)).xyz;
  vec3 viewCenter = quatVec(renderToViewQuat, worldCenter) + renderToViewPos;
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

  mat3 modelRotationScale = mat3(modelMatrix);
  vec3 modelScale = vec3(
      length(modelRotationScale[0]),
      length(modelRotationScale[1]),
      length(modelRotationScale[2])
  );
  vec3 transformedScale = splatScale * modelScale;
  mat3 modelRotation = mat3(
    modelRotationScale[0] / modelScale.x,
    modelRotationScale[1] / modelScale.y,
    modelRotationScale[2] / modelScale.z
  );
  vec4 modelQuat = mat3ToQuat(modelRotation);
  vec4 transformedRotation = quatQuat(modelQuat, splatRotation);

  vec4 viewQuaternion = quatQuat(renderToViewQuat, transformedRotation);
  mat3 RS = scaleQuaternionToMatrix(transformedScale, viewQuaternion);
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
  vec2 ndcOffset = (2.0 / scaledRenderSize) * pixelOffset;
  vec3 ndcCenter = clipCenter.xyz / clipCenter.w;
  vec3 ndc = vec3(ndcCenter.xy + ndcOffset, ndcCenter.z);

  // Pass data to fragment shader
  vSplatUv = position.xy * maxStdDev;
  vNdc = ndc;
  gl_Position = vec4(ndc.xy * clipCenter.w, clipCenter.zw);
}
