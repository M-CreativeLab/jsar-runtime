precision mediump float;
precision mediump int;

// Uniforms
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec2 renderSize;
uniform float maxStdDev;
uniform float minAlpha;
uniform float maxPixelRadius;
uniform float clipXY;
uniform float focalAdjustment;
uniform float maxDistance;

// Separate textures for splat data (no pack/unpack needed)
uniform sampler2D splatCenters;  // RGB for XYZ position
uniform sampler2D splatColors;   // RGBA for splat color and opacity  
uniform sampler2D splatScales;   // RGB for XYZ scale values
uniform sampler2D splatQuat;     // RGBA for rotation quaternion

// Texture size constants (power of 2)
const int TEXTURE_WIDTH_BITS = 10;  // 1024 width
const int TEXTURE_WIDTH = 1 << TEXTURE_WIDTH_BITS;
const int TEXTURE_WIDTH_MASK = TEXTURE_WIDTH - 1;

// Vertex attributes
in vec3 position;        // quad position (-1 to 1)
in uint splatIndex;      // splat index (sorted)

// Outputs
out vec4 vRgba;
out vec2 vSplatUv;

// Efficient texture coordinate calculation using bit operations
ivec2 getSplatTexCoord(int index)
{
  int x = index & TEXTURE_WIDTH_MASK;  // x = index & (1024 - 1)
  int y = index >> TEXTURE_WIDTH_BITS; // y = index >> 10
  return ivec2(x, y);
}

// SparkJS quaternion functions (unchanged)
vec3 quatVec(vec4 q, vec3 v)
{
  vec3 t = 2.0 * cross(q.xyz, v);
  return v + q.w * t + cross(q.xyz, t);
}

vec4 quatQuat(vec4 q1, vec4 q2)
{
  return vec4(
    q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
    q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
    q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
  );
}

mat3 scaleQuaternionToMat3(vec3 s, vec4 q)
{
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

vec4 mat3ToQuat(mat3 m)
{
  float trace = m[0][0] + m[1][1] + m[2][2];
  vec4 q;

  if (trace > 0.0)
  {
    float s = sqrt(trace + 1.0) * 2.0;
    q.w = 0.25 * s;
    q.x = (m[2][1] - m[1][2]) / s;
    q.y = (m[0][2] - m[2][0]) / s;
    q.z = (m[1][0] - m[0][1]) / s;
  }
  else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
  {
    float s = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2.0;
    q.w = (m[2][1] - m[1][2]) / s;
    q.x = 0.25 * s;
    q.y = (m[0][1] + m[1][0]) / s;
    q.z = (m[0][2] + m[2][0]) / s;
  }
  else if (m[1][1] > m[2][2])
  {
    float s = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2.0;
    q.w = (m[0][2] - m[2][0]) / s;
    q.x = (m[0][1] + m[1][0]) / s;
    q.y = 0.25 * s;
    q.z = (m[1][2] + m[2][1]) / s;
  }
  else
  {
    float s = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2.0;
    q.w = (m[1][0] - m[0][1]) / s;
    q.x = (m[0][2] + m[2][0]) / s;
    q.y = (m[1][2] + m[2][1]) / s;
    q.z = 0.25 * s;
  }
  return normalize(q);
}

mat3 computeCov3D(mat4 viewMatrix, mat4 modelMatrix, vec4 splatQuat, vec3 splatScale)
{
  mat3 RS = scaleQuaternionToMat3(splatScale, splatQuat);
  mat3 VM = mat3(viewMatrix * modelMatrix);
  mat3 T = VM * RS;
  return T * transpose(T);
}

void main()
{
  // Default to outside the frustum so it's discarded if we return early
  gl_Position = vec4(0.0, 0.0, 2.0, 1.0);

  // Handle special value for "no splat"
  if (splatIndex == 0xffffffffu)
  {
    return;
  }

  // Get texture coordinate using efficient bit operations
  ivec2 texCoord = getSplatTexCoord(int(splatIndex));

  // Direct texture fetches - no pack/unpack needed!
  vec4 rgba = texelFetch(splatColors, texCoord, 0);
  vec3 scales = texelFetch(splatScales, texCoord, 0).xyz;

  // Early alpha test
  if (rgba.a < minAlpha)
  {
    return;
  }

  // Check for zero scales
  bvec3 zeroScales = equal(scales, vec3(0.0));
  if (all(zeroScales))
  {
    return;
  }

  vRgba = rgba;
  vSplatUv = position.xy * maxStdDev;

  // Calculate the viewModel matrix
  mat4 viewModelMatrix = modelMatrix * viewMatrix;

  // Transform splat center to world space then view space
  vec3 center = texelFetch(splatCenters, texCoord, 0).rgb;
  vec4 viewCenter4 = viewModelMatrix * vec4(center, 1.0);
  vec3 viewCenter = viewCenter4.xyz;

  // Discard splats that are behind the camera or too far away
  if (viewCenter.z >= 0.0 || length(viewCenter) > maxDistance)
  {
    return;
  }

  vec4 clipCenter = projectionMatrix * viewCenter4;
  // TODO(yorkie): handle multiview

  // Discard splats outside near/far planes
  if (abs(clipCenter.z) >= clipCenter.w)
  {
    return;
  }

  float clip = clipXY * clipCenter.w;
  if (abs(clipCenter.x) > clip || abs(clipCenter.y) > clip)
  {
    return;
  }

  // Compute the 3D covariance matrix for the splat
  vec4 quaternion = texelFetch(splatQuat, texCoord, 0);
  mat3 cov3D = computeCov3D(viewMatrix, modelMatrix, quaternion, scales);

  // Compute the Jacobian of the splat's projection at its center
  vec2 scaledRenderSize = renderSize * focalAdjustment;
  vec2 focal = 0.5 * scaledRenderSize * vec2(projectionMatrix[0][0], projectionMatrix[1][1]);
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
  if (vRgba.a < minAlpha)
  {
    return;
  }

  // Compute the eigenvalue and eigenvectors of the 2D covariance matrix (Spark method)
  float eigenAvg = 0.5 * (a + d);
  float eigenDelta = sqrt(max(0.0, eigenAvg * eigenAvg - det));
  float eigen1 = eigenAvg + eigenDelta;
  float eigen2 = eigenAvg - eigenDelta;

  vec2 eigenVec1 = normalize(vec2((abs(b) < 0.001) ? 1.0 : b, eigen1 - a));
  vec2 eigenVec2 = vec2(-eigenVec1.y, eigenVec1.x);

  float scale1 = position.x * min(maxPixelRadius, maxStdDev * sqrt(eigen1));
  float scale2 = position.y * min(maxPixelRadius, maxStdDev * sqrt(eigen2));

  // Compute the NDC coordinates for the ellipsoid's diagonal axes.
  vec2 pixelOffset = eigenVec1 * scale1 + eigenVec2 * scale2;

  // Compute the NDC coordinates for the ellipsoid's diagonal axes.
  vec2 ndcOffset = (2.0 / scaledRenderSize) * pixelOffset;
  vec3 ndcCenter = clipCenter.xyz / clipCenter.w;
  vec3 ndc = vec3(ndcCenter.xy + ndcOffset, ndcCenter.z);
  gl_Position = vec4(ndc.xy * clipCenter.w, clipCenter.zw);
}
