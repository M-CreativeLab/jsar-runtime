#ifdef USE_INSTANCE_TEXTURE
uniform sampler2DArray instanceTexAltas;
in float vInstanceLayerIndex;
in float vInstanceTextureEnabled;
#endif

#ifdef USE_UVS
in vec2 uvs;
#endif

#ifdef USE_INSTANCE_SDF
in vec2 vInstanceSdfPlaneDimensions;
in vec4 vInstanceSdfBorderRadius;
#else
// SDF rendering uniforms (fallback for non-instanced usage)
uniform vec2 uPlaneDimensions; // Width and height of the plane in logical units
uniform vec4 uBorderRadius;    // Border radius for each corner (top-left, top-right, bottom-right, bottom-left)
#endif

uniform float uSdfAntiAliasWidth; // Width of anti-aliasing zone
uniform float uSdfEnabled;        // Enable/disable SDF rendering (0.0 or 1.0)

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
    mix(r.w, r.z, ux),
    mix(r.x, r.y, ux),
    uy);

  vec2 q = abs(p) - b + radius;
  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

// Convert UV coordinates to plane-centered coordinates
vec2 uvToPlaneCoord(vec2 uv, vec2 dimensions)
{
  return (uv - 0.5) * dimensions;
}

// Smooth step anti-aliasing based on SDF distance
float sdfAntiAlias(float dist, float width)
{
  return 1.0 - smoothstep(-width * 0.5, width * 0.5, dist);
}

void main()
{
  outColor = col;

#ifdef USE_INSTANCE_TEXTURE
  if (vInstanceTextureEnabled == 1.0)
  {
    vec4 textureColor = texture(instanceTexAltas, vec3(uvs, vInstanceLayerIndex));
    outColor = mix(outColor, textureColor, textureColor.a);
  }
#endif

  // Apply SDF-based anti-aliasing if enabled
  if (uSdfEnabled > 0.5)
  {
#ifdef USE_INSTANCE_SDF
    // Use instance data for SDF calculations
    vec2 planeDimensions = vInstanceSdfPlaneDimensions;
    vec4 borderRadius = vInstanceSdfBorderRadius;
#else
    // Fallback to uniforms for non-instanced usage
    vec2 planeDimensions = uPlaneDimensions;
    vec4 borderRadius = uBorderRadius;
#endif

    // Convert UV to plane coordinates centered at origin
    vec2 planeCoord = uvToPlaneCoord(uvs, planeDimensions);

    // Calculate SDF distance for rounded rectangle
    vec2 halfDim = planeDimensions * 0.5;
    float sdfDist = sdfRoundedBox(planeCoord, halfDim, borderRadius);

    // Apply anti-aliasing based on SDF distance
    float alpha = sdfAntiAlias(sdfDist, uSdfAntiAliasWidth);

    // Multiply the final alpha with the SDF alpha for crisp edges
    outColor.a *= alpha;
  }
}
