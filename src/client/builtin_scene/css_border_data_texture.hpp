#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <client/graphics/webgl_context.hpp>

namespace builtin_scene
{
  // Forward declaration
  class Instance;
  class RenderableInstancesList;

  /**
   * Manages the border data texture for SDF border rendering.
   * Stores border widths and colors in a texture-based format to avoid UBO limits
   * and ensure compatibility with macOS OpenGL (which doesn't support SSBOs).
   * 
   * Texture layout: 5 columns × N rows where each row represents one instance:
   * - Column 0: border widths (top, right, bottom, left)
   * - Columns 1-4: border colors for each side (top, right, bottom, left)
   */
  class CSSBorderDataTexture
  {
  public:
    CSSBorderDataTexture();
    ~CSSBorderDataTexture();

    /**
     * Initialize the border data texture.
     * @param glContext The WebGL2 context to use for texture creation
     * @return true if initialization was successful, false otherwise
     */
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext);

    /**
     * Update the border data texture with data from the given instances.
     * @param instances List of instances to extract border data from
     */
    void updateBorderData(const std::vector<std::shared_ptr<Instance>> &instances);

    /**
     * Bind the border data texture to the specified texture unit.
     * @param textureUnit The texture unit to bind to (typically 1)
     */
    void bind(client_graphics::WebGLTextureUnit textureUnit);

    /**
     * Get the underlying WebGL texture.
     * @return The border data texture
     */
    std::shared_ptr<client_graphics::WebGLTexture> getTexture() const
    {
      return borderDataTexture_;
    }

    /**
     * Check if the texture has been initialized.
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const
    {
      return borderDataTexture_ != nullptr;
    }

  private:
    /**
     * Resize the texture if needed to accommodate the given number of instances.
     * @param instanceCount Number of instances that need border data storage
     */
    void ensureTextureSize(size_t instanceCount);

    /**
     * Extract border data from a single instance.
     * @param instance The instance to extract border data from
     * @param borderWidth Output parameter for border width
     * @param borderColors Output parameter for border colors (4 sides)
     */
    void extractInstanceBorderData(const Instance &instance,
                                   glm::vec4 &borderWidth,
                                   glm::vec4 borderColors[4]);

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
    std::shared_ptr<client_graphics::WebGLTexture> borderDataTexture_;
    size_t currentTextureHeight_;
    bool isDirty_;

    // Buffer for texture data (5 columns × N rows of RGBA32F data)
    std::vector<float> textureData_;
  };
}
