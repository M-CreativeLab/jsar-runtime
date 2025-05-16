#pragma once

#include <memory>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkRRect.h>
#include <skia/modules/skparagraph/include/FontCollection.h>
#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <skia/modules/skparagraph/include/TextStyle.h>
#include <client/cssom/computed_style.hpp>
#include <client/layout/fragment.hpp>
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
    /**
     * Construct a new `WebContent` object for rendering the classic web content.
     *
     * @param name The content name.
     * @param initialWidth The initial width of the content.
     * @param initialHeight The initial height of the content.
     */
    WebContent(std::string name, float initialWidth, float initialHeight);

  public:
    /**
     * The content name.
     */
    inline const std::string &name() const { return name_; }

    // Returns if the surface is valid.
    bool resetSkSurface(float width, float height);
    inline SkCanvas *canvas() const
    {
      if (TR_UNLIKELY(surface_ == nullptr))
        return nullptr;

      SkCanvas *canvas = surface_->getCanvas();
      // TODO(yorkie): support scaling the canvas.
      return canvas;
    }

    inline const client_cssom::ComputedStyle &style() const { return style_; }
    void setStyle(const client_cssom::ComputedStyle &style, std::shared_ptr<WebContent> parent = nullptr);

    inline const std::optional<client_layout::Fragment> &fragment() const { return lastFragment_; }
    inline void setFragment(const client_layout::Fragment &fragment) { lastFragment_ = fragment; }

    inline float physicalWidth() const { return surface_ == nullptr ? 0.0f : surface_->width(); }
    inline float physicalHeight() const { return surface_ == nullptr ? 0.0f : surface_->height(); }
    inline float logicalWidth() const { return physicalWidth() / devicePixelRatio_; }
    inline float logicalHeight() const { return physicalHeight() / devicePixelRatio_; }

    // Check if the surface needs to be resized.
    bool needsResize(float w, float h) const;

    inline glm::vec4 backgroundColor() const { return backgroundColor_; }
    inline void setBackgroundColor(float r, float g, float b, float a)
    {
      backgroundColor_ = glm::vec4(r, g, b, a);
    }
    inline std::shared_ptr<Texture> textureRect() const { return texture_; }
    /**
     * Init or resize the texture.
     *
     * @param textureAtlas The texture atlas to create or resize the texture.
     * @returns The texture or `nullptr` if the texture is not used.
     */
    std::shared_ptr<Texture> resizeOrInitTexture(TextureAtlas &textureAtlas);
    inline void setEnabled(bool enabled) { enabled_ = enabled; }
    inline bool isEnabled() const { return enabled_; }
    /**
     * Set the web content to use the texture or not.
     *
     * To reduce the texture memory usage, we will leverage simple rendering in GPU directly in some cases, such as: background
     * color without border and radius.
     *
     * @param value Whether to use the texture.
     */
    inline void setTextureUsing(bool value)
    {
      if (isTextureUsing_ != value)
        isTextureUsing_ = value;
    }
    inline bool isOpaque() const { return isOpaque_; }
    inline bool isTransparent() const { return !isOpaque_; }
    inline void setOpaque(bool b) { isOpaque_ = b; }
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
    sk_sp<SkSurface> surface_;
    std::string name_;
    client_cssom::ComputedStyle style_;
    std::optional<client_layout::Fragment> lastFragment_;
    WebContentStyle contentStyle_;
    SkRRect roundedRect_;
    glm::vec4 backgroundColor_;
    std::shared_ptr<Texture> texture_;
    float devicePixelRatio_ = 1.0f;
    bool enabled_ = true;
    bool isTextureUsing_ = false;
    bool isOpaque_ = false;
    bool isVisible_ = true;
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
