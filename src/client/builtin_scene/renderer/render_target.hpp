#pragma once

#include <memory>
#include <vector>
#include <client/builtin_scene/xr.hpp>

namespace endor
{
  namespace builtin_scene
  {
    /**
   * The WebXR render target for the renderer, which stores the view or views to render.
   */
    class XRRenderTarget
    {
    public:
      /**
     * Construct the render target for the single view.
     *
     * @param view The view to render.
     */
      XRRenderTarget(std::shared_ptr<client_xr::XRView> view)
          : multiview_(false)
          , view_(view)
      {
      }
      /**
     * Construct the render target for the multiple views.
     *
     * @param views The views to render.
     */
      XRRenderTarget(const std::vector<std::shared_ptr<client_xr::XRView>> &views)
          : multiview_(true)
          , views_(&views)
      {
      }

    public:
      /**
       * @returns Whether the render target is multiview.
       */
      inline bool isMultiview() const
      {
        return multiview_;
      }
      /**
     * @returns The view to render, it is `nullptr` if the render target is multiview.
     */
      inline std::shared_ptr<client_xr::XRView> view() const
      {
        return view_;
      }
      /**
     * @returns The views to render, and it will throw an exception if the render target is not multiview.
     */
      inline const std::vector<std::shared_ptr<client_xr::XRView>> &views() const
      {
        if (views_ == nullptr)
          throw std::runtime_error("The render target is not multiview.");
        return *views_;
      }

    private:
      bool multiview_ = false;
      std::shared_ptr<client_xr::XRView> view_ = nullptr;
      const std::vector<std::shared_ptr<client_xr::XRView>> *views_ = nullptr;
    };
  }
} // namespace endor
