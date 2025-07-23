#include "./html_model_element.hpp"
#include <client/dom/document.hpp>
#include <renderer/renderer.hpp>
#include <common/debug.hpp>

namespace dom
{
  std::string HTMLModelElement::src() const
  {
    return src_;
  }

  void HTMLModelElement::setSrc(const std::string &src)
  {
    src_ = src;
    
    // Load the 3DGS model when src changes
    if (!src.empty())
    {
      loadModel();
    }
  }

  std::optional<std::string> HTMLModelElement::type() const
  {
    return type_;
  }

  void HTMLModelElement::setType(const std::string &type)
  {
    type_ = type;
  }

  bool HTMLModelElement::autoplay() const
  {
    return autoplay_;
  }

  void HTMLModelElement::setAutoplay(bool autoplay)
  {
    autoplay_ = autoplay;
  }

  std::string HTMLModelElement::loading() const
  {
    return loading_;
  }

  void HTMLModelElement::setLoading(const std::string &loading)
  {
    if (loading == "lazy" || loading == "eager" || loading == "auto")
    {
      loading_ = loading;
    }
  }

  void HTMLModelElement::loadModel()
  {
    if (src_.empty())
      return;

    // Get the renderer and load the model
    auto &renderer = renderer::TrRenderer::GetRendererRef();
    auto *gaussianRenderer = renderer.getGaussianSplattingRenderer();
    
    if (gaussianRenderer)
    {
      // Check if this is a 3DGS model based on file extension or type hint
      bool is3DGS = false;
      
      if (type_.has_value())
      {
        is3DGS = (type_.value() == "3dgs" || type_.value() == "gaussian-splatting");
      }
      else
      {
        // Auto-detect from file extension
        is3DGS = (src_.ends_with(".gsplat") || src_.ends_with(".ply"));
      }
      
      if (is3DGS)
      {
        if (!gaussianRenderer->loadModel(src_))
        {
          // TODO: Dispatch error event
          DEBUG(LOG_TAG_RENDERER, "Failed to load 3DGS model: %s", src_.c_str());
        }
      }
      else
      {
        // Fall back to existing mesh/GLTF loading system
        // TODO: Integrate with existing model loading
        DEBUG(LOG_TAG_RENDERER, "Non-3DGS model loading not yet implemented: %s", src_.c_str());
      }
    }
  }
}