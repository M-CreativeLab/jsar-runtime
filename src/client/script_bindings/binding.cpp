#include <iostream>
#include "./binding.hpp"
#include "./event.hpp"
#include "./event_target.hpp"
#include "./navigator.hpp"
#include "./events/all_events.hpp"
#include "./dom/node.hpp"
#include "./dom/element.hpp"
#include "./dom/document.hpp"
#include "./dom/character_data.hpp"
#include "./dom/text.hpp"
#include "./dom/console.hpp"
#include "./dom/comment.hpp"
#include "./dom/document_fragment.hpp"
#include "./dom/dom_parser.hpp"
#include "./dom/dom_string_map.hpp"
#include "./dom/mutation_observer.hpp"
#include "./dom/mutation_record.hpp"
#include "./dom/node_list.hpp"

// HTML element bindings
#include "./html/html_element.hpp"
#include "./html/html_div_element.hpp"
#include "./html/html_audio_element.hpp"
#include "./html/html_canvas_element.hpp"

// WebXR bindings
#include "./webxr/xr_space.hpp"
#include "./webxr/xr_session.hpp"
#include "./webxr/xr_frame.hpp"
#include "./webxr/xr_rigid_transform.hpp"
#include "./webxr/xr_pose.hpp"
#include "./webxr/xr_viewport.hpp"
#include "./webxr/xr_view.hpp"
#include "./webxr/xr_system.hpp"
#include "./webxr/xr_render_state.hpp"
#include "./webxr/xr_layer.hpp"
#include "./webxr/xr_input_source.hpp"
#include "./webxr/xr_hand.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  void Initialize(Isolate *isolate, Local<Context> context, ContextType type)
  {
    Context::Scope contextScope(context);
    HandleScope scope(isolate);
    Local<Object> global = context->Global();

#define STRING_FROM_UTF8(str) String::NewFromUtf8(isolate, str).ToLocalChecked()

    // Initialize base classes first
    auto Event = script_bindings::Event::Initialize(isolate);
    auto EventTarget = script_bindings::EventTarget::Initialize(isolate);
    global->Set(context, STRING_FROM_UTF8("Event"), Event).Check();
    global->Set(context, STRING_FROM_UTF8("EventTarget"), EventTarget).Check();

    // Initialize event classes
    auto UIEvent = event_bindings::UIEvent::Initialize(isolate);
    auto MouseEvent = event_bindings::MouseEvent::Initialize(isolate);
    auto PointerEvent = event_bindings::PointerEvent::Initialize(isolate);
    global->Set(context, STRING_FROM_UTF8("UIEvent"), UIEvent).Check();
    global->Set(context, STRING_FROM_UTF8("MouseEvent"), MouseEvent).Check();
    global->Set(context, STRING_FROM_UTF8("PointerEvent"), PointerEvent).Check();

    {
      // WebXR event classes
      auto XRSessionEvent = event_bindings::XRSessionEvent::Initialize(isolate);
      auto XRInputSourceEvent = event_bindings::XRInputSourceEvent::Initialize(isolate);
      auto XRInputSourcesChangeEvent = event_bindings::XRInputSourcesChangeEvent::Initialize(isolate);
      global->Set(context, STRING_FROM_UTF8("XRSessionEvent"), XRSessionEvent).Check();
      global->Set(context, STRING_FROM_UTF8("XRInputSourceEvent"), XRInputSourceEvent).Check();
      global->Set(context, STRING_FROM_UTF8("XRInputSourcesChangeEvent"), XRInputSourcesChangeEvent).Check();
    }

    // Create and set global console object
    auto Console = dom_bindings::Console::Initialize(isolate);
    auto console = dom_bindings::Console::CreateConsoleObject(isolate);
    global->Set(context, STRING_FROM_UTF8("Console"), Console).Check();
    global->Set(context, STRING_FROM_UTF8("console"), console).Check();

    // Initialize classes and objects for scripting context only
    if (type == ContextType::kScripting)
    {
      // Browser/Navigator object
      auto Navigator = Navigator::Initialize(isolate);
      global->Set(context, STRING_FROM_UTF8("Navigator"), Navigator).Check();
      global->Set(context,
                  STRING_FROM_UTF8("navigator"),
                  Navigator::NewInstance(isolate, make_shared<browser::Navigator>()))
        .Check();
      cout << "Navigator object initialized." << endl;

      // Base DOM classes
      auto Node = dom_bindings::Node::Initialize(isolate);
      auto Element = dom_bindings::Element::Initialize(isolate);
      auto Document = dom_bindings::Document::Initialize(isolate);
      auto CharacterData = dom_bindings::CharacterData::Initialize(isolate);
      auto Text = dom_bindings::Text::Initialize(isolate);
      auto Comment = dom_bindings::Comment::Initialize(isolate);
      auto DocumentFragment = dom_bindings::DocumentFragment::Initialize(isolate);
      auto DOMParser = dom_bindings::DOMParser::Initialize(isolate);
      auto DOMStringMap = dom_bindings::DOMStringMap::Initialize(isolate);
      auto MutationObserver = dom_bindings::MutationObserver::Initialize(isolate);
      auto MutationRecord = dom_bindings::MutationRecord::Initialize(isolate);
      auto NodeList = dom_bindings::NodeList::Initialize(isolate);
      {
        global->Set(context, STRING_FROM_UTF8("Node"), Node).Check();
        global->Set(context, STRING_FROM_UTF8("Element"), Element).Check();
        global->Set(context, STRING_FROM_UTF8("Document"), Document).Check();
        global->Set(context, STRING_FROM_UTF8("CharacterData"), CharacterData).Check();
        global->Set(context, STRING_FROM_UTF8("Text"), Text).Check();
        global->Set(context, STRING_FROM_UTF8("Comment"), Comment).Check();
        global->Set(context, STRING_FROM_UTF8("DocumentFragment"), DocumentFragment).Check();
        global->Set(context, STRING_FROM_UTF8("DOMParser"), DOMParser).Check();
        global->Set(context, STRING_FROM_UTF8("DOMStringMap"), DOMStringMap).Check();
        global->Set(context, STRING_FROM_UTF8("MutationObserver"), MutationObserver).Check();
        global->Set(context, STRING_FROM_UTF8("MutationRecord"), MutationRecord).Check();
        global->Set(context, STRING_FROM_UTF8("NodeList"), NodeList).Check();
      }

      // HTML elements
      auto HTMLElement = html_bindings::HTMLElement::Initialize(isolate);
      auto HTMLDivElement = html_bindings::HTMLDivElement::Initialize(isolate);
      auto HTMLAudioElement = html_bindings::HTMLAudioElement::Initialize(isolate);
      auto HTMLCanvasElement = html_bindings::HTMLCanvasElement::Initialize(isolate);
      {
        global->Set(context, STRING_FROM_UTF8("HTMLElement"), HTMLElement).Check();
        global->Set(context, STRING_FROM_UTF8("HTMLDivElement"), HTMLDivElement).Check();
        global->Set(context, STRING_FROM_UTF8("HTMLAudioElement"), HTMLAudioElement).Check();
        global->Set(context, STRING_FROM_UTF8("HTMLCanvasElement"), HTMLCanvasElement).Check();

        // Set up Audio constructor as a global function
        global->Set(context,
                    STRING_FROM_UTF8("Audio"),
                    FunctionTemplate::New(isolate, html_bindings::HTMLAudioElement::AudioConstructor)->GetFunction(context).ToLocalChecked())
          .Check();
      }

      // WebXR classes
      // Initialize WebXR classes
      auto XRSpace = webxr_bindings::XRSpace::Initialize(isolate);
      auto XRReferenceSpace = webxr_bindings::XRReferenceSpace::Initialize(isolate);
      auto XRSession = webxr_bindings::XRSession::Initialize(isolate);
      auto XRFrame = webxr_bindings::XRFrame::Initialize(isolate);
      auto XRRigidTransform = webxr_bindings::XRRigidTransform::Initialize(isolate);
      auto XRPose = webxr_bindings::XRPose::Initialize(isolate);
      auto XRViewerPose = webxr_bindings::XRViewerPose::Initialize(isolate);
      auto XRViewport = webxr_bindings::XRViewport::Initialize(isolate);
      auto XRView = webxr_bindings::XRView::Initialize(isolate);
      auto XRSystem = webxr_bindings::XRSystem::Initialize(isolate);
      auto XRRenderState = webxr_bindings::XRRenderState::Initialize(isolate);
      auto XRLayer = webxr_bindings::XRLayer::Initialize(isolate);
      auto XRWebGLLayer = webxr_bindings::XRWebGLLayer::Initialize(isolate);
      auto XRInputSource = webxr_bindings::XRInputSource::Initialize(isolate);
      auto XRHand = webxr_bindings::XRHand::Initialize(isolate);
      {
        global->Set(context, STRING_FROM_UTF8("XRSpace"), XRSpace).Check();
        global->Set(context, STRING_FROM_UTF8("XRReferenceSpace"), XRReferenceSpace).Check();
        global->Set(context, STRING_FROM_UTF8("XRSession"), XRSession).Check();
        global->Set(context, STRING_FROM_UTF8("XRFrame"), XRFrame).Check();
        global->Set(context, STRING_FROM_UTF8("XRRigidTransform"), XRRigidTransform).Check();
        global->Set(context, STRING_FROM_UTF8("XRPose"), XRPose).Check();
        global->Set(context, STRING_FROM_UTF8("XRViewerPose"), XRViewerPose).Check();
        global->Set(context, STRING_FROM_UTF8("XRViewport"), XRViewport).Check();
        global->Set(context, STRING_FROM_UTF8("XRView"), XRView).Check();
        global->Set(context, STRING_FROM_UTF8("XRSystem"), XRSystem).Check();
        global->Set(context, STRING_FROM_UTF8("XRRenderState"), XRRenderState).Check();
        global->Set(context, STRING_FROM_UTF8("XRLayer"), XRLayer).Check();
        global->Set(context, STRING_FROM_UTF8("XRWebGLLayer"), XRWebGLLayer).Check();
        global->Set(context, STRING_FROM_UTF8("XRInputSource"), XRInputSource).Check();
        global->Set(context, STRING_FROM_UTF8("XRHand"), XRHand).Check();
      }
    }

#undef STRING_FROM_UTF8
  }
}
