#ifdef USE_INSTANCE_TEXTURE
uniform sampler2DArray instanceTexAltas;
in float vInstanceLayerIndex;
in float vInstanceTextureEnabled;
#endif

#ifdef USE_UVS
in vec2 uvs;
#endif

#ifdef USE_INSTANCE_SDF
in vec2 vSdfPlaneTexCoord;
in vec2 vInstanceSdfPlaneDimensions;
in vec4 vInstanceSdfBorderRadius;
#else
// SDF rendering uniforms (fallback for non-instanced usage)
uniform vec2 uPlaneDimensions; // Width and height of the plane in logical units
uniform vec4 uBorderRadius;    // Border radius for each corner (top-left, top-right, bottom-right, bottom-left)
#endif

uniform float uSdfEnabled; // Enable/disable SDF rendering (0.0 or 1.0)

in vec4 col;
layout(location = 0) out vec4 outColor;

// SDF function for a rounded rectangle
// point: point coordinate (relative to center)
// halfSize: half-dimensions of the rectangle
// r: corner radius
float sdfRoundedBox(vec2 point, vec2 halfSize, vec4 r)
{
  // Select the appropriate corner radius based on quadrant
  float ux = step(0.0, point.x);
  float uy = step(0.0, point.y);
  float radius = mix(
    mix(r.x, r.y, ux),
    mix(r.w, r.z, ux),
    uy);

  vec2 q = abs(point) - halfSize + vec2(radius);
  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

// Convert UV coordinates to plane-centered coordinates
vec2 uvToPlaneCoord(vec2 uv, vec2 dimensions)
{
  return (uv - 0.5) * dimensions;
}

// Smooth step anti-aliasing based on SDF distance
float sdfAntiAlias(float dist)
{
  float width = max(fwidth(dist), 0.01);
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

    // Use original texture coordinates for SDF calculations (not the transformed uvs used for atlas sampling)
    vec2 planeCoord = uvToPlaneCoord(vSdfPlaneTexCoord, planeDimensions);

    // Calculate SDF distance for rounded rectangle
    float sdfDist = sdfRoundedBox(planeCoord, planeDimensions * 0.5, borderRadius);

    // Apply anti-aliasing based on SDF distance
    float alpha = sdfAntiAlias(sdfDist);

    // Multiply the final alpha with the SDF alpha for crisp edges
    outColor.a *= alpha;
  }
}
