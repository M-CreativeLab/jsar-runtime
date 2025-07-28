#ifdef USE_INSTANCE_TEXTURE
uniform sampler2DArray instanceTexAltas;
in float vInstanceLayerIndex;
in float vInstanceTextureEnabled;
#endif

#ifdef USE_UVS
in vec2 uvs;
#endif

// SDF rendering uniforms
uniform vec2 uPlaneDimensions;     // Width and height of the plane in logical units
uniform vec4 uBorderRadius;       // Border radius for each corner (top-left, top-right, bottom-right, bottom-left)
uniform float uSdfAntiAliasWidth; // Width of anti-aliasing zone
uniform float uSdfEnabled;        // Enable/disable SDF rendering (0.0 or 1.0)

in vec4 col;
layout(location = 0) out vec4 outColor;

// SDF function for a rounded rectangle
// p: point coordinate (relative to center)
// b: half-dimensions of the rectangle
// r: corner radius
float sdfRoundedBox(vec2 p, vec2 b, vec4 r) {
    // Select the appropriate corner radius based on quadrant
    vec2 corner = step(0.0, p) * vec2(1.0, 1.0);
    float radius = mix(
        mix(r.x, r.y, corner.x),  // top-left or top-right
        mix(r.w, r.z, corner.x),  // bottom-left or bottom-right
        corner.y
    );
    
    vec2 q = abs(p) - b + radius;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

// Convert UV coordinates to plane-centered coordinates
vec2 uvToPlaneCoord(vec2 uv, vec2 dimensions) {
    return (uv - 0.5) * dimensions;
}

// Smooth step anti-aliasing based on SDF distance
float sdfAntiAlias(float dist, float width) {
    return 1.0 - smoothstep(-width * 0.5, width * 0.5, dist);
}

void main() {
    outColor = col;

#ifdef USE_INSTANCE_TEXTURE
    if (vInstanceTextureEnabled == 1.0) {
        vec4 textureColor = texture(instanceTexAltas, vec3(uvs, vInstanceLayerIndex));
        outColor = mix(outColor, textureColor, textureColor.a);
    }
#endif

    // Apply SDF-based anti-aliasing if enabled
    if (uSdfEnabled > 0.5) {
        // Convert UV to plane coordinates centered at origin
        vec2 planeCoord = uvToPlaneCoord(uvs, uPlaneDimensions);
        
        // Calculate SDF distance for rounded rectangle
        vec2 halfDim = uPlaneDimensions * 0.5;
        float sdfDist = sdfRoundedBox(planeCoord, halfDim, uBorderRadius);
        
        // Apply anti-aliasing based on SDF distance
        float alpha = sdfAntiAlias(sdfDist, uSdfAntiAliasWidth);
        
        // Multiply the final alpha with the SDF alpha for crisp edges
        outColor.a *= alpha;
    }
}
