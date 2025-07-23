#pragma once

#include <string>
#include <memory>
#include <vector>
#include "./ecs.hpp"

namespace builtin_scene
{
  // Forward declarations for 3DGS data structures
  struct GaussianSplat
  {
    float position[3];
    float color[3];
    float opacity;
    float scale[3];
    float rotation[4];
  };

  /**
   * Component for 3D models, particularly 3D Gaussian Splatting models.
   * This component stores the model data and rendering state.
   */
  class Model3d : public ecs::Component
  {
  public:
    enum class ModelType
    {
      Unknown,
      GLTF,
      GLB,
      GaussianSplatting  // For .gsplat and .ply files
    };

    Model3d(const std::string &src, ModelType type)
        : src_(src)
        , type_(type)
        , loaded_(false)
        , visible_(true)
    {
    }

  public:
    // Accessors
    inline const std::string &src() const { return src_; }
    inline ModelType type() const { return type_; }
    inline bool isLoaded() const { return loaded_; }
    inline bool visible() const { return visible_; }
    inline void setVisible(bool visible) { visible_ = visible; }

    // 3D Gaussian Splatting specific methods
    inline bool isGaussianSplatting() const { return type_ == ModelType::GaussianSplatting; }
    inline const std::vector<GaussianSplat> &getSplats() const { return splats_; }
    inline size_t getSplatCount() const { return splats_.size(); }

    // Model loading
    void setLoaded(bool loaded) { loaded_ = loaded; }
    void setSplats(std::vector<GaussianSplat> &&splats) 
    { 
      splats_ = std::move(splats);
      loaded_ = !splats_.empty();
    }

    // WebGL rendering context
    inline bool hasWebGLTexture() const { return webglTextureId_ != 0; }
    inline unsigned int getWebGLTextureId() const { return webglTextureId_; }
    inline void setWebGLTextureId(unsigned int textureId) { webglTextureId_ = textureId; }

  private:
    std::string src_;
    ModelType type_;
    bool loaded_;
    bool visible_;
    
    // 3DGS data
    std::vector<GaussianSplat> splats_;
    
    // WebGL rendering resources  
    unsigned int webglTextureId_ = 0;
  };
}