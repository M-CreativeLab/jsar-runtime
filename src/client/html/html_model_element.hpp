#pragma once

#include <string>
#include <optional>
#include <client/html/html_element.hpp>
#include <client/builtin_scene/model_3d.hpp>

namespace dom
{
  /**
   * The HTMLModelElement interface provides special properties and methods for manipulating
   * the layout and presentation of <model> elements used for 3D model rendering.
   *
   * This element supports various 3D model formats including:
   * - GLTF/GLB models
   * - 3DGS (3D Gaussian Splatting) models (.ksplat)
   */
  class HTMLModelElement : public HTMLElement
  {
    using HTMLElement::HTMLElement;

  public:
    enum LoadingHint
    {
      // Loads the model immediately, regardless of whether or not the model is currently within the visible viewport
      kLoadingEager,
      // Defers loading the model until it reaches a calculated distance from the viewport
      kLoadingLazy,
      // No preference for the loading mode; the browser decides what is best for the user. This is the default value.
      kLoadingAuto,
    };

  public:
    HTMLModelElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("MODEL", ownerDocument)
    {
    }

    // HTMLElement lifecycle callbacks
    void createdCallback(bool from_scripting) override;
    void connectedCallback() override;
    void attributeChangedCallback(const std::string &name, const std::string &oldValue, const std::string &newValue) override;

    /**
     * Get the source URL of the 3D model file.
     * @return The URL string of the model file, or empty string if not set.
     */
    std::string src() const;

    /**
     * Set the source URL of the 3D model file.
     * @param src The URL string of the model file.
     */
    void setSrc(const std::string &src);

    /**
     * Get the model type hint.
     * @return The model type (e.g., "gltf", "3dgs"), or nullopt if auto-detected.
     */
    std::optional<std::string> type() const;

    /**
     * Set the model type hint.
     * @param type The model type hint (e.g., "gltf", "3dgs").
     */
    void setType(const std::string &type);

    /**
     * Get whether the model should auto-play animations.
     * @return true if animations should auto-play, false otherwise.
     */
    bool autoplay() const;

    /**
     * Set whether the model should auto-play animations.
     * @param autoplay true to auto-play animations, false otherwise.
     */
    void setAutoplay(bool autoplay);

    /**
     * Get the loading behavior for the model.
     * @return The loading hint for the model.
     */
    LoadingHint loading() const;

    /**
     * Set the loading behavior for the model.
     * @param loading The loading hint for the model.
     */
    void setLoading(LoadingHint loading);

    /**
     * Get the loading behavior as a string (for JavaScript bindings).
     * @return "lazy", "eager", or "auto".
     */
    std::string loadingString() const;

    /**
     * Set the loading behavior from a string (for JavaScript bindings).
     * @param loading "lazy", "eager", or "auto".
     */
    void setLoadingString(const std::string &loading);

    /**
     * Load the model at the scripting thread, if you want to achieve the loading from other threads, you must use
     * `loadModelAsync()`.
     */
    void loadModel();

    /**
     * Returns a boolean value that is true if the user agent has finished fetching the model, whether successful or
     * not. That means this value is also true if the model has no src value indicating a model to load.
     */
    bool complete = false;

  private:
    std::string src_;
    std::optional<std::string> type_;
    bool autoplay_ = false;
    LoadingHint loading_ = LoadingHint::kLoadingAuto;

    // Model loading state management (similar to HTMLImageElement)
    std::optional<std::vector<char>> model_data_ = std::nullopt;
    bool is_src_model_loading_ = false;
    bool is_src_model_loaded_ = false;
    bool is_src_model_decoded_ = false;

    /**
     * Create the Model3d component for this HTML element.
     */
    void createModelComponent();

    /**
     * Update the existing Model3d component with new attributes.
     */
    void updateModelComponent();

    /**
     * Detect the model type from source URL and type hint.
     */
    builtin_scene::Model3d::ModelType detectModelType(const std::string &src, const std::string &typeHint);

    /**
     * Called when model data is ready after fetch.
     */
    void onModelDataReady(const void *modelData, size_t modelByteLength);

    /**
     * Parse the model data and create Model3d components.
     */
    bool parseModel(const std::vector<char> &modelData, builtin_scene::Model3d &model);

    /**
     * Parse model data asynchronously.
     */
    void parseModelAsync(const std::vector<char> &modelData);

    /**
     * Called when model is successfully parsed.
     */
    void onModelParsed(const builtin_scene::Model3d &model);

    // UV work handle for async model parsing
    uv_work_t parse_work_handle_;
  };
}