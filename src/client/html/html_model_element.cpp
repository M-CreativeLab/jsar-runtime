#include "./html_model_element.hpp"
#include <client/dom/document.hpp>
#include <client/builtin_scene/meshes/loaders/gaussian_splat_loader.hpp>
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
      else if (loadingValue == "progressive")
        setLoading(LoadingHint::kLoadingProgressive);
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
      else if (newValue == "progressive")
        setLoading(LoadingHint::kLoadingProgressive);
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
    case LoadingHint::kLoadingProgressive:
      return "progressive";
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
    else if (loading == "progressive")
      setLoading(LoadingHint::kLoadingProgressive);
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

    // Parse the model data based on type and loading mode
    if (modelType == ModelType::KSplat || modelType == ModelType::GaussianSplatting)
    {
      // Use Ksplat parser to parse the model data
      vector<builtin_scene::GaussianSplat> parsedSplats;

      if (loading_ == LoadingHint::kLoadingProgressive)
      {
        // Use progressive loading
        if (parseModelProgressive(modelData, model_loaders::KsplatLoader()))
        {
          cout << "Successfully initialized progressive loading for .ksplat file" << endl;
          return true;
        }
        else
        {
          cerr << "Failed to initialize progressive loading for Ksplat file, falling back to full load" << endl;
          // Fall back to full loading
        }
      }

      // Full loading (either not progressive mode or progressive fallback)
      if (model_loaders::KsplatLoader::load(modelData, parsedSplats))
      {
        // Store parsed splats directly (unified type)
        parsed_splats_ = move(parsedSplats);
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

      if (loading_ == LoadingHint::kLoadingProgressive)
      {
        // Use progressive loading
        if (parseModelProgressive(modelData, model_loaders::SpzLoader()))
        {
          cout << "Successfully initialized progressive loading for .spz file" << endl;
          return true;
        }
        else
        {
          cerr << "Failed to initialize progressive loading for SPZ file, falling back to full load" << endl;
          // Fall back to full loading
        }
      }

      // Full loading (either not progressive mode or progressive fallback)
      if (model_loaders::SpzLoader::load(modelData, parsedSplats))
      {
        // Store parsed splats directly (unified type)
        parsed_splats_ = move(parsedSplats);
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

      if (loading_ == LoadingHint::kLoadingProgressive)
      {
        // Use progressive loading
        if (parseModelProgressive(modelData, model_loaders::PlyLoader()))
        {
          cout << "Successfully initialized progressive loading for .ply file" << endl;
          return true;
        }
        else
        {
          cerr << "Failed to initialize progressive loading for PLY file, falling back to full load" << endl;
          // Fall back to full loading
        }
      }

      // Full loading (either not progressive mode or progressive fallback)
      if (model_loaders::PlyLoader::Load(modelData, parsedSplats))
      {
        // Store parsed splats directly (unified type)
        parsed_splats_ = move(parsedSplats);
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

  template <typename LoaderType>
  bool HTMLModelElement::parseModelProgressive(const std::vector<char> &modelData, LoaderType loader)
  {
    // Initialize the specific loader with progressive loading
    progressive_loader_ = std::make_unique<LoaderType>(std::move(loader));
    progressive_model_data_ = modelData;

    // Set up progress callback
    auto progressCallback = [this](int loadedCount, int totalCount)
    {
      // Update the rendering system with partial results
      updateModelComponent();

      // Log progress
      if (totalCount > 0)
      {
        float progress = (float)loadedCount / totalCount * 100.0f;
        cout << "Progressive loading: " << loadedCount << "/" << totalCount
             << " splats (" << progress << "%)" << endl;
      }
    };

    // Initialize progressive loading
    if (!progressive_loader_->initProgressiveLoading(progressive_model_data_, progressCallback))
    {
      progressive_loader_.reset();
      return false;
    }

    // Initialize splats vector
    if (!parsed_splats_.has_value())
    {
      parsed_splats_ = std::vector<builtin_scene::GaussianSplat>();
    }

    progressive_loading_active_ = true;

    // Start loading the first batch
    loadNextProgressiveBatch();

    return true;
  }

  void HTMLModelElement::loadNextProgressiveBatch()
  {
    if (!progressive_loading_active_ || !progressive_loader_ ||
        progressive_loader_->isProgressiveLoadingComplete())
    {
      // Progressive loading is complete
      if (progressive_loading_active_)
      {
        progressive_loading_active_ = false;
        cout << "Progressive loading completed with "
             << (parsed_splats_.has_value() ? parsed_splats_->size() : 0)
             << " total splats" << endl;

        // Final update to the rendering system
        updateModelComponent();

        // Clean up
        progressive_loader_.reset();
        progressive_model_data_.clear();
      }
      return;
    }

    // Load next batch
    std::vector<builtin_scene::GaussianSplat> batchSplats;
    if (progressive_loader_->loadNextBatch(progressive_batch_size_, batchSplats))
    {
      // Append new splats to existing collection
      if (!parsed_splats_.has_value())
      {
        parsed_splats_ = std::move(batchSplats);
      }
      else
      {
        parsed_splats_->insert(parsed_splats_->end(), batchSplats.begin(), batchSplats.end());
      }

      // Update the rendering system with new splats
      updateModelComponent();

      // Schedule next batch (using a simple timer for now)
      // In a real implementation, this might be frame-rate dependent or use a proper scheduler
      scheduleNextProgressiveBatch();
    }
    else
    {
      // Loading failed or completed
      progressive_loading_active_ = false;
      progressive_loader_.reset();
    }
  }

  void HTMLModelElement::scheduleNextProgressiveBatch()
  {
    // Simple implementation using a timer
    // In a real implementation, this might be integrated with the rendering loop
    static uv_timer_t progressiveTimer;
    progressiveTimer.data = this;

    uv_timer_init(uv_default_loop(), &progressiveTimer);
    uv_timer_start(&progressiveTimer, [](uv_timer_t *handle)
                   {
      HTMLModelElement* element = static_cast<HTMLModelElement*>(handle->data);
      element->loadNextProgressiveBatch();
      uv_timer_stop(handle); },
                   16,
                   0); // 16ms delay (~60fps)
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