#include "./html_model_element.hpp"
#include <client/dom/document.hpp>
#include <client/builtin_scene/meshes/loaders/ksplat_loader.hpp>
#include <client/builtin_scene/meshes/loaders/spz_loader.hpp>
#include <client/builtin_scene/meshes/loaders/ply_loader.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/layout/layout_object.hpp>
#include <client/layout/layout_model3d.hpp>
#include <client/per_process.hpp>
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
      string loadingValue = getAttribute("loading");
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

    // Create the model component for this element
    createModelComponent();
  }

  void HTMLModelElement::attributeChangedCallback(const string &name, const string &oldValue, const string &newValue)
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

  string HTMLModelElement::src() const
  {
    return src_;
  }

  void HTMLModelElement::setSrc(const string &src)
  {
    if (src_ == src)
      return;

    // Reset loading state when src changes
    is_src_model_loaded_ = false;
    is_src_model_decoded_ = false;

    src_ = src;
    setAttribute("src", src, false);

    // Start loading if eager loading is enabled
    // if (loading_ == LoadingHint::kLoadingEager && !src.empty())
    //   loadModel();
    loadModel();
  }

  optional<string> HTMLModelElement::type() const
  {
    return type_;
  }

  void HTMLModelElement::setType(const string &type)
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

  string HTMLModelElement::loadingString() const
  {
    switch (loading_)
    {
    case LoadingHint::kLoadingLazy:
      return "lazy";
    case LoadingHint::kLoadingEager:
      return "eager";
    case LoadingHint::kLoadingAuto:
      return "auto";
    default:
      return "auto";
    }
  }

  void HTMLModelElement::setLoadingString(const string &loading)
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
    fetchArrayBufferLikeResource(src(), [this](const void *data, size_t length)
                                 { this->onModelDataReady(data, length); });
  }

  void HTMLModelElement::createModelComponent()
  {
    // TODO
  }

  void HTMLModelElement::updateModelComponent()
  {
    // Get the layout object associated with this element
    auto layoutObject = this->principalBox();
    if (!layoutObject || !layoutObject->isLayoutModel3d())
      return;

    auto layoutModel3d = dynamic_pointer_cast<client_layout::LayoutModel3d>(layoutObject);
    assert(layoutModel3d != nullptr && "LayoutModel3d should not be null");

    // Set the parsed model data in the layout object
    if (parsed_splats_.has_value())
      layoutModel3d->setModelData(parsed_splats_.value());
  }

  HTMLModelElement::ModelType HTMLModelElement::detectModelType(const string &src, const string &typeHint)
  {
    if (!typeHint.empty())
    {
      if (typeHint == "3dgs" || typeHint == "gaussian-splatting")
      {
        return ModelType::GaussianSplatting;
      }
      else if (typeHint == "ksplat")
      {
        return ModelType::KSplat;
      }
      else if (typeHint == "spz")
      {
        return ModelType::SPZ;
      }
      else if (typeHint == "ply")
      {
        return ModelType::PLY;
      }
    }

    // Auto-detect from file extension
    size_t dotPos = src.find_last_of('.');
    if (dotPos != string::npos)
    {
      string ext = src.substr(dotPos + 1);
      transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

      if (ext == "ksplat")
      {
        return ModelType::KSplat;
      }
      else if (ext == "spz")
      {
        return ModelType::SPZ;
      }
      else if (ext == "ply")
      {
        return ModelType::PLY;
      }
      else if (ext == "gltf")
      {
        return ModelType::GLTF;
      }
      else if (ext == "glb")
      {
        return ModelType::GLB;
      }
    }

    return ModelType::Unknown;
  }

  void HTMLModelElement::onModelDataReady(const void *modelData, size_t modelByteLength)
  {
    model_data_ = vector<char>(modelByteLength);
    model_data_->assign(static_cast<const char *>(modelData),
                        static_cast<const char *>(modelData) + modelByteLength);
    cout << "Model data received, size: " << modelByteLength << " bytes" << endl;

    // Mark the model as loaded.
    is_src_model_loading_ = false;
    is_src_model_loaded_ = true;

    // Dispatch the error event if the model data is null.
    if (TR_UNLIKELY(model_data_ == nullopt))
    {
      dispatchEvent(DOMEventType::Error);
      return;
    }

    // Parse model data asynchronously
    parseModelAsync(model_data_.value());
  }

  bool HTMLModelElement::parseModel(const vector<char> &modelData)
  {
    if (is_src_model_decoded_)
      return true;

    // Detect model type
    ModelType modelType = detectModelType(src_, type_.value_or(""));

    // Parse the model data based on type
    if (modelType == ModelType::KSplat || modelType == ModelType::GaussianSplatting)
    {
      // Use Ksplat parser to parse the model data
      vector<builtin_scene::GaussianSplat> parsedSplats;
      if (model_loaders::KsplatLoader::load(modelData, parsedSplats))
      {
        // Convert builtin_scene::GaussianSplat to HTMLModelElement::GaussianSplat for layout
        vector<GaussianSplat> elementSplats;
        elementSplats.reserve(parsedSplats.size());

        for (const auto &splat : parsedSplats)
        {
          GaussianSplat elementSplat;
          elementSplat.position[0] = splat.position[0];
          elementSplat.position[1] = splat.position[1];
          elementSplat.position[2] = splat.position[2];
          elementSplat.color[0] = splat.color[0];
          elementSplat.color[1] = splat.color[1];
          elementSplat.color[2] = splat.color[2];
          elementSplat.opacity = splat.opacity;
          elementSplat.scale[0] = splat.scale[0];
          elementSplat.scale[1] = splat.scale[1];
          elementSplat.scale[2] = splat.scale[2];
          elementSplat.rotation[0] = splat.rotation[0];
          elementSplat.rotation[1] = splat.rotation[1];
          elementSplat.rotation[2] = splat.rotation[2];
          elementSplat.rotation[3] = splat.rotation[3];
          elementSplats.push_back(elementSplat);
        }

        // Store parsed splats for layout
        parsed_splats_ = move(elementSplats);
        cout << "Successfully parsed .ksplat file with " << parsed_splats_->size() << " splats" << endl;
      }
      else
      {
        cerr << "Failed to parse Ksplat file"
             << (src_.empty() ? "" : " (" + src_ + ")")
             << ": invalid format or corrupted data" << endl;
      }
    }
    else if (modelType == ModelType::SPZ)
    {
      // Use SPZ parser to parse the model data
      vector<builtin_scene::GaussianSplat> parsedSplats;
      if (model_loaders::SpzLoader::load(modelData, parsedSplats))
      {
        // Convert builtin_scene::GaussianSplat to HTMLModelElement::GaussianSplat for layout
        vector<GaussianSplat> elementSplats;
        elementSplats.reserve(parsedSplats.size());

        for (const auto &splat : parsedSplats)
        {
          GaussianSplat elementSplat;
          elementSplat.position[0] = splat.position[0];
          elementSplat.position[1] = splat.position[1];
          elementSplat.position[2] = splat.position[2];
          elementSplat.color[0] = splat.color[0];
          elementSplat.color[1] = splat.color[1];
          elementSplat.color[2] = splat.color[2];
          elementSplat.opacity = splat.opacity;
          elementSplat.scale[0] = splat.scale[0];
          elementSplat.scale[1] = splat.scale[1];
          elementSplat.scale[2] = splat.scale[2];
          elementSplat.rotation[0] = splat.rotation[0];
          elementSplat.rotation[1] = splat.rotation[1];
          elementSplat.rotation[2] = splat.rotation[2];
          elementSplat.rotation[3] = splat.rotation[3];
          elementSplats.push_back(elementSplat);
        }

        // Store parsed splats for layout
        parsed_splats_ = move(elementSplats);
        cout << "Successfully parsed .spz file with " << parsed_splats_->size() << " splats" << endl;
      }
      else
      {
        cerr << "SPZ parsing failed" << endl;
      }
    }
    else if (modelType == ModelType::PLY)
    {
      // Use PLY parser to parse the model data
      vector<builtin_scene::GaussianSplat> parsedSplats;
      if (model_loaders::PlyLoader::load(modelData, parsedSplats))
      {
        // Convert builtin_scene::GaussianSplat to HTMLModelElement::GaussianSplat for layout
        vector<GaussianSplat> elementSplats;
        elementSplats.reserve(parsedSplats.size());

        for (const auto &splat : parsedSplats)
        {
          GaussianSplat elementSplat;
          elementSplat.position[0] = splat.position[0];
          elementSplat.position[1] = splat.position[1];
          elementSplat.position[2] = splat.position[2];
          elementSplat.color[0] = splat.color[0];
          elementSplat.color[1] = splat.color[1];
          elementSplat.color[2] = splat.color[2];
          elementSplat.opacity = splat.opacity;
          elementSplat.scale[0] = splat.scale[0];
          elementSplat.scale[1] = splat.scale[1];
          elementSplat.scale[2] = splat.scale[2];
          elementSplat.rotation[0] = splat.rotation[0];
          elementSplat.rotation[1] = splat.rotation[1];
          elementSplat.rotation[2] = splat.rotation[2];
          elementSplat.rotation[3] = splat.rotation[3];
          elementSplats.push_back(elementSplat);
        }

        // Store parsed splats for layout
        parsed_splats_ = move(elementSplats);
        cout << "Successfully parsed .ply file with " << parsed_splats_->size() << " splats" << endl;
      }
      else
      {
        cerr << "Failed to parse PLY file"
             << (src_.empty() ? "" : " (" + src_ + ")")
             << ": invalid format or corrupted data" << endl;
      }
    }
    else
    {
      cerr << "Model type not supported: GLTF/GLB model format is not yet supported." << endl;
      cerr << "Please use .ksplat, .spz, or .ply format for 3D Gaussian Splatting models." << endl;
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

  void HTMLModelElement::parseModelAsync(const vector<char> &modelData)
  {
    auto work = [](uv_work_t *handle)
    {
      if (handle != nullptr && handle->data != nullptr)
      {
        auto modelElement = static_cast<HTMLModelElement *>(handle->data);

        // Parse the model data
        modelElement->parseModel(modelElement->model_data_.value());

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
          // Create/update the model component
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

  void HTMLModelElement::onModelParsed()
  {
    // Update the ECS component with the parsed model
    updateModelComponent();

    // Dispatch load event
    complete = true;
    dispatchEvent(DOMEventType::Load);
  }
}