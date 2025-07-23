# Spatial Images in JSAR

JSAR supports spatial (stereo) images that can provide different content for the left and right eyes in immersive viewing modes. This is useful for creating 3D stereoscopic content using standard HTML `<img>` elements.

## Usage

To mark an image as spatial, add the `spatial="stereo"` attribute to an `<img>` element:

```html
<img src="stereo-image.jpg" spatial="stereo" style="width: 300px; height: 150px;" />
```

## Image Format

Spatial images should be formatted as side-by-side stereo pairs:
- **Left half**: Content for the left eye
- **Right half**: Content for the right eye
- The image should have a 2:1 aspect ratio (e.g., 400x200 pixels)

When displayed in stereo mode:
- Left eye sees the left half of the image (first 50% horizontally)
- Right eye sees the right half of the image (second 50% horizontally)

When displayed in non-stereo mode:
- The full image is displayed normally

## Example

```html
<!DOCTYPE html>
<html>
<head>
    <title>Spatial Images Demo</title>
</head>
<body>
    <h1>Mixed Content Example</h1>
    
    <!-- Regular image -->
    <img src="regular-photo.jpg" style="width: 300px; height: 200px;" />
    
    <!-- Spatial stereo image -->
    <img src="stereo-landscape.jpg" spatial="stereo" style="width: 300px; height: 150px;" />
    
    <!-- Multiple spatial images -->
    <div class="gallery">
        <img src="stereo-1.jpg" spatial="stereo" />
        <img src="stereo-2.jpg" spatial="stereo" />
        <img src="stereo-3.jpg" spatial="stereo" />
    </div>
</body>
</html>
```

## JavaScript API

You can also create and manipulate spatial images via JavaScript:

```javascript
// Create a spatial image element
const spatialImg = document.createElement('img');
spatialImg.src = 'my-stereo-image.jpg';
spatialImg.setAttribute('spatial', 'stereo');
document.body.appendChild(spatialImg);

// Using the Image constructor
const img = new Image();
img.onload = function() {
    this.setAttribute('spatial', 'stereo');
    document.body.appendChild(this);
};
img.src = 'another-stereo.jpg';

// Check if an image is spatial
if (img.getAttribute('spatial') === 'stereo') {
    console.log('This is a spatial image');
}
```

## Best Practices

1. **Image Dimensions**: Use 2:1 aspect ratio images for best results
2. **Content Alignment**: Ensure left and right eye content is properly aligned vertically
3. **Fallback**: Spatial images work gracefully in non-stereo environments (full image is shown)
4. **File Size**: Consider that spatial images contain two views, so optimize accordingly
5. **Testing**: Test both in stereo and non-stereo modes to ensure good user experience

## Supported Formats

Spatial images work with all image formats supported by JSAR:
- JPEG
- PNG
- WebP
- Any format supported by the underlying Skia rendering engine

## Browser Compatibility

The `spatial` attribute is a JSAR-specific extension and will be ignored by regular web browsers. This ensures forward compatibility - your spatial images will display as regular images in standard browsers.