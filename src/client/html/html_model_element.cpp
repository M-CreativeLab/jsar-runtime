#include "./html_model_element.hpp"
#include <client/dom/document.hpp>
#include <client/builtin_scene/model_3d.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/layout/layout_object.hpp>
#include <common/debug.hpp>

namespace dom
{
  using namespace builtin_scene;

  void HTMLModelElement::createdCallback(bool from_scripting)
  {
    HTMLElement::createdCallback(from_scripting);

    // Initialize attributes from HTML
    if (hasAttribute("src"))
      setSrc(getAttribute("src"));
    if (hasAttribute("type"))
      setType(getAttribute("type"));
    if (hasAttribute("autoplay"))
      setAutoplay(getAttribute("autoplay") == "true" || hasAttribute("autoplay"));
    if (hasAttribute("loading"))
      setLoading(getAttribute("loading"));

    loading_ = from_scripting ? "eager" : "lazy";
  }

  void HTMLModelElement::connectedCallback()
  {
    HTMLElement::connectedCallback();
    
    // Create the Model3d component for this element
    createModelComponent();
  }

  void HTMLModelElement::attributeChangedCallback(const std::string &name, const std::string &oldValue, const std::string &newValue)
  {
    HTMLElement::attributeChangedCallback(name, oldValue, newValue);

    if (name == "src")
    {
      setSrc(newValue);
    }
    else if (name == "type")
    {
      setType(newValue);
    }
    else if (name == "autoplay")
    {
      setAutoplay(newValue == "true" || !newValue.empty());
    }
    else if (name == "loading")
    {
      setLoading(newValue);
    }
  }

  std::string HTMLModelElement::src() const
  {
    return src_;
  }

  void HTMLModelElement::setSrc(const std::string &src)
  {
    if (src_ == src)
      return;
      
    src_ = src;
    
    // Update the Model3d component when src changes
    if (!src.empty())
    {
      updateModelComponent();
    }
  }

  std::optional<std::string> HTMLModelElement::type() const
  {
    return type_;
  }

  void HTMLModelElement::setType(const std::string &type)
  {
    type_ = type;
    updateModelComponent();
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

  void HTMLModelElement::createModelComponent()
  {
    // Get the layout object for this element
    auto layoutObject = getLayoutObject();
    if (!layoutObject)
      return;

    // Create Model3d component with detected type
    Model3d::ModelType modelType = detectModelType(src_, type_.value_or(""));
    auto model3d = std::make_shared<Model3d>(src_, modelType);
    
    // Add the component to the ECS entity associated with this element
    // This follows the same pattern as HTMLImageElement
    auto entityId = layoutObject->getEntityId();
    if (entityId != ecs::INVALID_ENTITY_ID)
    {
      auto &ecs = builtin_scene::ecs::ECS::getInstance();
      ecs.addComponent(entityId, model3d);
      
      DEBUG("HTMLModelElement", "Created Model3d component for entity %u with type %d", 
            entityId, static_cast<int>(modelType));
    }
  }

  void HTMLModelElement::updateModelComponent()
  {
    auto layoutObject = getLayoutObject();
    if (!layoutObject)
      return;

    auto entityId = layoutObject->getEntityId();
    if (entityId == ecs::INVALID_ENTITY_ID)
      return;

    auto &ecs = builtin_scene::ecs::ECS::getInstance();
    auto model3d = ecs.getComponent<Model3d>(entityId);
    
    if (!model3d)
    {
      // Create new component if it doesn't exist
      createModelComponent();
      return;
    }

    // Update existing component with new source and type
    Model3d::ModelType modelType = detectModelType(src_, type_.value_or(""));
    *model3d = Model3d(src_, modelType);
    
    // Mark associated WebContent as dirty to trigger re-rendering
    auto webContent = ecs.getComponent<WebContent>(entityId);
    if (webContent)
    {
      webContent->setDirty(true);
    }

    DEBUG("HTMLModelElement", "Updated Model3d component for entity %u", entityId);
  }

  Model3d::ModelType HTMLModelElement::detectModelType(const std::string &src, const std::string &typeHint)
  {
    if (!typeHint.empty())
    {
      if (typeHint == "3dgs" || typeHint == "gaussian-splatting")
      {
        return Model3d::ModelType::GaussianSplatting;
      }
    }

    // Auto-detect from file extension
    size_t dotPos = src.find_last_of('.');
    if (dotPos != std::string::npos)
    {
      std::string ext = src.substr(dotPos + 1);
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      
      if (ext == "gsplat" || ext == "ply")
      {
        return Model3d::ModelType::GaussianSplatting;
      }
      else if (ext == "gltf")
      {
        return Model3d::ModelType::GLTF;
      }
      else if (ext == "glb")
      {
        return Model3d::ModelType::GLB;
      }
    }

    return Model3d::ModelType::Unknown;
  }
}