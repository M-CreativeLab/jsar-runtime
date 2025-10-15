#include <iostream>
#include "./binding.hpp"
#include "./event.hpp"
#include "./event_target.hpp"
#include "./navigator.hpp"
#include "./location.hpp"
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

// CSSOM
#include "./cssom/css_style_declaration.hpp"

// HTML element bindings
#include "./html/html_element.hpp"
#include "./html/html_audio_element.hpp"
#include "./html/html_body_element.hpp"
#include "./html/html_button_element.hpp"
#include "./html/html_canvas_element.hpp"
#include "./html/html_div_element.hpp"
#include "./html/html_head_element.hpp"
#include "./html/html_heading_element.hpp"
#include "./html/html_html_element.hpp"
#include "./html/html_iframe_element.hpp"
#include "./html/html_image_element.hpp"
#include "./html/html_input_element.hpp"
#include "./html/html_link_element.hpp"
#include "./html/html_media_element.hpp"
#include "./html/html_meta_element.hpp"
#include "./html/html_model_element.hpp"
#include "./html/html_paragraph_element.hpp"
#include "./html/html_script_element.hpp"
#include "./html/html_section_element.hpp"
#include "./html/html_span_element.hpp"
#include "./html/html_style_element.hpp"
#include "./html/html_template_element.hpp"
#include "./html/html_video_element.hpp"

// Canvas bindings
#include "./canvas/image_bitmap.hpp"
#include "./canvas/image_data.hpp"
#include "./canvas/canvas_rendering_context_2d.hpp"
#include "./canvas/canvas.hpp"
#include "./canvas/offscreen_canvas_rendering_context_2d.hpp"
#include "./canvas/path2d.hpp"

// WebGL bindings
#include "./webgl/active_info.hpp"
#include "./webgl/buffer.hpp"
#include "./webgl/framebuffer.hpp"
#include "./webgl/program.hpp"
#include "./webgl/renderbuffer.hpp"
#include "./webgl/shader.hpp"
#include "./webgl/texture.hpp"
#include "./webgl/uniform_location.hpp"
#include "./webgl/vertex_array.hpp"
#include "./webgl/webgl_rendering_context.hpp"
#include "./webgl/webgl2_rendering_context.hpp"

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

