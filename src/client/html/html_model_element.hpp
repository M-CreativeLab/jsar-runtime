#pragma once

#include <string>
#include <optional>
#include <client/html/html_element.hpp>

namespace dom
{
  /**
   * The HTMLModelElement interface provides special properties and methods for manipulating
   * the layout and presentation of <model> elements used for 3D model rendering.
   *
   * This element supports various 3D model formats including:
   * - GLTF/GLB models (existing support)
   * - 3DGS (3D Gaussian Splatting) models (.gsplat, .ply)
   */
  class HTMLModelElement : public HTMLElement
  {
    using HTMLElement::HTMLElement;

  public:
    HTMLModelElement(const HTMLModelElement &) = delete;
    HTMLModelElement &operator=(const HTMLModelElement &) = delete;

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
     * @return "lazy", "eager", or "auto" (default).
     */
    std::string loading() const;

    /**
     * Set the loading behavior for the model.
     * @param loading "lazy", "eager", or "auto".
     */
    void setLoading(const std::string &loading);

  private:
    std::string src_;
    std::optional<std::string> type_;
    bool autoplay_ = false;
    std::string loading_ = "auto";

    /**
     * Internal method to load the 3D model based on current src and type.
     */
    void loadModel();
  };
}