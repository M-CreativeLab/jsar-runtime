#ifdef USE_INSTANCE_TEXTURE
uniform sampler2DArray instanceTexAltas;
in float vInstanceLayerIndex;
in float vInstanceTextureEnabled;
in float vInstanceUseSDFTexture;
#endif

#ifdef USE_UVS
in vec2 uvs;
#endif

#ifdef USE_INSTANCE_SDF
in vec2 vInstanceTexCoord;
in vec2 vInstanceDimensions;
in vec4 vInstanceBorderRadius;
in float vInstanceBorderStyle;
flat in int vInstanceId;
in float vSdfDepthScale;

// Texture-based border data storage
uniform sampler2D borderDataTexture;

// Texture-based scroll shadow data storage
uniform sampler2D scrollShadowDataTexture;
#else
// SDF rendering uniforms (fallback for non-instanced usage)
uniform vec2 uDimensions;   // Width and height of the plane in logical units
uniform vec4 uBorderRadius; // Border radius for each corner (top-left, top-right, bottom-right, bottom-left)
uniform vec4 uBorderWidth;  // Border width for each side (top, right, bottom, left)
uniform vec4 uBorderColor;  // Border color (RGBA)
uniform float uBorderStyle; // Border style (0=none, 1=solid, 2=dashed)
#endif

uniform float uSdfEnabled; // Enable/disable SDF rendering (0.0 or 1.0)

in vec4 col;
layout(location = 0) out vec4 outColor;

