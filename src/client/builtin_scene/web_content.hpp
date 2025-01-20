#pragma once

#include <memory>
#include <skia/include/core/SkCanvas.h>
#include <skia/modules/skparagraph/include/FontCollection.h>
#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <skia/modules/skparagraph/include/TextStyle.h>
#include <client/cssom/css_style_declaration.hpp>
#include <client/per_process.hpp>
#include "./ecs-inl.hpp"

namespace builtin_scene
{
  namespace web_renderer
  {
    class RenderBaseSystem;
    class RenderBackgroundSystem;
    class RenderTextSystem;
    class UpdateTextureSystem;
  }

  class WebContent : public ecs::Component
  {
    friend class web_renderer::UpdateTextureSystem;

  public:
    WebContent(SkCanvas *canvas, std::string name, client_cssom::CSSStyleDeclaration &style);

  public:
    /**
     * This inline function provides access to the SkCanvas object associated with the current instance.
     *
     * @returns SkCanvas* Pointer to the SkCanvas object.
     */
    inline SkCanvas *canvas() const { return canvas_; }
    /**
     * The content name.
     */
    inline const std::string &name() const { return name_; }
    /**
     * This function assigns the provided SkCanvas pointer to the internal `canvas_` member.
     *
     * @param canvas A pointer to an SkCanvas object to be set.
     */
    inline void setCanvas(SkCanvas *canvas)
    {
      canvas_ = canvas;
    }
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
    void setStyle(const client_cssom::CSSStyleDeclaration &style);
    /**
     * @returns A constant reference to the Layout object in the last re-layout.
     */
    inline const std::optional<crates::layout::Layout> &layout() const { return lastLayout_; }
    /**
     * Set the layout object in the last re-layout.
     */
    inline void setLayout(const crates::layout::Layout &layout) { lastLayout_ = layout; }
    /**
     * This function provides access to the width and height of the SkCanvas object.
     *
     * @returns float The width of the SkCanvas object.
     */
    inline float width() const
    {
      if (canvas_ == nullptr)
        return 0.0f;
      return canvas_->imageInfo().width();
    }
    /**
     * This function provides access to the width and height of the SkCanvas object.
     *
     * @returns float The height of the SkCanvas object.
     */
    inline float height() const
    {
      if (canvas_ == nullptr)
        return 0.0f;
      return canvas_->imageInfo().height();
    }
    /**
     * @returns Whether the content is dirty, namely needs to be re-rendered.
     */
    inline bool isDirty() const { return isDirty_; }

  private:
    void setDirty(bool dirty) { isDirty_ = dirty; }

  public:
    skia::textlayout::ParagraphStyle paragraphStyle;
    skia::textlayout::TextStyle textStyle;

  private:
    SkCanvas *canvas_;
    std::string name_;
    client_cssom::CSSStyleDeclaration style_;
    std::optional<crates::layout::Layout> lastLayout_;
    bool isDirty_ = true;
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

    public:
      const std::string name() const override { return "web_render.RenderBackgroundSystem"; }

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

    public:
      RenderTextSystem();

    public:
      const std::string name() const override { return "web_render.RenderTextSystem"; }

    private:
      void render(ecs::EntityId entity, WebContent &content) override;

    private:
      float getLayoutWidthForText(WebContent &content);

    private:
      TrClientContextPerProcess *clientContext_;
      sk_sp<skia::textlayout::FontCollection> fontCollection_;
      std::unique_ptr<skia::textlayout::ParagraphBuilder> paragraphBuilder_;
    };

    class UpdateTextureSystem final : public RenderBaseSystem
    {
    public:
      using RenderBaseSystem::RenderBaseSystem;

    public:
      const std::string name() const override { return "web_render.UpdateTextureSystem"; }

    public:
      void render(ecs::EntityId entity, WebContent &content) override;
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

      auto renderBackground = System::Make<RenderBackgroundSystem>();
      auto renderText = System::Make<RenderTextSystem>();
      auto updateTexture = System::Make<UpdateTextureSystem>();

      renderBackground->chain(renderText)->chain(updateTexture);
      app.addSystem(SchedulerLabel::kUpdate, renderBackground);
    }
  };
}
