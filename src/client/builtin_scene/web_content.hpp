#pragma once

#include <memory>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkRRect.h>
#include <skia/modules/skparagraph/include/FontCollection.h>
#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <skia/modules/skparagraph/include/TextStyle.h>
#include <client/cssom/css_style_declaration.hpp>
#include <client/per_process.hpp>

#include "./ecs-inl.hpp"
#include "./texture_altas.hpp"

namespace builtin_scene
{
  namespace web_renderer
  {
    class InitSystem;
    class RenderBaseSystem;
    class RenderBackgroundSystem;
    class RenderImageSystem;
    class RenderTextSystem;
    class UpdateTextureSystem;
  }

  struct WebContentFontStyle
  {
    SkFontStyle::Slant slant;
    SkFontStyle::Weight weight;
    SkFontStyle::Width width;
  };

  class WebContentTextStyle
  {
  public:
    WebContentTextStyle();

  public:
    SkColor color;
    std::optional<SkColor> foregroundColor;
    std::optional<SkColor> backgroundColor;
    uint8_t decoration;
    SkScalar decorationThickness;
    SkColor decorationColor;
    WebContentFontStyle fontStyle;
    std::vector<SkString> fontFamilies;
    SkScalar fontSize;
    std::optional<SkScalar> letterSpacing;
    std::optional<SkScalar> wordSpacing;
  };

  class WebContentStyle
  {
  public:
    WebContentStyle();

  public:
    bool disableHinting;
    size_t maxLines;
    skia::textlayout::TextAlign textAlign;
    skia::textlayout::TextDirection textDirection;
    skia::textlayout::TextHeightBehavior textHeightBehavior;
    WebContentTextStyle textStyle;
    SkScalar lineHeight;
    bool useFixedLineHeight;
    bool halfLeading;
    SkScalar leading;
    bool strutEnabled;
    bool forceStrutHeight;
    bool applyRoundingHack;
  };

  class WebContent : public ecs::Component
  {
    friend class web_renderer::RenderBackgroundSystem;
    friend class web_renderer::RenderImageSystem;
    friend class web_renderer::RenderTextSystem;
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
      setDirty(true);
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
     * @param parent The parent WebContent object.
     */
    void setStyle(const client_cssom::CSSStyleDeclaration &style, std::shared_ptr<WebContent> parent = nullptr);
    /**
     * @returns A constant reference to the Layout object in the last re-layout.
     */
    inline const std::optional<crates::layout2::Layout> &layout() const { return lastLayout_; }
    /**
     * Set the layout object in the last re-layout.
     */
    inline void setLayout(const crates::layout2::Layout &layout) { lastLayout_ = layout; }
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
    inline std::shared_ptr<Texture> textureRect() const { return texture_; }
    /**
     * Init or resize the texture.
     *
     * @param textureAtlas The texture atlas to create or resize the texture.
     * @returns The texture.
     */
    inline std::shared_ptr<Texture> resizeOrInitTexture(TextureAtlas &textureAtlas)
    {
      float w = width();
      float h = height();

      if (texture_ == nullptr)
        texture_ = textureAtlas.addTexture(w, h, true);
      else
        texture_ = textureAtlas.resizeTexture(texture_, w, h, true);

      assert(texture_ != nullptr && "The texture must be valid.");
      return texture_;
    }
    inline bool isOpaque() const { return isOpaque_; }
    inline bool isTransparent() const { return !isOpaque_; }
    inline void setOpaque(bool isOpaque) { isOpaque_ = isOpaque; }
    /**
     * @returns Whether the content is dirty, namely needs to be re-rendered.
     */
    inline bool isDirty() const { return isDirty_; }
    /**
     * Mark the content as dirty or not.
     *
     * @param dirty Whether the content is dirty.
     */
    inline void setDirty(bool dirty)
    {
      isDirty_ = dirty;
    }

  public:
    skia::textlayout::TextStyle textStyle() const;
    skia::textlayout::StrutStyle structStyle() const;
    skia::textlayout::ParagraphStyle paragraphStyle() const;

  private:
    SkCanvas *canvas_;
    std::string name_;
    client_cssom::CSSStyleDeclaration style_;
    std::optional<crates::layout2::Layout> lastLayout_;
    WebContentStyle contentStyle_;
    SkRRect roundedRect_;
    std::shared_ptr<Texture> texture_;
    bool isOpaque_ = false;
    bool isDirty_ = true;
  };

  class WebContentContext : public ecs::Resource
  {
    friend class web_renderer::InitSystem;

  public:
    ecs::EntityId instancedMeshEntity() const { return instancedMeshEntity_; }

  private:
    ecs::EntityId instancedMeshEntity_;
  };

  /**
   * Systems for rendering Web content: background, boxes, text, etc.
   */
  namespace web_renderer
  {
    class InitSystem final : public ecs::System
    {
    public:
      using ecs::System::System;

    public:
      const std::string name() const override { return "web_render.InitSystem"; }
      void onExecute() override;
    };

    class RenderBaseSystem : public ecs::System
    {
    public:
      using ecs::System::System;

    public:
      void onExecute() override final;

    protected:
      virtual void render(ecs::EntityId entity, WebContent &content) = 0;

    protected:
      /**
       * Get the instanced mesh component.
       *
       * @tparam ComponentType The type of the component.
       * @returns The instanced mesh component.
       */
      template <typename ComponentType>
      std::shared_ptr<ComponentType> getInstancedMeshComponent()
      {
        if (webContentCtx_ == nullptr)
          webContentCtx_ = getResource<WebContentContext>();
        assert(webContentCtx_ != nullptr && "The WebContentContext must be valid.");

        auto entity = webContentCtx_->instancedMeshEntity();
        return getComponent<ComponentType>(entity);
      }
      /**
       * Get the object reference to the instanced mesh's component, it will expect the component to be valid.
       *
       * @tparam ComponentType The type of the component.
       * @returns The object reference to the instanced mesh's component.
       */
      template <typename ComponentType>
      ComponentType &getInstancedMeshComponentChecked()
      {
        auto component = getInstancedMeshComponent<ComponentType>();
        assert(component != nullptr && "The instanced mesh component must be valid.");
        return *component;
      }

    private:
      std::shared_ptr<WebContentContext> webContentCtx_;
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

    class RenderImageSystem final : public RenderBaseSystem
    {
    public:
      using RenderBaseSystem::RenderBaseSystem;

    public:
      const std::string name() const override { return "web_render.RenderImageSystem"; }

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

      app.addResource(Resource::Make<WebContentContext>());
      app.registerComponent<WebContent>();

      auto initWebContent = System::Make<InitSystem>();
      app.addSystem(SchedulerLabel::kPostStartup, initWebContent);

      auto renderBackground = System::Make<RenderBackgroundSystem>();
      auto renderImage = System::Make<RenderImageSystem>();
      auto renderText = System::Make<RenderTextSystem>();
      auto updateTexture = System::Make<UpdateTextureSystem>();

      renderBackground
          ->chain(renderImage)
          ->chain(renderText)
          ->chain(updateTexture);
      app.addSystem(SchedulerLabel::kUpdate, renderBackground);
    }
  };
}
