#pragma once

#include <memory>
#include <skia/include/core/SkCanvas.h>
#include <skia/modules/skparagraph/include/FontCollection.h>
#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <skia/modules/skparagraph/include/TextStyle.h>
#include <client/cssom/css_style_declaration.hpp>
#include "./ecs-inl.hpp"

namespace builtin_scene
{
  class WebContent : public ecs::Component
  {
  public:
    WebContent(SkCanvas *canvas, client_cssom::CSSStyleDeclaration &style)
        : canvas_(canvas), style_(style)
    {
      // Initialize the paragraph style and text style.
      paragraphStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);
      paragraphStyle.setTextDirection(skia::textlayout::TextDirection::kLtr);
      textStyle.setColor(SK_ColorBLACK);
      textStyle.setFontSize(14.0f);
    }

  public:
    /**
     * This inline function provides access to the SkCanvas object associated with the current instance.
     *
     * @returns SkCanvas* Pointer to the SkCanvas object.
     */
    inline SkCanvas *canvas() const { return canvas_; }
    /**
     * This function assigns the provided SkCanvas pointer to the internal `canvas_` member.
     *
     * @param canvas A pointer to an SkCanvas object to be set.
     */
    inline void setCanvas(SkCanvas *canvas) { canvas_ = canvas; }
    /**
     * This function provides access to the CSS style declaration associated with the object.
     *
     * @returns A constant reference to the CSSStyleDeclaration object.
     */
    inline const client_cssom::CSSStyleDeclaration &style() const { return style_; }
    /**
     * This function assigns the provided CSSStyleDeclaration object to the internal `style_` member.
     *
     * @param style A CSSStyleDeclaration object to be set.
     */
    inline void setStyle(const client_cssom::CSSStyleDeclaration &style)
    {
      style_ = style;
    }

  public:
    skia::textlayout::ParagraphStyle paragraphStyle;
    skia::textlayout::TextStyle textStyle;

  private:
    SkCanvas *canvas_;
    client_cssom::CSSStyleDeclaration style_;
  };

  /**
   * Systems for rendering Web content: background, boxes, text, etc.
   */
  namespace web_renderer
  {
    class RenderBaseSystem : public ecs::System
    {
    public:
      using ecs::System::System;

    public:
      void onExecute() override final;

    protected:
      virtual void render(ecs::EntityId entity, WebContent &content) = 0;
    };

    /**
     * Render the background.
     *
     * - background
     * - border
     * - radius
     */
    class RenderBackgroundSystem final : public RenderBaseSystem
    {
    public:
      using RenderBaseSystem::RenderBaseSystem;

    private:
      void render(ecs::EntityId entity, WebContent &content) override;
    };

    /**
     * Render the text.
     *
     * - font
     * - size
     * - color
     */
    class RenderTextSystem final : public RenderBaseSystem
    {
    public:
      using RenderBaseSystem::RenderBaseSystem;

    private:
      void render(ecs::EntityId entity, WebContent &content);
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
