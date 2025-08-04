# Creating Spatial Images

This tutorial will guide you through creating and displaying spatial (stereo) images in JSAR, allowing you to create immersive 3D visual content using standard HTML.

## Step 1: Prepare Your Stereo Image

First, you need to prepare a side-by-side stereo image. This should be a single image file containing two views:

- Left half: Content for the left eye
- Right half: Content for the right eye

The image should have a 2:1 aspect ratio (e.g., 800x400 pixels).

For this tutorial, let's assume you have a stereo image named `landscape-stereo.jpg`.

## Step 2: Create Your HTML File

Create a new HTML file with basic spatial image display:

```html
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Spatial Images Demo</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      padding: 20px;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      color: white;
    }
    .container {
      max-width: 800px;
      margin: 0 auto;
    }
    .image-section {
      margin: 30px 0;
      text-align: center;
    }
    .spatial-image {
      border: 3px solid #fff;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Spatial Images in JSAR</h1>

    <div class="image-section">
      <h2>Regular Image</h2>
      <img src="regular-photo.jpg" 
           style="width: 400px; height: 300px;" 
           alt="Regular photo" />
      <p>This image displays the same content to both eyes</p>
    </div>
    
    <div class="image-section">
      <h2>Spatial Stereo Image</h2>
      <img src="landscape-stereo.jpg" 
           spatial="stereo" 
           class="spatial-image"
           style="width: 400px; height: 200px;" 
           alt="Stereo landscape" />
      <p>This image shows different content to each eye, creating a 3D effect</p>
    </div>
  </div>
</body>
</html>
```

## Step 3: Understanding the Behavior

### In Stereo Mode (WebXR/Immersive)

- **Spatial images**: Left half displayed to left eye, right half to right eye
- **Regular images**: Full image displayed to both eyes (unchanged behavior)

### In Non-Stereo Mode

- **Spatial images**: Full side-by-side image displayed normally
- **Regular images**: Normal display (unchanged behavior)

## Step 4: Testing Your Spatial Images

1. **Test in JSAR Environment**: Run your HTML file in the JSAR runtime to see the full spatial effect
2. **Test in Regular Browser**: Verify that spatial images display as regular images (fallback behavior)
3. **Test Different Stereo Content**: Try various types of stereo images to understand the effect

## Step 5: Best Practices

1. **Optimize Image Size**: Spatial images contain twice the visual information, so compress appropriately
2. **Align Content**: Ensure left and right eye content is vertically aligned
3. **Test Parallax**: Use appropriate stereo separation for comfortable viewing
4. **Provide Fallbacks**: Always test how spatial images look in non-stereo mode

## Summary

You've learned how to:
- Create basic spatial images with the `spatial="stereo"` attribute
- Add interactive JavaScript functionality
- Handle dynamic spatial image creation
- Test and optimize spatial image content

Spatial images in JSAR provide a simple yet powerful way to add 3D depth to your immersive applications using standard HTML elements.
