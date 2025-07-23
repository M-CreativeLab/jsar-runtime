# Creating Test Images for Spatial (Stereo) Images

This guide explains how to create test images to validate the spatial image functionality in JSAR.

## Test Image Format

Spatial images should be side-by-side stereo pairs with a 2:1 aspect ratio:
- **Total dimensions**: 400x200 pixels (or any 2:1 ratio)
- **Left half**: First 200x200 pixels (left eye view)
- **Right half**: Last 200x200 pixels (right eye view)

## Creating Simple Test Images

### Method 1: Simple Text-Based Test Image

Create an image with different text for each eye:

```
[LEFT EYE]    [RIGHT EYE]
    L             R
```

This can be done with any image editor by:
1. Creating a 400x200 canvas
2. Drawing "L" on the left half (0-200px)
3. Drawing "R" on the right half (200-400px)

### Method 2: Color-Based Test Image

Create an image with different colors:
- Left half: Red background
- Right half: Blue background

### Method 3: Shape-Based Test Image

Create geometric patterns:
- Left half: Circle in center
- Right half: Square in center

## Testing the Implementation

Use these test images in your HTML:

```html
<!DOCTYPE html>
<html>
<head>
    <title>Spatial Image Test</title>
</head>
<body>
    <h1>Spatial Image Tests</h1>
    
    <!-- Test 1: Text-based -->
    <h2>Text Test (Should show 'L' in left eye, 'R' in right eye)</h2>
    <img src="test-text-lr.jpg" spatial="stereo" style="width: 300px; height: 150px; border: 2px solid red;" />
    
    <!-- Test 2: Color-based -->
    <h2>Color Test (Should show red in left eye, blue in right eye)</h2>
    <img src="test-color-redblue.jpg" spatial="stereo" style="width: 300px; height: 150px; border: 2px solid green;" />
    
    <!-- Test 3: Regular image for comparison -->
    <h2>Regular Image (Should show full image in both eyes)</h2>
    <img src="test-regular.jpg" style="width: 300px; height: 150px; border: 2px solid blue;" />
    
    <!-- Test 4: JavaScript creation -->
    <h2>JavaScript Created Spatial Image</h2>
    <div id="js-test"></div>
    
    <script>
        // Create spatial image via JavaScript
        const spatialImg = document.createElement('img');
        spatialImg.src = 'test-js-spatial.jpg';
        spatialImg.setAttribute('spatial', 'stereo');
        spatialImg.style.width = '300px';
        spatialImg.style.height = '150px';
        spatialImg.style.border = '2px solid orange';
        
        spatialImg.onload = function() {
            console.log('Spatial image loaded successfully');
            console.log('Spatial attribute:', this.getAttribute('spatial'));
        };
        
        document.getElementById('js-test').appendChild(spatialImg);
    </script>
</body>
</html>
```

## Validation Checklist

When testing spatial images in JSAR:

### In Stereo Mode (WebXR):
- [ ] Left eye sees only the left half of the spatial image
- [ ] Right eye sees only the right half of the spatial image  
- [ ] Regular (non-spatial) images appear the same in both eyes
- [ ] Image maintains proper aspect ratio and scaling

### In Non-Stereo Mode:
- [ ] Spatial images display the full side-by-side image
- [ ] Regular images display normally
- [ ] No errors or distortions occur

### JavaScript API:
- [ ] `setAttribute('spatial', 'stereo')` works correctly
- [ ] `getAttribute('spatial')` returns 'stereo' for spatial images
- [ ] Dynamically created spatial images render correctly

## Sample Image Specifications

Here are the exact specifications for creating test images:

### Text Test Image (400x200px):
```
Background: White (#FFFFFF)
Left half (0-200px): Large black "L" centered
Right half (200-400px): Large black "R" centered
Font: Bold, Arial, 120px
```

### Color Test Image (400x200px):
```
Left half (0-200px): Solid red (#FF0000)
Right half (200-400px): Solid blue (#0000FF)
```

### Pattern Test Image (400x200px):
```
Background: Light gray (#EEEEEE)
Left half: Black circle, radius 60px, centered at 100,100
Right half: Black square, 120x120px, centered at 300,100
```

These test images will clearly demonstrate whether the spatial image splitting is working correctly in both stereo and non-stereo modes.