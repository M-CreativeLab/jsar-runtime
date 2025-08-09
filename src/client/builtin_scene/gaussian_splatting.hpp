#pragma once

#include <memory>
#include <string>
#include <vector>
#include "./ecs.hpp"

namespace builtin_scene
{
  // Forward declarations
  class WebXRExperience;

  // Forward declarations for 3DGS data structures
  struct GaussianSplat
  {
    float position[3];
    float color[3];
    float opacity;
    float scale[3];
    float rotation[4];
  };

  namespace gaussian_splatting
  {
    /**
     * System for initializing the global Gaussian splats mesh entity.
     * This system creates the global entity with Root component for rendering all splats.
     */
    class GaussianSplattingInitSystem final : public ecs::System
    {
      using ecs::System::System;

    public:
      const std::string name() const override
      {
        return "GaussianSplattingInitSystem";
      }
      void onExecute() override;
    };

    /**
     * System for updating Gaussian splats from individual model entities.
     * This system runs before RenderSystem to collect splats from all GaussianSplattingModel3d
     * entities and update the global GaussianSplatsMesh with proper depth sorting.
     */
    class GaussianSplatsUpdateSystem final : public ecs::System
    {
      using ecs::System::System;

    public:
      const std::string name() const override
      {
        return "GaussianSplatsUpdateSystem";
      }
      void onExecute() override;

    private:
      /**
       * Collect splats from all GaussianSplattingModel3d entities and update the global GaussianSplatsMesh.
       */
      void updateGlobalSplatsMesh();

    private:
      std::shared_ptr<WebXRExperience> xrExperience_ = nullptr;
    };
  }

  /**
   * Component for storing Gaussian splats data for individual model entities.
   * This component preserves the splats for each HTMLModelElement entity.
   */
  class GaussianSplattingModel3d : public ecs::Component
  {
  public:
    GaussianSplattingModel3d(const std::string &src)
        : src_(src)
        , loaded_(false)
        , visible_(true)
    {
    }

  public:
    // Accessors
    inline const std::string &src() const
    {
      return src_;
    }
    inline bool isLoaded() const
    {
      return loaded_;
    }
    inline bool visible() const
    {
      return visible_;
    }
    inline void setVisible(bool visible)
    {
      visible_ = visible;
    }

    // Splat data methods
    inline const std::vector<GaussianSplat> &getSplats() const
    {
      return splats_;
    }
    inline size_t getSplatCount() const
    {
      return splats_.size();
    }

    // Model loading
    void setLoaded(bool loaded)
    {
      loaded_ = loaded;
    }
    void setSplats(std::vector<GaussianSplat> &&splats)
    {
      splats_ = std::move(splats);
      loaded_ = !splats_.empty();
    }

  private:
    std::string src_;
    bool loaded_;
    bool visible_;

    // 3DGS data
    std::vector<GaussianSplat> splats_;
  };

  /**
   * Resource for managing Gaussian splatting context and shared state.
   * Similar to WebContentContext for managing the global Gaussian splats mesh entity.
   */
  class GaussianSplattingContext : public ecs::Resource
  {
    friend class gaussian_splatting::GaussianSplattingInitSystem;

  public:
    ecs::EntityId globalSplatsMeshEntity() const
    {
      return globalSplatsMeshEntity_;
    }

  private:
    ecs::EntityId globalSplatsMeshEntity_;
  };
}
