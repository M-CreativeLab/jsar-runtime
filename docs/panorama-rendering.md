# 360-Degree Panorama Rendering

The `EnvironmentRenderer` in `src/examples/environment_renderer.cpp` now supports loading and rendering 360-degree panoramic photos as immersive environment backgrounds.

## Features

- **Single Image Input**: Load PNG/JPG panorama images with equirectangular projection
- **High Quality**: Converts to 512x512 cube map faces for crisp rendering  
- **Backward Compatible**: Existing cube map directory loading still works
- **Automatic Detection**: Auto-detects whether input is a panorama file or cube map directory
- **Cross-Platform**: Works on macOS with Core Graphics, graceful fallback on other platforms

## Usage

### Loading a Panorama Image

```cpp
#include "environment_renderer.hpp"

EnvironmentRenderer renderer;

// Load a single panorama image
if (renderer.initialize("/path/to/panorama.jpg")) {
    // Success - panorama converted to cube map automatically
}

// OR use the direct API
if (renderer.loadPanoramaImage("/path/to/panorama.jpg")) {
    // Success - panorama loaded and converted
}
```

### Command Line Usage

When using the transmute browser example:

```bash
# Load a panorama image
./transmute_browser --env-map /path/to/panorama.jpg

# Still works with cube map directories  
./transmute_browser --env-map /path/to/cubemap_directory/
```

### Supported Formats

- **Input**: PNG, JPG, JPEG panorama images with equirectangular projection
- **Projection**: Standard equirectangular mapping (2:1 aspect ratio recommended)
- **Output**: Converted to OpenGL cube map for rendering

## Technical Details

### Conversion Process

1. Load panorama image using platform image loader (Core Graphics on macOS)
2. Convert equirectangular projection to 6 cube map faces
3. Sample panorama using spherical coordinate mapping
4. Generate 512x512 textures for each cube face
5. Use existing cube map rendering pipeline

### Mathematical Approach

For each cube face pixel:
1. Convert cube face (u,v) coordinates to 3D direction vector
2. Transform 3D direction to spherical coordinates (theta, phi)  
3. Map spherical coordinates to equirectangular texture coordinates
4. Sample panorama image and copy to cube face

### Performance

- **Memory**: ~6MB for 512x512x6 RGB cube faces
- **Conversion**: One-time cost during loading
- **Rendering**: Same performance as existing cube map rendering

## Implementation Files

- `src/examples/environment_renderer.hpp` - Added `loadPanoramaImage()` method
- `src/examples/environment_renderer.cpp` - Core conversion implementation
  - `loadPanoramaImage()` - Main panorama loading function
  - `convertEquirectangularToCubeMap()` - Mathematical conversion logic
  - Extended `loadCubeMap()` - Auto-detection of input type

## Error Handling

- **Invalid Files**: Falls back to procedural environment
- **Unsupported Platforms**: Graceful fallback with warning message  
- **OpenGL Errors**: Detailed error reporting during cube map creation
- **Memory Issues**: Bounds checking for all array accesses

## Example Images

Panorama images should use equirectangular projection with:
- **Aspect Ratio**: 2:1 (width:height)
- **Horizontal Coverage**: Full 360° longitude (-180° to +180°)
- **Vertical Coverage**: Full 180° latitude (-90° to +90°)
- **Format**: Standard PNG or JPEG

Popular sources include 360° cameras, panorama photography, and online panorama libraries.