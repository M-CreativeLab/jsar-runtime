#include <client/builtin_scene/scene.hpp>
#include <client/layout/hit_test_ray.hpp>
#include <client/layout/hit_test_result.hpp>

#include "./document-inl.hpp"
#include "./document_renderer.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;

  RenderHTMLDocument::RenderHTMLDocument(HTMLDocument *document)
      : ecs::System(),
        document_(document)
  {
    assert(document_ != nullptr);
  }

  void RenderHTMLDocument::onExecute()
  {
    assert(document_ != nullptr);
    auto body = document_->body();
    auto scene = document_->scene;
    if (scene == nullptr || body == nullptr)
      return;

    // Step 1: Compute each element's styles.
    {
      auto adoptStyleForElement = [this](shared_ptr<HTMLElement> element)
      {
        const auto &computedStyle = document_->defaultView()->getComputedStyle(element);
        element->adoptStyle(computedStyle);
        return true;
      };
      auto adoptStyleForText = [](shared_ptr<Text> textNode)
      {
        textNode->adoptStyle(*textNode->style_);
      };
      traverseElementOrTextNode(body, adoptStyleForElement, adoptStyleForText, TreverseOrder::PreOrder);
    }

    // Step 2: Compute the layout from the root element.
    auto layoutView = document_->layoutView();
    layoutView->computeLayout(targetSpace());
    layoutView->debugPrint("After layout", client_layout::LayoutView::DebugOptions::Default()
                                               .withFormattingContext(true)
                                               .withDisabled());

    // Step 3: Visit the layout view to render CSS boxes.
    client_layout::LayoutViewVisitor::visit(*layoutView);

    // Step 4: Do hit test and dispatch the related events.
    hitTestAndDispatchEvents();
  }

  bool RenderHTMLDocument::onVisitObject(client_layout::LayoutObject &object, int depth)
  {
    if (object.isNone()) // Skip the object for "display: none".
      return false;

    object.maybeAdjustSize();
    return true;
  }

  void RenderHTMLDocument::onVisitBox(const client_layout::LayoutBoxModelObject &box, int depth)
  {
    if (TR_LIKELY(box.hasEntity()))
    {
      renderEntity(box.entity(), box.fragment());
    }
  }

  void RenderHTMLDocument::onVisitText(const client_layout::LayoutText &text, int depth)
  {
    if (TR_LIKELY(text.hasEntity()))
    {
      renderEntity(text.entity(), text.fragment());
    }
  }

  void RenderHTMLDocument::renderEntity(const ecs::EntityId &entity, const client_layout::Fragment &fragment)
  {
    auto scene = document_->scene;
    assert(scene != nullptr && "The scene is not set when rendering the entity.");

    shared_ptr<WebContent> webContent = document_->scene->getComponent<WebContent>(entity);
    if (webContent != nullptr)
      webContent->setFragment(fragment);

    auto boundingBox = scene->getComponent<BoundingBox>(entity);
    if (boundingBox != nullptr)
      boundingBox->updateSize(fragment.width(), fragment.height(), fragment.depth());

    // Update transform
    auto transform = scene->getComponent<Transform>(entity);
    if (transform != nullptr)
    {
      transform->setScale({client_cssom::pixelToMeter(boundingBox->width()),
                           client_cssom::pixelToMeter(boundingBox->height()),
                           1.0f});

      float left = fragment.left(); // Get the left position.
      float top = fragment.top();   // Get the top position.
      if (!scene->hasComponent<hierarchy::Root>(entity))
      {
        auto parentComponent = scene->getComponent<hierarchy::Parent>(entity);
        auto rootBoundingBox = scene->getComponent<BoundingBox>(parentComponent->root());
        if (TR_LIKELY(rootBoundingBox != nullptr))
        {
          /**
           * Transform the xyz() in LTW space to the left-handed world space.
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

  bool RenderHTMLDocument::hitTestAndDispatchEvents()
  {
    auto ray = document_->scene->selectRayForHitTesting();
    if (!ray.has_value())
      return false;

    client_layout::HitTestResult r;
    client_layout::HitTestRay hitTestRay(ray.value());

    if (document_->layoutView()->hitTest(hitTestRay, r) &&
        r.innerNode() != nullptr)
    {
      if (r.innerNode()->isElement())
      {
        auto &hitElement = Node::AsChecked<Element>(r.innerNode());
        hitElement.setAttribute("style", "background-color: red;");
        // TODO(yorkie): dispatch the hit events.
      }
      return true;
    }
    return false;
  }

  void RenderHTMLDocument::traverseElementOrTextNode(shared_ptr<Node> elementOrTextNode,
                                                     function<bool(shared_ptr<HTMLElement>)> elementCallback,
                                                     function<void(shared_ptr<Text>)> textNodeCallback,
                                                     TreverseOrder order)
  {
    if (TR_UNLIKELY(elementOrTextNode == nullptr) || !elementOrTextNode->connected)
      return;

    if (elementOrTextNode->nodeType == NodeType::TEXT_NODE)
    {
      auto textNode = dynamic_pointer_cast<Text>(elementOrTextNode);
      if (textNode != nullptr)
        textNodeCallback(textNode);
      return;
    }

    if (elementOrTextNode->nodeType == NodeType::ELEMENT_NODE)
    {
      auto element = dynamic_pointer_cast<HTMLElement>(elementOrTextNode);
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