// Workers bindings
#include "./workers/worker.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  void Initialize(Isolate *isolate, Local<Context> context, ContextType type)
  {
    HandleScope scope(isolate);
    Context::Scope contextScope(context);
    Local<Object> global = context->Global();

#define NAME(str) String::NewFromUtf8(isolate, str).ToLocalChecked()

    // Initialize base classes first
#define ADD_BASE_TYPE(X) \
  global->Set(context, NAME(#X), script_bindings::X::Initialize(isolate)).Check();

    ADD_BASE_TYPE(Event)
    ADD_BASE_TYPE(EventTarget)
#undef ADD_BASE_TYPE

    // Initialize event classes
#define ADD_EVENT(X) \
  global->Set(context, NAME(#X), event_bindings::X::Initialize(isolate)).Check();

    ADD_EVENT(MessageEvent)
    ADD_EVENT(UIEvent)
    ADD_EVENT(MouseEvent)
    ADD_EVENT(PointerEvent)

    // WebXR events
    ADD_EVENT(XRSessionEvent)
    ADD_EVENT(XRInputSourceEvent)
    ADD_EVENT(XRInputSourcesChangeEvent)
#undef ADD_EVENT

    {
      // Create and set global console object
      auto Console = dom_bindings::Console::Initialize(isolate);
      auto console = dom_bindings::Console::NewInstance(isolate, nullptr);
      global->Set(context, NAME("Console"), Console).Check();
      global->Set(context, NAME("console"), console).Check();
    }

#define ADD_CANVAS_TYPE(X) \
  global->Set(context, NAME(#X), canvas_bindings::X::Initialize(isolate)).Check();

    ADD_CANVAS_TYPE(ImageBitmap)
    ADD_CANVAS_TYPE(ImageData)
    ADD_CANVAS_TYPE(CanvasRenderingContext2D)
    ADD_CANVAS_TYPE(OffscreenCanvas)
    ADD_CANVAS_TYPE(OffscreenCanvasRenderingContext2D)
    ADD_CANVAS_TYPE(Path2D)

#undef ADD_CANVAS_TYPE

    // Initialize Worker class
#define ADD_WORKER_TYPE(X) \
  global->Set(context, NAME(#X), workers_bindings::X::Initialize(isolate)).Check();

    ADD_WORKER_TYPE(Worker)
#undef ADD_WORKER_TYPE

    // Initialize classes and objects for scripting context only
    if (type == ContextType::kScripting)
    {
      // Browser/Navigator object
      auto Navigator = Navigator::Initialize(isolate);
      global->Set(context, NAME("Navigator"), Navigator).Check();
      auto Location = Location::Initialize(isolate);
      global->Set(context, NAME("Location"), Location).Check();

      // Base DOM classes
#define ADD_DOM_TYPE(X) \
  global->Set(context, NAME(#X), dom_bindings::X::Initialize(isolate)).Check();

      ADD_DOM_TYPE(Node)
      ADD_DOM_TYPE(NodeList)
      ADD_DOM_TYPE(Element)
      ADD_DOM_TYPE(Document)
      ADD_DOM_TYPE(DocumentFragment)
      ADD_DOM_TYPE(CharacterData)
      ADD_DOM_TYPE(Text)
      ADD_DOM_TYPE(Comment)
      ADD_DOM_TYPE(DOMParser)
      ADD_DOM_TYPE(DOMStringMap)
      ADD_DOM_TYPE(MutationObserver)
      ADD_DOM_TYPE(MutationRecord)
#undef ADD_DOM_TYPE

      // CSSOM
#define ADD_CSSOM_TYPE(X) \
  global->Set(context, NAME(#X), cssom_bindings::X::Initialize(isolate)).Check();

      ADD_CSSOM_TYPE(CSSStyleDeclaration)
#undef ADD_CSSOM_TYPE

      // HTML elements
#define ADD_HTML_ELEMENT(X) \
  global->Set(context, NAME(#X), html_bindings::X::Initialize(isolate)).Check();

      ADD_HTML_ELEMENT(HTMLElement)
      ADD_HTML_ELEMENT(HTMLAudioElement)
      ADD_HTML_ELEMENT(HTMLBodyElement)
      ADD_HTML_ELEMENT(HTMLButtonElement)
      ADD_HTML_ELEMENT(HTMLCanvasElement)
      ADD_HTML_ELEMENT(HTMLDivElement)
      ADD_HTML_ELEMENT(HTMLHeadElement)
      ADD_HTML_ELEMENT(HTMLHeadingElement)
      ADD_HTML_ELEMENT(HTMLHtmlElement)
      ADD_HTML_ELEMENT(HTMLIframeElement)
      ADD_HTML_ELEMENT(HTMLImageElement)
      ADD_HTML_ELEMENT(HTMLInputElement)
      ADD_HTML_ELEMENT(HTMLLinkElement)
      ADD_HTML_ELEMENT(HTMLMediaElement)
      ADD_HTML_ELEMENT(HTMLMetaElement)
      ADD_HTML_ELEMENT(HTMLModelElement)
      ADD_HTML_ELEMENT(HTMLParagraphElement)
      ADD_HTML_ELEMENT(HTMLScriptElement)
      ADD_HTML_ELEMENT(HTMLSpanElement)
      ADD_HTML_ELEMENT(HTMLStyleElement)
      ADD_HTML_ELEMENT(HTMLTemplateElement)
      ADD_HTML_ELEMENT(HTMLVideoElement)
#undef ADD_HTML_ELEMENT
      {
        using namespace html_bindings;

        // Set up constructor as a global function: Audio, Image, etc.
        global->Set(context, NAME("Audio"), HTMLAudioElement::CreateAudioConstructor(isolate)).Check();
        global->Set(context, NAME("Image"), HTMLImageElement::CreateImageConstructor(isolate)).Check();

        // Initialize HTML elements which won't be exposed
        HTMLSectionElement::Initialize(isolate);
      }

      // WebGL classes
#define ADD_WEBGL_TYPE(X) \
  global->Set(context, NAME(#X), webgl_bindings::X::Initialize(isolate)).Check();

      ADD_WEBGL_TYPE(WebGLActiveInfo)
      ADD_WEBGL_TYPE(WebGLBuffer)
      ADD_WEBGL_TYPE(WebGLFramebuffer)
      ADD_WEBGL_TYPE(WebGLProgram)
      ADD_WEBGL_TYPE(WebGLRenderbuffer)
      ADD_WEBGL_TYPE(WebGLShader)
      ADD_WEBGL_TYPE(WebGLShaderPrecisionFormat)
      ADD_WEBGL_TYPE(WebGLTexture)
      ADD_WEBGL_TYPE(WebGLUniformLocation)
      ADD_WEBGL_TYPE(WebGLVertexArray)
      ADD_WEBGL_TYPE(WebGLRenderingContext)
      ADD_WEBGL_TYPE(WebGL2RenderingContext)
#undef ADD_WEBGL_TYPE

      // WebXR classes
      // Initialize WebXR classes
#define ADD_WEBXR_TYPE(X) \
  global->Set(context, NAME(#X), webxr_bindings::X::Initialize(isolate)).Check();

      ADD_WEBXR_TYPE(XRSpace)
      ADD_WEBXR_TYPE(XRReferenceSpace)
      ADD_WEBXR_TYPE(XRSession)
      ADD_WEBXR_TYPE(XRFrame)
      ADD_WEBXR_TYPE(XRRigidTransform)
      ADD_WEBXR_TYPE(XRPose)
      ADD_WEBXR_TYPE(XRViewerPose)
      ADD_WEBXR_TYPE(XRViewport)
      ADD_WEBXR_TYPE(XRView)
      ADD_WEBXR_TYPE(XRSystem)
      ADD_WEBXR_TYPE(XRRenderState)
      ADD_WEBXR_TYPE(XRLayer)
      ADD_WEBXR_TYPE(XRWebGLLayer)
      ADD_WEBXR_TYPE(XRInputSource)
      ADD_WEBXR_TYPE(XRHand)
#undef ADD_WEBXR_TYPE
    }

#undef NAME
  }
}
