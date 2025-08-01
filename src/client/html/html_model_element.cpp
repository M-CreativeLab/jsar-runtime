#include "./html_model_element.hpp"
#include <client/dom/document.hpp>
#include <client/builtin_scene/model_3d.hpp>
#include <client/builtin_scene/model_3d_renderer.hpp>
#include <client/builtin_scene/ply_parser.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/layout/layout_object.hpp>
#include <client/per_process.hpp>
#include <common/debug.hpp>
#include <algorithm>
#include <node/uv.h>

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
    {
      std::string loadingValue = getAttribute("loading");
      if (loadingValue == "lazy")
        setLoading(LoadingHint::kLoadingLazy);
      else if (loadingValue == "eager")
        setLoading(LoadingHint::kLoadingEager);
      else
        setLoading(LoadingHint::kLoadingAuto);
    }

    loading_ = from_scripting ? LoadingHint::kLoadingEager : LoadingHint::kLoadingLazy;
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
      if (newValue == "lazy")
        setLoading(LoadingHint::kLoadingLazy);
      else if (newValue == "eager")
        setLoading(LoadingHint::kLoadingEager);
      else
        setLoading(LoadingHint::kLoadingAuto);
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
      
    // Reset loading state when src changes
    is_src_model_loaded_ = false;
    is_src_model_decoded_ = false;
    
    src_ = src;
    setAttribute("src", src, false);
    
    // Start loading if eager loading is enabled
    if (loading_ == LoadingHint::kLoadingEager && !src.empty())
      loadModel();
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

  HTMLModelElement::LoadingHint HTMLModelElement::loading() const
  {
    return loading_;
  }

  void HTMLModelElement::setLoading(LoadingHint loading)
  {
    loading_ = loading;
  }

  std::string HTMLModelElement::loadingString() const
  {
    switch (loading_) {
      case LoadingHint::kLoadingLazy: return "lazy";
      case LoadingHint::kLoadingEager: return "eager";
      case LoadingHint::kLoadingAuto: return "auto";
      default: return "auto";
    }
  }

  void HTMLModelElement::setLoadingString(const std::string &loading)
  {
    if (loading == "lazy")
      setLoading(LoadingHint::kLoadingLazy);
    else if (loading == "eager")
      setLoading(LoadingHint::kLoadingEager);
    else
      setLoading(LoadingHint::kLoadingAuto);
  }

  void HTMLModelElement::loadModel()
  {
    if (is_src_model_loading_ ||
        is_src_model_loaded_ ||
        src().empty())
      return;

    is_src_model_loading_ = true;
    fetchResource(src(), [this](const void *data, size_t length)
                  { this->onModelDataReady(data, length); });
  }

  void HTMLModelElement::loadModelAsync()
  {
    if (is_src_model_loading_ || is_src_model_loaded_)
      return;

    // Schedule the model loading on the scripting thread.
    is_src_model_loading_ = true;
    fetchResourceThreadSafe(src(), [this](const void *data, size_t length)
                            { this->onModelDataReady(data, length); });
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

  void HTMLModelElement::onModelDataReady(const void *modelData, size_t modelByteLength)
  {
    model_data_ = std::vector<char>(modelByteLength);
    model_data_->assign(static_cast<const char *>(modelData),
                        static_cast<const char *>(modelData) + modelByteLength);

    // Mark the model as loaded.
    is_src_model_loading_ = false;
    is_src_model_loaded_ = true;

    // Dispatch the error event if the model data is null.
    if (TR_UNLIKELY(model_data_ == std::nullopt))
    {
      dispatchEvent(DOMEventType::Error);
      return;
    }

    // Parse model data asynchronously
    parseModelAsync(model_data_.value());
  }

  bool HTMLModelElement::parseModel(const std::vector<char> &modelData, Model3d &model)
  {
    if (is_src_model_decoded_)
      return true;

    // Detect model type
    Model3d::ModelType modelType = detectModelType(src_, type_.value_or(""));
    
    // Create new model with detected type
    model = Model3d(src_, modelType);
    
    // Parse the model data based on type
    // This is where the actual file parsing would happen
    // For now, we'll create a placeholder implementation
    
    if (modelType == Model3d::ModelType::GaussianSplatting)
    {
      // Use PLY parser to parse the model data
      std::vector<model_renderer::GaussianSplat> parsedSplats;
      
      if (model_renderer::PlyParser::parse(modelData, parsedSplats))
      {
        model.setSplats(parsedSplats);
        model.setLoaded(true);
        DEBUG("HTMLModelElement", "Successfully parsed PLY file with %zu splats", parsedSplats.size());
      }
      else
      {
        std::cerr << "PLY parsing failed" << std::endl;
      }
    }
    else
    {
      std::cerr << "Model type not supported yet: " << typeHint << std::endl;
      std::cerr << "GLTF/GLB parsing not yet implemented" << std::endl;
      // Model loading failed - do not set as loaded
    }

    is_src_model_decoded_ = true;
    
    // Clear model data after parsing
    if (model_data_.has_value())
    {
      model_data_->clear();
      model_data_.reset();
    }
    
    return is_src_model_decoded_;
  }

  void HTMLModelElement::parseModelAsync(const std::vector<char> &modelData)
  {
    auto work = [](uv_work_t *handle)
    {
      if (handle != nullptr && handle->data != nullptr)
      {
        auto modelElement = static_cast<HTMLModelElement *>(handle->data);
        
        // Create a temporary model to parse into
        Model3d tempModel("", Model3d::ModelType::Unknown);
        modelElement->parseModel(modelElement->model_data_.value(), tempModel);
        
        // Store the parsed model in the element (this is not thread-safe, but matches HTMLImageElement pattern)
        // In a real implementation, we'd need better synchronization
      }
    };
    
    auto afterWork = [](uv_work_t *handle, int status)
    {
      if (handle != nullptr && handle->data != nullptr)
      {
        auto modelElement = static_cast<HTMLModelElement *>(handle->data);
        if (modelElement->is_src_model_decoded_)
        {
          // Create/update the Model3d component
          modelElement->updateModelComponent();

          // Mark the model as completed.
          modelElement->complete = true;
          modelElement->dispatchEvent(DOMEventType::Load);
        }
        else
        {
          modelElement->dispatchEvent(DOMEventType::Error);
        }
      }
      else
      {
        assert(false);
      }
    };

    // Schedule the model parsing on the scripting thread.
    parse_work_handle_.data = this;
    uv_queue_work(TrClientContextPerProcess::Get()->getScriptingEventLoop(),
                  &parse_work_handle_,
                  work,
                  afterWork);
  }

  void HTMLModelElement::onModelParsed(const Model3d &model)
  {
    // Update the ECS component with the parsed model
    updateModelComponent();
    
    // Dispatch load event
    complete = true;
    dispatchEvent(DOMEventType::Load);
  }
}