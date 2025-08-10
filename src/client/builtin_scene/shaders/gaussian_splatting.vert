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

vec3 quatVec(vec4 q, vec3 v)
{
    // Rotate vector v by quaternion q
  vec3 t = 2.0 * cross(q.xyz, v);
  return v + q.w * t + cross(q.xyz, t);
}

vec4 quatQuat(vec4 q1, vec4 q2)
{
  return vec4(q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y, q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x, q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w, q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

// Convert scale and quaternion to 3x3 matrix
mat3 scaleQuaternionToMatrix(vec3 s, vec4 q)
{
    // Compute the matrix of scaling by s then rotating by q
  return mat3(s.x * (1.0 - 2.0 * (q.y * q.y + q.z * q.z)), s.x * (2.0 * (q.x * q.y + q.w * q.z)), s.x * (2.0 * (q.x * q.z - q.w * q.y)), s.y * (2.0 * (q.x * q.y - q.w * q.z)), s.y * (1.0 - 2.0 * (q.x * q.x + q.z * q.z)), s.y * (2.0 * (q.y * q.z + q.w * q.x)), s.z * (2.0 * (q.x * q.z + q.w * q.y)), s.z * (2.0 * (q.y * q.z - q.w * q.x)), s.z * (1.0 - 2.0 * (q.x * q.x + q.y * q.y)));
}

vec4 mat3ToQuat(mat3 m)
{
  float tr = m[0][0] + m[1][1] + m[2][2];
  vec4 q;
  if (tr > 0.0)
  {
    float S = sqrt(tr + 1.0) * 2.0;
    q.w = 0.25 * S;
    q.x = (m[2][1] - m[1][2]) / S;
    q.y = (m[0][2] - m[2][0]) / S;
    q.z = (m[1][0] - m[0][1]) / S;
  }
  else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
  {
    float S = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2.0;
    q.w = (m[2][1] - m[1][2]) / S;
    q.x = 0.25 * S;
    q.y = (m[0][1] + m[1][0]) / S;
    q.z = (m[0][2] + m[2][0]) / S;
  }
  else if (m[1][1] > m[2][2])
  {
    float S = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2.0;
    q.w = (m[0][2] - m[2][0]) / S;
    q.x = (m[0][1] + m[1][0]) / S;
    q.y = 0.25 * S;
    q.z = (m[1][2] + m[2][1]) / S;
  }
  else
  {
    float S = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2.0;
    q.w = (m[1][0] - m[0][1]) / S;
    q.x = (m[0][2] + m[2][0]) / S;
    q.y = (m[1][2] + m[2][1]) / S;
    q.z = 0.25 * S;
  }
  return q;
}

void main()
{
  // Default to outside the frustum so it's discarded if we return early
  gl_Position = vec4(0.0, 0.0, 2.0, 1.0);

  // Early alpha test
  // if (splatOpacity < minAlpha) {
  //   return;
  // }

  // Check for zero scales
  // if (length(splatScale) < 0.001) {
  //   return;
  // }

  vec4 renderToViewQuat = mat3ToQuat(mat3(viewMatrix[0].xyz, viewMatrix[1].xyz, viewMatrix[2].xyz));
  vec3 renderToViewPos = viewMatrix[3].xyz;

  // Transform splat center to world space then view space
  vec3 viewCenter = quatVec(renderToViewQuat, splatPosition) + renderToViewPos;

  // Discard splats behind the camera
  // if (viewCenter.z >= 0.0)
  // {
  //   return;
  // }

  // Compute the clip space center of the splat
  vec4 clipCenter = projectionMatrix * vec4(viewCenter, 1.0);

  // Compute the 3D covariance matrix of the splat in view space
  // Transform quaternion to view space (simplified - assumes model matrix is mostly identity for rotation)
  vec4 viewQuaternion = quatQuat(renderToViewQuat, splatRotation);

  // Compute NDC center of the splat
  vec3 ndcCenter = clipCenter.xyz / clipCenter.w;

  // Compute the 3D covariance matrix of the splat
  mat3 RS = scaleQuaternionToMatrix(splatScale, viewQuaternion);
  mat3 cov3D = RS * transpose(RS);

  // Compute the Jacobian of the splat's projection at its center
  float focalAdjustment = 0.1;
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

  // Add small blur amount for anti-aliasing
  float blurAmount = 0.1;
  a += blurAmount;
  d += blurAmount;
  float det = a * d - b * b;

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
  vec2 ndcOffset = (2.0 / scaledRenderSize) * pixelOffset;
  vec3 ndc = vec3(ndcCenter.xy + ndcOffset, ndcCenter.z);

  // Pass data to fragment shader
  vRgba = vec4(splatColor, splatOpacity);
  vSplatUv = position.xy * maxStdDev;
  vNdc = ndc;
  gl_Position = vec4(ndcCenter.xy * clipCenter.w, clipCenter.zw);

  // vec3 localPos = vec3(position.xy, 0.0) * splatScale;
  // vec3 rotatedPos = quatVec(splatRotation, localPos);
  // rotatedPos += splatPosition;

  // gl_Position = projectionMatrix * viewMatrix * vec4(rotatedPos, 1.0);
  // gl_Position = vec4(position.xy * 0.3 + vec2(0.5, 0.5), 0.0, 1.0);
}
