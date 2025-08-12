precision highp float;
precision highp int;
precision highp usampler2DArray;

// SparkJS-compatible constants and defines
const float LN_SCALE_MIN = -12.0;
const float LN_SCALE_MAX = 9.0;

const uint SPLAT_TEX_WIDTH_BITS = 11u;
const uint SPLAT_TEX_HEIGHT_BITS = 11u;
const uint SPLAT_TEX_DEPTH_BITS = 11u;
const uint SPLAT_TEX_LAYER_BITS = SPLAT_TEX_WIDTH_BITS + SPLAT_TEX_HEIGHT_BITS;

const uint SPLAT_TEX_WIDTH = 1u << SPLAT_TEX_WIDTH_BITS;
const uint SPLAT_TEX_HEIGHT = 1u << SPLAT_TEX_HEIGHT_BITS;
const uint SPLAT_TEX_DEPTH = 1u << SPLAT_TEX_DEPTH_BITS;

const uint SPLAT_TEX_WIDTH_MASK = SPLAT_TEX_WIDTH - 1u;
const uint SPLAT_TEX_HEIGHT_MASK = SPLAT_TEX_HEIGHT - 1u;
const uint SPLAT_TEX_DEPTH_MASK = SPLAT_TEX_DEPTH - 1u;

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

uniform usampler2DArray packedSplats;
uniform vec4 rgbMinMaxLnScaleMinMax;

// Vertex attributes
attribute vec3 position;        // quad position (-1 to 1)
attribute uint splatIndex;      // splat index (sorted)

// Outputs
out vec4 vRgba;
out vec2 vSplatUv;
out vec3 vNdc;

// SparkJS texture coordinate calculation
ivec3 splatTexCoord(int index) {
    uint x = uint(index) & SPLAT_TEX_WIDTH_MASK;
    uint y = (uint(index) >> SPLAT_TEX_WIDTH_BITS) & SPLAT_TEX_HEIGHT_MASK;
    uint z = uint(index) >> SPLAT_TEX_LAYER_BITS;
    return ivec3(x, y, z);
}

// Decode quaternion from SparkJS octahedral encoding
vec4 decodeQuatOctXy88R8(uint encoded) {
    // Extract the fields
    uint quantU = encoded & 0xFFu;               // bits 0–7
    uint quantV = (encoded >> 8u) & 0xFFu;      // bits 8–15
    uint angleInt = encoded >> 16u;              // bits 16–23

    // Recover u and v in [0,1], then map to [-1,1]
    float u_f = float(quantU) / 255.0;
    float v_f = float(quantV) / 255.0;
    vec2 f = vec2(u_f * 2.0 - 1.0, v_f * 2.0 - 1.0);

    vec3 axis = vec3(f.xy, 1.0 - abs(f.x) - abs(f.y));
    float t = max(-axis.z, 0.0);
    axis.x += (axis.x >= 0.0) ? -t : t;
    axis.y += (axis.y >= 0.0) ? -t : t;
    axis = normalize(axis);
    
    // Decode the angle θ ∈ [0,π]
    float theta = (float(angleInt) / 255.0) * 3.14159265359;
    float halfTheta = theta * 0.5;
    float s = sin(halfTheta);
    float w = cos(halfTheta);
    
    return vec4(axis * s, w);
}