// SDF function for a rounded rectangle
// p: point coordinate (relative to center)
// b: half-dimensions of the rectangle
// r: corner radius
float sdfRoundedBox(vec2 p, vec2 b, vec4 r)
{
  // Select the appropriate corner radius based on quadrant
  float ux = step(0.0, p.x);
  float uy = step(0.0, p.y);
  float radius = mix(
    mix(r.w, r.z, ux), // When uy=0 (bottom): mix between r.w (bottom-left) and r.z (bottom-right)
    mix(r.x, r.y, ux), // When uy=1 (top): mix between r.x (top-left) and r.y (top-right)
    uy);

  vec2 q = abs(p) - b + vec2(radius);
  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

// Convert UV coordinates to plane-centered coordinates
vec2 uvToPlaneCoord(vec2 uv, vec2 dimensions)
{
  return (uv - 0.5) * dimensions;
}

// Read border data from texture
// Texture layout: each row is one instance, columns are: [width, topColor, rightColor, bottomColor, leftColor]
vec4 getBorderWidth(int instanceId)
{
  return texelFetch(borderDataTexture, ivec2(0, instanceId), 0);
}

vec4 getBorderTopColor(int instanceId)
{
  return texelFetch(borderDataTexture, ivec2(1, instanceId), 0);
}

vec4 getBorderRightColor(int instanceId)
{
  return texelFetch(borderDataTexture, ivec2(2, instanceId), 0);
}

vec4 getBorderBottomColor(int instanceId)
{
  return texelFetch(borderDataTexture, ivec2(3, instanceId), 0);
}

vec4 getBorderLeftColor(int instanceId)
{
  return texelFetch(borderDataTexture, ivec2(4, instanceId), 0);
}

// Get scroll shadow color from texture
vec4 getScrollShadowColor(int instanceId)
{
  return texelFetch(scrollShadowDataTexture, ivec2(0, instanceId), 0);
}

// Get scroll shadow parameters from texture
vec4 getScrollShadowParams(int instanceId)
{
  return texelFetch(scrollShadowDataTexture, ivec2(1, instanceId), 0);
}

// Get content size from texture 
vec4 getScrollShadowContentSize(int instanceId)
{
  return texelFetch(scrollShadowDataTexture, ivec2(2, instanceId), 0);
}

// Smooth step anti-aliasing based on SDF distance
float sdfAntiAlias(float dist, float depthScale)
{
  float width = max(fwidth(dist), 0.01) * depthScale;
  // TODO(yorkie): use gradient-based width for better quality on high-end devices?
  return 1.0 - smoothstep(-width * 0.5, width * 0.5, dist);
}

// Calculate border region using SDF
// Returns SDF distance: negative inside border, positive outside
float sdfBorder(vec2 p, vec2 dimensions, vec4 borderRadius, vec4 borderWidth)
{
  // Calculate outer and inner border bounds
  vec2 outerDimensions = dimensions;
  vec2 innerDimensions = dimensions - vec2(borderWidth.y + borderWidth.w, borderWidth.x + borderWidth.z);

  // Calculate outer and inner border radius
  vec4 outerRadius = borderRadius;
  vec4 innerRadius = max(vec4(0.0), borderRadius - vec4(max(borderWidth.x, borderWidth.w), // top-left: max(top, left)
                                                        max(borderWidth.x, borderWidth.y), // top-right: max(top, right)
                                                        max(borderWidth.z, borderWidth.y), // bottom-right: max(bottom, right)
                                                        max(borderWidth.z, borderWidth.w)  // bottom-left: max(bottom, left)
                                                        ));

  // Calculate distances to outer and inner edges
  float outerDist = sdfRoundedBox(p, outerDimensions * 0.5, outerRadius);
  float innerDist = sdfRoundedBox(p, innerDimensions * 0.5, innerRadius);

  // Border SDF: negative inside border region, positive outside
  // Uses SDF subtraction: max(outer, -inner)
  return max(outerDist, -innerDist);
}

// Generate dashed pattern based on position
float dashPattern(vec2 p, vec2 dimensions, float dashLength)
{
  // Calculate distance along the perimeter
  vec2 absP = abs(p);
  vec2 halfDim = dimensions * 0.5;

  // Determine which edge we're closest to and calculate distance along that edge
  float dist = 0.0;
  if (absP.x > absP.y * (halfDim.x / halfDim.y))
  {
    // Horizontal edges (top/bottom)
    dist = absP.x + sign(p.y) * halfDim.y;
  }
  else
  {
    // Vertical edges (left/right)
    dist = absP.y + sign(p.x) * halfDim.x;
  }

  // Create dash pattern
  float dashCycle = mod(dist, dashLength * 2.0);
  return step(dashCycle, dashLength);
}
// Calculate scroll shadow alpha based on scroll position and edge proximity
// p: point coordinate (relative to center)
// dimensions: element dimensions (width, height)
// scrollOffset: current scroll offset (x, y)
// contentSize: content size (width, height)
// maxHeight: maximum shadow height as proportion of element size
vec4 calculateScrollShadows(vec2 p, vec2 dimensions, vec2 scrollOffset, vec2 contentSize, float maxHeight)
{
  float maxShadowWidth = dimensions.x * maxHeight;
  float maxShadowHeight = dimensions.y * maxHeight;

  vec2 maxScroll = max(vec2(0.0), contentSize - dimensions);
  vec2 halfDim   = dimensions * 0.5;

  // Fade shadows towards center to avoid harsh edges
  float centerFadeX = clamp(1.0 - abs(p.x) / halfDim.x, 0.0, 1.0);
  float centerFadeY = clamp(1.0 - abs(p.y) / halfDim.y, 0.0, 1.0);

  vec4 shadowAlpha = vec4(0.0);

  // Top shadow
  if (scrollOffset.y > 0.0)
  {
    float scrollProgress = clamp(scrollOffset.y / maxScroll.y, 0.0, 1.0);
    float distToEdge = halfDim.y - p.y;
    float fade = 1.0 - clamp(distToEdge / maxShadowHeight, 0.0, 1.0);
    shadowAlpha.x = scrollProgress * fade * centerFadeX;
  }

  // Right shadow
  if (scrollOffset.x > 0.0)
  {
    float scrollProgress = clamp(scrollOffset.x / maxScroll.x, 0.0, 1.0);
    float distToEdge = halfDim.x - p.x;
    float fade = 1.0 - clamp(distToEdge / maxShadowWidth, 0.0, 1.0);
    shadowAlpha.y = scrollProgress * fade * centerFadeY;
  }

  // Bottom shadow
  if (scrollOffset.y < maxScroll.y)
  {
    float remainingScroll = (maxScroll.y - scrollOffset.y) / maxScroll.y;
    float distToEdge = halfDim.y + p.y;
    float fade = 1.0 - clamp(distToEdge / maxShadowHeight, 0.0, 1.0);
    shadowAlpha.z = remainingScroll * fade * centerFadeX;
  }

  // Left shadow
  if (scrollOffset.x < maxScroll.x)
  {
    float remainingScroll = (maxScroll.x - scrollOffset.x) / maxScroll.x;
    float distToEdge = halfDim.x + p.x;
    float fade = 1.0 - clamp(distToEdge / maxShadowWidth, 0.0, 1.0);
    shadowAlpha.w = remainingScroll * fade * centerFadeY;
  }
  return shadowAlpha;
}


void main()
{
  outColor = col;

#ifdef USE_INSTANCE_TEXTURE
  if (vInstanceTextureEnabled == 1.0)
  {
    vec4 textureColor = texture(instanceTexAltas, vec3(uvs, vInstanceLayerIndex));

    // Check if this instance uses SDF texture rendering
    if (vInstanceUseSDFTexture > 0.5)
    {
      // For SDF text or other texture, the `textureColor` contains distance field data in the alpha channel
      // Apply SDF distance field rendering for smooth text anti-aliasing
      float texDist = textureColor.a - 0.5; // Convert from [0,1] to [-0.5,0.5] range
      float sdfWidth = max(fwidth(texDist), 0.01);
      float sdfAlpha = smoothstep(-sdfWidth * 0.5, sdfWidth * 0.5, texDist);

      // Use black color with SDF-computed alpha for crisp text edges
      textureColor.a *= sdfAlpha;
    }
    outColor = mix(outColor, textureColor, textureColor.a);
  }
#endif

  // Apply SDF-based anti-aliasing if enabled
  if (uSdfEnabled > 0.5)
  {
#ifdef USE_INSTANCE_SDF
    // Use instance data for SDF calculations
    vec2 dimensions = vInstanceDimensions;
    vec4 borderRadius = vInstanceBorderRadius;
    vec4 borderWidth = getBorderWidth(vInstanceId);
    // TODO: Support different colors per border side - for now using top border color for all sides
    vec4 borderColor = getBorderTopColor(vInstanceId);
    float borderStyle = vInstanceBorderStyle;
#else
    // Fallback to uniforms for non-instanced usage
    vec2 dimensions = uDimensions;
    vec4 borderRadius = uBorderRadius;
    vec4 borderWidth = uBorderWidth;
    vec4 borderColor = uBorderColor;
    float borderStyle = uBorderStyle;
#endif

    // Use instance texture coordinates for SDF calculations (not the transformed uvs used for atlas sampling)
    vec2 planeCoord = uvToPlaneCoord(vInstanceTexCoord, dimensions);

    // Calculate SDF distance for rounded rectangle (for content area clipping)
    float sdfDist = sdfRoundedBox(planeCoord, dimensions * 0.5, borderRadius);
    float contentAlpha = sdfAntiAlias(sdfDist, vSdfDepthScale);

    // Apply content alpha for crisp edges
    outColor.a *= contentAlpha;

    // Apply border rendering if border is enabled (borderStyle > 0)
    if (borderStyle > 0.5 && borderColor.a > 0.0)
    {
      // Calculate border SDF distance
      float borderDist = sdfBorder(planeCoord, dimensions, borderRadius, borderWidth);

      // Apply SDF anti-aliasing to border
      float borderAlpha = sdfAntiAlias(borderDist, vSdfDepthScale);

      // Apply border style
      if (borderStyle > 1.0) // Dashed style
      {
        float dashLength = max(borderWidth.x, max(borderWidth.y, max(borderWidth.z, borderWidth.w))) * 3.0;
        float dashMask = dashPattern(planeCoord, dimensions, dashLength);
        borderAlpha *= dashMask;
      }

      // Blend border with existing color
      if (borderAlpha > 0.0)
      {
        vec3 finalColor = mix(outColor.rgb, borderColor.rgb, borderAlpha * borderColor.a);
        float finalAlpha = max(outColor.a, borderAlpha * borderColor.a);
        outColor = vec4(finalColor, finalAlpha);
      }
    }

    // Apply scroll shadows if enabled and color has alpha
#ifdef USE_INSTANCE_SDF
    vec4 shadowColor = getScrollShadowColor(vInstanceId);
    if (shadowColor.a > 0.0)
    {
      vec4 shadowParams = getScrollShadowParams(vInstanceId);
      vec4 contentSizeData = getScrollShadowContentSize(vInstanceId);

      float shadowMaxHeight = shadowParams.x;
      vec2 scrollOffset = shadowParams.yz;
      vec2 contentSize = vec2(shadowParams.w, contentSizeData.x);

      vec4 shadowAlphas = calculateScrollShadows(planeCoord, dimensions, scrollOffset, contentSize, shadowMaxHeight);

      // Combine all shadow alphas (max of all edges)
      float totalShadowAlpha = max(max(shadowAlphas.x, shadowAlphas.y), max(shadowAlphas.z, shadowAlphas.w));
      if (totalShadowAlpha > 0.0)
      {
        // Apply shadow with SDF anti-aliasing
        vec3 finalShadowColor = mix(outColor.rgb, shadowColor.rgb, totalShadowAlpha * shadowColor.a);
        float shadowFinalAlpha = max(outColor.a, totalShadowAlpha * shadowColor.a);
        outColor = vec4(finalShadowColor, shadowFinalAlpha);
      }
    }
#endif
  }
}
