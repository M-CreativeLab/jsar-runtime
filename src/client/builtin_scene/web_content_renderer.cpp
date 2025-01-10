#include <assert.h>
#include <skia/include/core/SkCanvas.h>
#include "./web_content.hpp"
#include "./text.hpp"

namespace builtin_scene::web_renderer
{
  void RenderBaseSystem::onExecute()
  {
    auto entities = queryEntities<WebContent>();
    if (entities.size() == 0)
      return;

    for (auto &entity : entities)
    {
      auto content = getComponent<WebContent>(entity);
      assert(content != nullptr);
      render(entity, *content);
    }
  }

  void RenderBackgroundSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto canvas = content.canvas();

    // Draw the background.
    canvas->clear(SK_ColorWHITE);

    // Draw the border.
    SkPaint paint;
    paint.setColor(SK_ColorBLACK);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(1.0f);
    canvas->drawRect(SkRect::MakeWH(300.0f, 300.0f), paint);
  }

  void RenderTextSystem::render(ecs::EntityId entity, WebContent &content)
  {
    using namespace skia::textlayout;

    auto fontCollection = sk_make_sp<FontCollection>();
    auto paragraphBuilder = ParagraphBuilder::make(content.paragraphStyle, fontCollection);

    paragraphBuilder->pushStyle(content.textStyle);
    paragraphBuilder->addText("Hello, world!");
    paragraphBuilder->pop();

    auto paragraph = paragraphBuilder->Build();
    paragraph->layout(300.0f);
    paragraph->paint(content.canvas(), 0.0f, 0.0f);
  }
}
