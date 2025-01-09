#pragma once

#include <memory>
#include <skia/include/core/SkCanvas.h>
#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <skia/modules/skparagraph/include/TextStyle.h>
#include "./ecs-inl.hpp"

namespace builtin_scene
{
  class WebContent : public ecs::Component
  {
  public:
    WebContent(SkCanvas *canvas) : canvas_(canvas) {}

  private:
    SkCanvas *canvas_;
  };

  /**
   * Systems for rendering Web content: background, boxes, text, etc.
   */
  namespace web_renderer
  {
    /**
     * Render the background.
     * 
     * - background
     * - border
     * - radius
     */
    class RenderBackgroundSystem final : public ecs::System
    {
    public:
      using ecs::System::System;

    public:
      void onExecute() override;
    };

    /**
     * Render the text.
     * 
     * - font
     * - size
     * - color
     */
    class RenderTextSystem final : public ecs::System
    {
    public:
      using ecs::System::System;

    public:
      void onExecute() override;
    };
  }

  /**
   * The plugin to render Web content spatially.
   */
  class WebContentPlugin final : public ecs::Plugin
  {
  public:
    using ecs::Plugin::Plugin;

  protected:
    void build(ecs::App &app) override
    {
      using namespace ecs;
      using namespace web_renderer;

      app.registerComponent<WebContent>();

      auto renderChain = System::Make<RenderBackgroundSystem>()
                             ->chain(System::Make<RenderTextSystem>());
      app.addSystem(SchedulerLabel::kUpdate, renderChain);
    }
  };
}