// Unpack splat data from uvec4 (SparkJS method)
void unpackSplatEncoding(uvec4 packed, out vec3 center, out vec3 scales, out vec4 quaternion, out vec4 rgba, vec4 rgbMinMaxLnScaleMinMax) {
    uint word0 = packed.x, word1 = packed.y, word2 = packed.z, word3 = packed.w;

    // Unpack RGBA
    uvec4 uRgba = uvec4(word0 & 0xffu, (word0 >> 8u) & 0xffu, (word0 >> 16u) & 0xffu, (word0 >> 24u) & 0xffu);
    float rgbMin = rgbMinMaxLnScaleMinMax.x;
    float rgbMax = rgbMinMaxLnScaleMinMax.y;
    rgba = (vec4(uRgba) / 255.0);
    rgba.rgb = rgba.rgb * (rgbMax - rgbMin) + rgbMin;

    // Unpack center position
    center = vec4(
        unpackHalf2x16(word1),
        unpackHalf2x16(word2 & 0xffffu)
    ).xyz;

    // Unpack scales
    uvec3 uScales = uvec3(word3 & 0xffu, (word3 >> 8u) & 0xffu, (word3 >> 16u) & 0xffu);
    float lnScaleMin = rgbMinMaxLnScaleMinMax.z;
    float lnScaleMax = rgbMinMaxLnScaleMinMax.w;
    float lnScaleScale = (lnScaleMax - lnScaleMin) / 254.0;
    scales = vec3(
        (uScales.x == 0u) ? 0.0 : exp(lnScaleMin + float(uScales.x - 1u) * lnScaleScale),
        (uScales.y == 0u) ? 0.0 : exp(lnScaleMin + float(uScales.y - 1u) * lnScaleScale),
        (uScales.z == 0u) ? 0.0 : exp(lnScaleMin + float(uScales.z - 1u) * lnScaleScale)
    );

    // Unpack quaternion
    uint uQuat = ((word2 >> 16u) & 0xFFFFu) | ((word3 >> 8u) & 0xFF0000u);
    quaternion = decodeQuatOctXy88R8(uQuat);
}

// SparkJS quaternion functions
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

mat3 scaleQuaternionToMatrix(vec3 s, vec4 q) {
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

void decomposeViewMatrix(
    mat4 viewMatrix,
    out vec3 viewPosition,
    out vec4 viewQuaternion
) {
    mat3 viewRotMatrix = mat3(viewMatrix);
    mat3 worldRotMatrix = transpose(viewRotMatrix);
    float det = determinant(viewRotMatrix);

    if (det > 0.0) {
        viewQuaternion = mat3ToQuat(worldRotMatrix);
    } else {
        worldRotMatrix[2] = -worldRotMatrix[2];
        viewQuaternion = mat3ToQuat(worldRotMatrix);
    }

    viewPosition = viewMatrix[3].xyz;
}

void main() {
    // Default to outside the frustum so it's discarded if we return early
    gl_Position = vec4(0.0, 0.0, 2.0, 1.0);

    // Handle special value for "no splat"
    if (splatIndex == 0xffffffffu) {
        return;
    }

    // Get texture coordinate using SparkJS method
    ivec3 texCoord = splatTexCoord(int(splatIndex));
    uvec4 packed = texelFetch(packedSplats, texCoord, 0);

    // Unpack splat data using SparkJS method
    vec3 center, scales;
    vec4 quaternion, rgba;
    unpackSplatEncoding(packed, center, scales, quaternion, rgba, rgbMinMaxLnScaleMinMax);

    vRgba = rgba;

    // Early alpha test
    if (rgba.a < minAlpha) {
        return;
    }

    // Check for zero scales
    bvec3 zeroScales = equal(scales, vec3(0.0));
    if (all(zeroScales)) {
        return;
    }

    vec3 renderToViewPos;
    vec4 renderToViewQuat;
    decomposeViewMatrix(viewMatrix, renderToViewPos, renderToViewQuat);

    // Transform splat center to world space then view space
    vec3 worldCenter = (modelMatrix * vec4(center, 1.0)).xyz;
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
    vec3 transformedScale = scales * modelScale;
    mat3 modelRotation = mat3(
        modelRotationScale[0] / modelScale.x,
        modelRotationScale[1] / modelScale.y,
        modelRotationScale[2] / modelScale.z
    );
    vec4 modelQuat = mat3ToQuat(modelRotation);
    vec4 transformedRotation = quatQuat(modelQuat, quaternion);

    vec4 viewQuaternion = quatQuat(renderToViewQuat, transformedRotation);
    mat3 RS = scaleQuaternionToMatrix(transformedScale, viewQuaternion);
    mat3 cov3D = RS * transpose(RS);

    // Compute the Jacobian of the splat's projection at its center
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

    // Pass data to fragment shader
    vSplatUv = position.xy * maxStdDev;
    vNdc = ndc;
    gl_Position = vec4(ndc.xy * clipCenter.w, clipCenter.zw);
}