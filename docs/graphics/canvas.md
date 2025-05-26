# Canvas API Implementation

JSAR Runtime provides a comprehensive implementation of the Canvas API, supporting 2D rendering with a Skia backend. This document covers the Canvas implementation, its features, and usage patterns.

## Table of Contents

- [Overview](#overview)
- [Canvas Base Class](#canvas-base-class)
- [Canvas Class](#canvas-class)
- [OffscreenCanvas Class](#offscreencanvas-class)
- [Canvas Rendering Context 2D](#canvas-rendering-context-2d)
- [Text Rendering](#text-rendering)
- [Image Handling](#image-handling)
- [Examples](#examples)

## Overview

The Canvas API implementation in JSAR Runtime provides a complete 2D rendering system based on the Skia graphics library. It supports standard Canvas features such as drawing shapes, text, and images, as well as more advanced features like transformations and compositing.

## Canvas Base Class

The `CanvasBase` class is a template class that provides common functionality for both `Canvas` and `OffscreenCanvas` classes.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `skSurface` | `sk_sp<SkSurface>` | The Skia surface for rendering |
| `renderingContext` | `std::shared_ptr<RenderingContextBase<T>>` | The rendering context |
| `widthToSet` | `uint32_t` | The width to set for the canvas |
| `heightToSet` | `uint32_t` | The height to set for the canvas |

### Methods

```cpp
std::shared_ptr<RenderingContextBase<T>> getContext(RenderingContextType type);
size_t width() const;
size_t height() const;
bool readPixels(SkPixmap &dst) const;
void setWidth(uint32_t width);
void setHeight(uint32_t height);
void resize();
```

### Usage

The `CanvasBase` class is not used directly but serves as a base class for `Canvas` and `OffscreenCanvas`. It provides common functionality such as getting the rendering context, setting dimensions, and reading pixel data.

## Canvas Class

The `Canvas` class represents an HTML canvas element.

### Methods

```cpp
std::string toDataURL(const std::string &type, double encoderOptions);
```

### Usage

The `Canvas` class is used for rendering 2D graphics in the main thread. It provides methods for converting the canvas content to a data URL.

```javascript
const canvas = document.createElement('canvas');
canvas.width = 300;
canvas.height = 150;

const ctx = canvas.getContext('2d');
ctx.fillStyle = 'red';
ctx.fillRect(10, 10, 100, 100);

const dataURL = canvas.toDataURL();
```

## OffscreenCanvas Class

The `OffscreenCanvas` class represents an offscreen canvas, which can be used for rendering in a worker thread.

### Methods

```cpp
void commit();
```

### Usage

The `OffscreenCanvas` class is used for rendering 2D graphics in a worker thread. It provides methods for committing the canvas content to the main thread.

```javascript
const offscreenCanvas = new OffscreenCanvas(300, 150);
const ctx = offscreenCanvas.getContext('2d');
ctx.fillStyle = 'blue';
ctx.fillRect(10, 10, 100, 100);
offscreenCanvas.commit();
```

## Canvas Rendering Context 2D

The `CanvasRenderingContext2D` class provides methods for 2D rendering on a canvas.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `fillStyle` | `std::string` | The fill style for shapes |
| `strokeStyle` | `std::string` | The stroke style for shapes |
| `lineWidth` | `float` | The width of lines |
| `lineCap` | `std::string` | The style of line endpoints |
| `lineJoin` | `std::string` | The style of line joins |
| `miterLimit` | `float` | The maximum miter length |
| `globalAlpha` | `float` | The global alpha value |
| `globalCompositeOperation` | `std::string` | The global composite operation |
| `font` | `std::string` | The font for text rendering |
| `textAlign` | `TextAlign` | The text alignment |
| `textBaseline` | `TextBaseline` | The text baseline |
| `direction` | `std::string` | The text direction |
| `imageSmoothingEnabled` | `bool` | Whether image smoothing is enabled |
| `imageSmoothingQuality` | `std::string` | The quality of image smoothing |

### Methods

#### State Management

```cpp
void save();
void restore();
```

#### Transformations

```cpp
void scale(float x, float y);
void rotate(float angle);
void translate(float x, float y);
void transform(float a, float b, float c, float d, float e, float f);
void setTransform(float a, float b, float c, float d, float e, float f);
void resetTransform();
```

#### Path Operations

```cpp
void beginPath();
void closePath();
void moveTo(float x, float y);
void lineTo(float x, float y);
void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
void quadraticCurveTo(float cpx, float cpy, float x, float y);
void arc(float x, float y, float radius, float startAngle, float endAngle, bool counterclockwise);
void arcTo(float x1, float y1, float x2, float y2, float radius);
void rect(float x, float y, float width, float height);
```

#### Drawing Operations

```cpp
void fill();
void stroke();
void clip();
void fillRect(float x, float y, float width, float height);
void strokeRect(float x, float y, float width, float height);
void clearRect(float x, float y, float width, float height);
```

#### Text Operations

```cpp
void fillText(const std::string &text, float x, float y, std::optional<float> maxWidth);
void strokeText(const std::string &text, float x, float y, std::optional<float> maxWidth);
TextMetrics measureText(const std::string &text);
```

#### Image Operations

```cpp
void drawImage(std::shared_ptr<CanvasImageSource> image, float dx, float dy);
void drawImage(std::shared_ptr<CanvasImageSource> image, float dx, float dy, float dw, float dh);
void drawImage(std::shared_ptr<CanvasImageSource> image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh);
```

#### Pixel Manipulation

```cpp
ImageData createImageData(float width, float height);
ImageData getImageData(float sx, float sy, float sw, float sh);
void putImageData(const ImageData &imageData, float dx, float dy);
void putImageData(const ImageData &imageData, float dx, float dy, float dirtyX, float dirtyY, float dirtyWidth, float dirtyHeight);
```

### Usage

The `CanvasRenderingContext2D` class is used for 2D rendering on a canvas. It provides methods for drawing shapes, text, and images, as well as transformations and compositing.

```javascript
const canvas = document.createElement('canvas');
const ctx = canvas.getContext('2d');

// Drawing shapes
ctx.fillStyle = 'red';
ctx.fillRect(10, 10, 100, 100);

// Drawing text
ctx.font = '16px Arial';
ctx.fillStyle = 'black';
ctx.fillText('Hello, world!', 10, 130);

// Drawing images
const image = new Image();
image.src = 'image.png';
image.onload = () => {
  ctx.drawImage(image, 120, 10);
};
```

## Text Rendering

The Canvas API implementation in JSAR Runtime provides comprehensive text rendering capabilities.

### Text Alignment

The `TextAlign` enum defines the text alignment options:

- `kStart`: The text is aligned at the start of the text (left for left-to-right text, right for right-to-left text).
- `kEnd`: The text is aligned at the end of the text (right for left-to-right text, left for right-to-left text).
- `kLeft`: The text is aligned to the left.
- `kRight`: The text is aligned to the right.
- `kCenter`: The text is centered.

### Text Baseline

The `TextBaseline` enum defines the text baseline options:

- `kTop`: The text baseline is at the top of the em square.
- `kHanging`: The text baseline is at the hanging baseline.
- `kMiddle`: The text baseline is at the middle of the em square.
- `kAlphabetic`: The text baseline is at the alphabetic baseline.
- `kIdeographic`: The text baseline is at the ideographic baseline.
- `kBottom`: The text baseline is at the bottom of the em square.

### Text Metrics

The `TextMetrics` class provides information about the dimensions of a text string:

```cpp
class TextMetrics
{
public:
  TextMetrics(float width, float actualBoundingBoxLeft, float actualBoundingBoxRight, float actualBoundingBoxAscent, float actualBoundingBoxDescent, float fontBoundingBoxAscent, float fontBoundingBoxDescent, float emHeightAscent, float emHeightDescent, float hangingBaseline, float alphabeticBaseline, float ideographicBaseline);

  float width() const;
  float actualBoundingBoxLeft() const;
  float actualBoundingBoxRight() const;
  float actualBoundingBoxAscent() const;
  float actualBoundingBoxDescent() const;
  float fontBoundingBoxAscent() const;
  float fontBoundingBoxDescent() const;
  float emHeightAscent() const;
  float emHeightDescent() const;
  float hangingBaseline() const;
  float alphabeticBaseline() const;
  float ideographicBaseline() const;
};
```

### Usage

```javascript
const canvas = document.createElement('canvas');
const ctx = canvas.getContext('2d');

// Set text properties
ctx.font = '16px Arial';
ctx.textAlign = 'center';
ctx.textBaseline = 'middle';

// Draw text
ctx.fillText('Hello, world!', canvas.width / 2, canvas.height / 2);

// Measure text
const metrics = ctx.measureText('Hello, world!');
console.log('Text width:', metrics.width);
console.log('Text height:', metrics.actualBoundingBoxAscent + metrics.actualBoundingBoxDescent);
```

## Image Handling

The Canvas API implementation in JSAR Runtime provides comprehensive image handling capabilities.

### Image Sources

The `CanvasImageSource` interface represents objects that can be used as sources for drawing images:

- `HTMLImageElement`: An HTML image element.
- `HTMLVideoElement`: An HTML video element.
- `HTMLCanvasElement`: An HTML canvas element.
- `ImageBitmap`: An image bitmap.
- `OffscreenCanvas`: An offscreen canvas.

### Image Data

The `ImageData` class represents pixel data for a canvas:

```cpp
class ImageData
{
public:
  ImageData(uint32_t width, uint32_t height);
  ImageData(uint32_t width, uint32_t height, const std::vector<uint8_t> &data);

  uint32_t width() const;
  uint32_t height() const;
  std::vector<uint8_t> &data();
  const std::vector<uint8_t> &data() const;
};
```

### Usage

```javascript
const canvas = document.createElement('canvas');
const ctx = canvas.getContext('2d');

// Create image data
const imageData = ctx.createImageData(100, 100);
for (let i = 0; i < imageData.data.length; i += 4) {
  imageData.data[i + 0] = 255; // R
  imageData.data[i + 1] = 0;   // G
  imageData.data[i + 2] = 0;   // B
  imageData.data[i + 3] = 255; // A
}
ctx.putImageData(imageData, 10, 10);

// Get image data
const pixelData = ctx.getImageData(10, 10, 1, 1);
console.log('Pixel color:', pixelData.data[0], pixelData.data[1], pixelData.data[2], pixelData.data[3]);
```

## Examples

### Drawing Shapes

```javascript
const canvas = document.createElement('canvas');
canvas.width = 300;
canvas.height = 150;
const ctx = canvas.getContext('2d');

// Draw a rectangle
ctx.fillStyle = 'red';
ctx.fillRect(10, 10, 100, 100);

// Draw a circle
ctx.beginPath();
ctx.arc(200, 60, 50, 0, Math.PI * 2);
ctx.fillStyle = 'blue';
ctx.fill();

// Draw a line
ctx.beginPath();
ctx.moveTo(10, 120);
ctx.lineTo(290, 120);
ctx.strokeStyle = 'green';
ctx.lineWidth = 5;
ctx.stroke();
```

### Drawing Text

```javascript
const canvas = document.createElement('canvas');
canvas.width = 300;
canvas.height = 150;
const ctx = canvas.getContext('2d');

// Set text properties
ctx.font = '24px Arial';
ctx.textAlign = 'center';
ctx.textBaseline = 'middle';

// Draw filled text
ctx.fillStyle = 'black';
ctx.fillText('Hello, world!', 150, 50);

// Draw stroked text
ctx.strokeStyle = 'red';
ctx.lineWidth = 1;
ctx.strokeText('Hello, world!', 150, 100);
```

### Drawing Images

```javascript
const canvas = document.createElement('canvas');
canvas.width = 300;
canvas.height = 150;
const ctx = canvas.getContext('2d');

// Load an image
const image = new Image();
image.src = 'image.png';
image.onload = () => {
  // Draw the image
  ctx.drawImage(image, 10, 10);

  // Draw a scaled image
  ctx.drawImage(image, 150, 10, 100, 100);

  // Draw a cropped image
  ctx.drawImage(image, 0, 0, 100, 100, 10, 120, 50, 50);
};
```

### Using Transformations

```javascript
const canvas = document.createElement('canvas');
canvas.width = 300;
canvas.height = 150;
const ctx = canvas.getContext('2d');

// Save the current state
ctx.save();

// Apply transformations
ctx.translate(150, 75);
ctx.rotate(Math.PI / 4);
ctx.scale(1.5, 1.5);

// Draw a rectangle
ctx.fillStyle = 'purple';
ctx.fillRect(-25, -25, 50, 50);

// Restore the state
ctx.restore();

// Draw another rectangle
ctx.fillStyle = 'orange';
ctx.fillRect(10, 10, 50, 50);
```

### Using Compositing

```javascript
const canvas = document.createElement('canvas');
canvas.width = 300;
canvas.height = 150;
const ctx = canvas.getContext('2d');

// Draw a blue rectangle
ctx.fillStyle = 'blue';
ctx.fillRect(10, 10, 100, 100);

// Set compositing operation
ctx.globalCompositeOperation = 'source-atop';

// Draw a red circle
ctx.fillStyle = 'red';
ctx.beginPath();
ctx.arc(110, 60, 50, 0, Math.PI * 2);
ctx.fill();
```

### Using Offscreen Canvas

```javascript
// Create an offscreen canvas
const offscreenCanvas = new OffscreenCanvas(300, 150);
const ctx = offscreenCanvas.getContext('2d');

// Draw on the offscreen canvas
ctx.fillStyle = 'green';
ctx.fillRect(10, 10, 100, 100);

// Commit the offscreen canvas to the main thread
offscreenCanvas.commit();
```

### Pixel Manipulation

```javascript
const canvas = document.createElement('canvas');
canvas.width = 300;
canvas.height = 150;
const ctx = canvas.getContext('2d');

// Create image data
const imageData = ctx.createImageData(100, 100);

// Set pixel data
for (let y = 0; y < 100; y++) {
  for (let x = 0; x < 100; x++) {
    const index = (y * 100 + x) * 4;
    imageData.data[index + 0] = x / 100 * 255; // R
    imageData.data[index + 1] = y / 100 * 255; // G
    imageData.data[index + 2] = 0;             // B
    imageData.data[index + 3] = 255;           // A
  }
}

// Put image data on the canvas
ctx.putImageData(imageData, 10, 10);

// Get image data from the canvas
const pixelData = ctx.getImageData(10, 10, 100, 100);
```
