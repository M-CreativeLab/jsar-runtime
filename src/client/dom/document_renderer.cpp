#include <functional>
#include <client/per_process.hpp>
#include <client/builtin_scene/scene.hpp>

#include "./document-inl.hpp"
#include "./document_renderer.hpp"

namespace dom
{
  using namespace std;
  using namespace std::placeholders;
  using namespace builtin_scene;
  using namespace client_layout;

  RenderHTMLDocument::RenderHTMLDocument(HTMLDocument *document)
      : ecs::System()
      , DocumentEventDispatcher(document)
      , document_(document)
  {
    assert(document_ != nullptr);
  }

  RenderHTMLDocument::~RenderHTMLDocument() = default;

  void RenderHTMLDocument::onExecute()
  {
    assert(document_ != nullptr);
    document_->timelineRef().updateCurrentTime();

    auto root = document_->getDirtyRootTextOrElement();
    auto scene = document_->scene;
    if (TR_UNLIKELY(scene == nullptr))
      return;

    const auto &clientEnv = TrClientContextPerProcess::GetEnvironmentRef();
    auto layoutView = document_->layoutView();
    if (root != nullptr)
    {
      // Compute each element's styles.
      auto adoptStyleForElement = [this](shared_ptr<HTMLElement> element)
      {
        element->adoptStyle(document_->defaultView()->getComputedStyle(element));
        return true;
      };
      auto adoptStyleForText = [this](shared_ptr<Text> textNode)
      {
        textNode->adoptStyle(document_->defaultView()->getComputedStyle(textNode));
      };
      traverseElementOrTextNode(root, adoptStyleForElement, adoptStyleForText, TreverseOrder::PreOrder);

      // Compute the layout from the root element.
      // TODO(yorkie): compute the layout from the root?
      layoutView->computeLayout(targetSpace());
      layoutView->debugPrint("After layout",
                             LayoutView::DebugOptions::Default()
                               .withFormattingContext(clientEnv.debugLayoutFormattingContext)
                               .withDisabled(clientEnv.debugLayoutTree == false));
    }

    // Visit the layout view to render CSS boxes.
    if (root != nullptr || isScrolling())
    {
      // TODO(yorkie): visit the tree from the root?
      LayoutViewVisitor::visit(*layoutView);
    }

    // Do hit test and dispatch the related events.
    DocumentEventDispatcher::hitTestAndDispatchEvents();
  }

  bool RenderHTMLDocument::onVisitObject(LayoutObject &object, int depth)
  {
    if (object.isNone()) // Skip the object for "display: none".
      return false;

    object.maybeAdjustSize();
    return true;
  }

  void RenderHTMLDocument::onVisitBox(const LayoutBoxModelObject &box, int depth)
  {
    if (TR_LIKELY(box.hasEntity()))
    {
      LayoutObject::FragmentDifference diff;
      auto &fragment = box.computeOrGetFragment(diff);
      if (diff.isChanged())
        renderEntity(box.entity(), fragment);
    }
  }

  void RenderHTMLDocument::onVisitText(const LayoutText &text, int depth)
  {
    if (TR_LIKELY(text.hasEntity()))
    {
      LayoutObject::FragmentDifference diff;
      auto &fragment = text.computeOrGetFragment(diff);
      if (diff.isChanged())
        renderEntity(text.entity(), fragment);
    }
  }

  void RenderHTMLDocument::renderEntity(const ecs::EntityId &entity,
                                        const Fragment &fragment)
  {
    auto scene = document_->scene;
    assert(scene != nullptr && "The scene is not set when rendering the entity.");

    shared_ptr<WebContent> webContent = document_->scene->getComponent<WebContent>(entity);
    if (webContent != nullptr)
      webContent->setFragment(fragment);

    auto boundingBox = scene->getComponent<BoundingBox>(entity);
    if (boundingBox != nullptr)
      boundingBox->updateSize(fragment.contentSize());

    // Update transform
    auto transform = scene->getComponent<Transform>(entity);
    if (transform != nullptr)
    {
      transform->setScale({client_cssom::pixelToMeter(boundingBox->width()),
                           client_cssom::pixelToMeter(boundingBox->height()),
                           0.001f});

      float left = fragment.left(); // Get the left position.
      float top = fragment.top();   // Get the top position.
      if (!scene->hasComponent<hierarchy::Root>(entity))
      {
        auto parentComponent = scene->getComponent<hierarchy::Parent>(entity);
        auto rootBoundingBox = document_->visualBoundingBox();
        if (TR_LIKELY(rootBoundingBox.has_value()))
        {
          /**
           * Transform the xyz() in LTW(Left-Top) space to the left-handed world space.
           *
           * First, calculate the distance from the root bounding box to the current bounding box:
           *
           * ```
           * var distance = (root - box) / 2
           * ```
           *
           * Then move the origin to the left-top-center of the root bounding box:
           *
           * ```
           * var origin = distance * (-1, 1, 1)
           * ```
           *
           * 3D space uses right(+x) and up(+y), thus if we wanna move the origin to the left and top, we need to multiply the
           * y-axis by -1 only.
           *
           * Finally, calculate the offset in world space to make the final translation:
           *
           * ```
           * var offset = origin + layout.xyz() * (1, -1, 1)
           * ```
           *
           * Note that there is a transformation are required to convert the layout space, namely right(+x) and up(-y), to the
           * world space, namely right(+x) and up(+y).
           */
          auto origin = (*rootBoundingBox - *boundingBox) / 2.0f * glm::vec3(-1.0f, 1.0f, 1.0f);
          auto offset = origin + fragment.xyz() * glm::vec3(1.0f, -1.0f, 1.0f);
          left = offset.x;
          top = offset.y;
        }
      }
      transform->setTranslation({client_cssom::pixelToMeter(left),
                                 client_cssom::pixelToMeter(top),
                                 fragment.z()});
    }

    // Update custom material?
    // TODO(yorkie): support custom material.
  }

  void RenderHTMLDocument::traverseElementOrTextNode(shared_ptr<Node> elementOrTextNode,
                                                     function<bool(shared_ptr<HTMLElement>)> elementCallback,
                                                     function<void(shared_ptr<Text>)> textNodeCallback,
                                                     TreverseOrder order)
  {
    if (TR_UNLIKELY(elementOrTextNode == nullptr) || !elementOrTextNode->connected)
      return;

    if (elementOrTextNode->isText())
    {
      auto textNode = static_pointer_cast<Text>(elementOrTextNode);
      if (textNode != nullptr)
        textNodeCallback(textNode);
      return;
    }

    if (elementOrTextNode->isHTMLElement())
    {
      auto element = static_pointer_cast<HTMLElement>(elementOrTextNode);
      if (element == nullptr)
        return;

      bool shouldContinue = true;
      if (order == TreverseOrder::PreOrder)
      {
        if (!elementCallback(element)) // If the element callback returns false, stop traversing in pre-order.
          shouldContinue = false;
      }

      if (shouldContinue)
      {
        for (auto childNode : element->childNodes)
          traverseElementOrTextNode(childNode, elementCallback, textNodeCallback, order);
        if (order == TreverseOrder::PostOrder)
          elementCallback(element);
      }
    }
  }
}
