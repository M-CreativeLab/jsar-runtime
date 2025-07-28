# I Added Stereo <img> Rendering to Our Browser Engine — With Copilot

*Published: January 2025*

Recently, I set out to make spatial (stereo) image rendering as simple as possible in JSAR Runtime. My goal: let any web developer create immersive 3D content for XR just by writing HTML. And thanks to GitHub Copilot, this feature shipped faster and cleaner than ever.

## The Problem: Stereo Images Are Too Hard for the Web

Most browser engines treat all images as flat rectangles. If you want to display a stereo photo (side-by-side for left/right eyes), you usually have to dive into WebGL, shaders, or even game engines. That's a huge barrier for web developers.

I wanted a solution where you could just write:

```html
<img src="stereo-photo.png" spatial="stereo" />
```

And have the browser engine handle everything—splitting the image for each eye and rendering it correctly in an XR view.

## The Solution: Engine-Native Stereo Image Support

With this commit ([ff8e2918](https://github.com/M-CreativeLab/jsar-runtime/commit/ff8e2918d166527a0da15104fbcbcf529a9bd6b6)) and [PR #131](https://github.com/M-CreativeLab/jsar-runtime/pull/131), JSAR Runtime now supports the `spatial="stereo"` attribute on `<img>` tags. Here's how we made it work:

### 1. HTML Attribute Parsing

The first step was to teach the HTML parser to recognize `spatial="stereo"` on `<img>`.
- When this attribute is detected, the element is marked as a stereo image in the DOM tree.

### 2. Layout Logic

Next, we modified the layout engine:
- Instead of mapping the whole image to both eyes, we compute two sets of UV coordinates:
  - **Left Eye**: Maps to the left half of the image ([0,0]→[0.5,1]).
  - **Right Eye**: Maps to the right half ([0.5,0]→[1,1]).
- This logic is handled in the render tree, and the necessary information is passed down to the GPU renderer.

### 3. Renderer Changes

The renderer now checks for the spatial flag during draw calls:
- For stereo images, it issues two draw calls per frame:
  - One for the left eye, using the left-half UVs.
  - One for the right eye, using the right-half UVs.
- The renderer reuses the same GPU texture, applying the correct UVs for each eye—super efficient.

#### Code Snippet (from the commit):

```rust
if img_node.has_spatial_stereo() {
    // Left eye: render left half
    let left_uv = [0.0, 0.0, 0.5, 1.0];
    renderer.draw_image(img_node, left_uv, Eye::Left);

    // Right eye: render right half
    let right_uv = [0.5, 0.0, 1.0, 1.0];
    renderer.draw_image(img_node, right_uv, Eye::Right);
} else {
    // Regular image
    renderer.draw_image(img_node, [0.0, 0.0, 1.0, 1.0], Eye::Mono);
}
```

### 4. Copilot Collaboration

Throughout the implementation, I partnered with GitHub Copilot.
- **Boilerplate**: Copilot helped scaffold new Rust functions and types for DOM attribute parsing and renderer logic.
- **Edge Cases**: When handling image formats and UV calculations, Copilot made suggestions that sped up discovery and debugging.
- **Refactoring**: Copilot proposed clean ways to branch the rendering code, minimizing duplication.

It felt like true pair programming—Copilot would offer smart completions, and I could focus on architecture and integration.

## The Impact

- **Developer Simplicity**: You only need HTML to display immersive stereo content.
- **Performance**: No JS libraries, no shader code, just native engine speed.
- **Openness**: All implementation lives in one commit and PR #131.
- **AI-Augmented Workflow**: Copilot really does accelerate real browser engine work.

## Try It Yourself

Ready to experiment with stereo images in JSAR? Here's a complete example:

```html
<!DOCTYPE html>
<html>
<head>
  <style>
    .stereo-container {
      background: linear-gradient(135deg, #667eea, #764ba2);
      padding: 20px;
      border-radius: 10px;
      transform: translate3d(0, 0, 1px);
    }
    
    .stereo-image {
      width: 400px;
      height: 200px;
      border-radius: 8px;
      box-shadow: 0 4px 8px rgba(0,0,0,0.3);
    }
  </style>
</head>
<body>
  <div class="stereo-container">
    <h1>Stereo Image Demo</h1>
    <img src="my-stereo-photo.jpg" spatial="stereo" class="stereo-image" />
    <p>This side-by-side stereo image is automatically split for left/right eyes!</p>
  </div>
</body>
</html>
```

### Getting Started

```bash
# Clone and build JSAR Runtime
git clone https://github.com/M-CreativeLab/jsar-runtime.git
cd jsar-runtime
npm install && make jsbundle
make darwin  # or android for mobile XR
```

## Technical Architecture: How It Works Under the Hood

### DOM Integration

The stereo image support integrates seamlessly with JSAR's existing DOM architecture:

- **HTML Parser**: Extended to recognize the `spatial` attribute on `<img>` elements
- **DOM Tree**: Stereo flag is stored as metadata on the image node
- **CSS Integration**: Works with all existing CSS transforms and layout properties

### Rendering Pipeline

JSAR's multi-pass rendering system makes stereo support efficient:

```rust
// Simplified rendering flow
for eye in [Eye::Left, Eye::Right] {
    renderer.set_view_matrix(eye.view_matrix());
    renderer.set_projection_matrix(eye.projection_matrix());
    
    for img_node in scene.stereo_images() {
        let uv_coords = if eye == Eye::Left {
            [0.0, 0.0, 0.5, 1.0]  // Left half
        } else {
            [0.5, 0.0, 1.0, 1.0]  // Right half
        };
        renderer.draw_image(img_node, uv_coords, eye);
    }
}
```

### Cross-Platform Support

This feature works across all JSAR's supported platforms:

| Platform | Status | Notes |
|----------|--------|-------|
| **macOS** | ✅ Production | OpenGL backend |
| **Android** | ✅ Production | OpenGL ES3 backend |
| **Windows** | 🚧 In Progress | D3D11 backend |
| **Unity Integration** | ✅ Production | Via jsar-loader-unity |

## The Future of Spatial HTML

This stereo image feature is just the beginning. Our roadmap includes:

### Coming Soon
- **Video Support**: `spatial="stereo"` for `<video>` elements
- **360° Content**: Panoramic image and video support with `spatial="360"`
- **Depth Maps**: Support for depth-enhanced stereo content

### Experimental Features
- **AI-Generated Stereo**: Convert mono images to stereo using AI
- **Eye Tracking**: Adaptive stereo rendering based on eye position
- **Spatial Audio**: 3D audio that matches visual stereo depth

## Community and Collaboration

### The Role of AI in Development

Working with Copilot on this feature highlighted how AI can accelerate complex systems programming:

**What Copilot Excelled At:**
- Pattern recognition in existing codebase
- Boilerplate generation for similar structures
- Suggesting edge cases I hadn't considered
- Clean refactoring proposals

**Where Human Expertise Was Essential:**
- Architecture decisions and API design
- Integration with existing rendering pipeline
- Performance optimization strategies
- XR-specific domain knowledge

### Open Source Development

The entire implementation is open source and documented:

- **Commit**: [ff8e2918](https://github.com/M-CreativeLab/jsar-runtime/commit/ff8e2918d166527a0da15104fbcbcf529a9bd6b6)
- **Pull Request**: [#131](https://github.com/M-CreativeLab/jsar-runtime/pull/131)
- **Documentation**: Feature guide in our docs
- **Examples**: Demo applications in the examples folder

## What's Next?

Would you use HTML for more immersive content if the engine supported it natively? Any other spatial features you'd like to see built with AI pair programming?

**Get Involved:**
- ⭐ [Star us on GitHub](https://github.com/M-CreativeLab/jsar-runtime)
- 📖 [Read the documentation](https://deepwiki.com/m-creativelab/jsar-runtime)
- 💬 Join our community discussions
- 🐛 Report issues or suggest new spatial HTML features
- 🎯 Build amazing spatial web experiences

The spatial web is here, and it's built on the web technologies you already know. Let's make immersive computing accessible to every web developer.

---

*JSAR Runtime is developed by M-CreativeLab and the open source community. Licensed under the MIT License.*

**Links:**
- [GitHub Repository](https://github.com/M-CreativeLab/jsar-runtime)
- [Stereo Image PR #131](https://github.com/M-CreativeLab/jsar-runtime/pull/131)
- [Commit: ff8e2918](https://github.com/M-CreativeLab/jsar-runtime/commit/ff8e2918d166527a0da15104fbcbcf529a9bd6b6)
- [Documentation](https://deepwiki.com/m-creativelab/jsar-runtime)