#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <client/graphics/webgl_context.hpp>

namespace builtin_scene
{
  // Forward declaration
  class Instance;

  /**
   * Manages the scroll shadow data texture for SDF scroll shadow rendering.
   * Stores scroll shadow properties in a texture-based format to avoid varying limits
   * and ensure compatibility with macOS OpenGL (which doesn't support SSBOs).
   * 
   * Texture layout: 3 columns × N rows where each row represents one instance:
   * - Column 0: scroll shadow color (r, g, b, a)
   * - Column 1: scroll shadow parameters (maxHeight, scrollOffset.x, scrollOffset.y, contentSize.x)
   * - Column 2: content size continuation (contentSize.y, 0, 0, 0)
   */
  class CSSScrollShadowTexture
  {
  public:
    CSSScrollShadowTexture();
    ~CSSScrollShadowTexture();

    /**
     * Initialize the scroll shadow data texture.
     * @param glContext The WebGL2 context to use for texture creation
     * @return true if initialization was successful, false otherwise
     */
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext);

    /**
     * Update the scroll shadow data texture with data from the given instances.
     * @param instances List of instances to extract scroll shadow data from
     */
    void updateScrollShadowData(const std::vector<std::shared_ptr<Instance>> &instances);

    /**
     * Bind the scroll shadow data texture to the specified texture unit.
     * @param textureUnit The texture unit to bind to (typically 2)
     */
    void bind(client_graphics::WebGLTextureUnit textureUnit);

    /**
     * Get the underlying WebGL texture.
     * @return The scroll shadow data texture
     */
    std::shared_ptr<client_graphics::WebGLTexture> getTexture() const
    {
      return scrollShadowDataTexture_;
    }

    /**
     * Check if the texture is initialized.
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const
    {
      return glContext_ != nullptr && scrollShadowDataTexture_ != nullptr;
    }

  private:
    /**
     * Ensure the texture is large enough for the given number of instances.
     * @param instanceCount Number of instances to accommodate
     */
    void ensureTextureSize(size_t instanceCount);

    /**
     * Extract scroll shadow data from a single instance.
     * @param instance The instance to extract data from
     * @param shadowColor Output parameter for shadow color
     * @param shadowMaxHeight Output parameter for max shadow height
     * @param scrollOffset Output parameter for scroll offset
     * @param contentSize Output parameter for content size
     */
    void extractInstanceScrollShadowData(const Instance &instance,
                                         glm::vec4 &shadowColor,
                                         float &shadowMaxHeight,
                                         glm::vec2 &scrollOffset,
                                         glm::vec2 &contentSize) const;

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
    std::shared_ptr<client_graphics::WebGLTexture> scrollShadowDataTexture_;
    std::vector<float> textureData_;
    size_t currentTextureHeight_;
  };
}