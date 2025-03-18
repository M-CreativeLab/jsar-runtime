#include "binding.hpp"
#include "./event.hpp"
#include "./node-inl.hpp"
#include "./character_data-inl.hpp"
#include "./text.hpp"
#include "./element-inl.hpp"
#include "./all_html_elements.hpp"
#include "./document-inl.hpp"
#include "./document_fragment.hpp"
#include "./dom_parser.hpp"
#include "./browsing_context.hpp"
#include "./worker_context.hpp"
#include "./mutation_record.hpp"
#include "./mutation_observer.hpp"

namespace bindings
{
  namespace dom
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      // Add events
      dombinding::InitEvents(env);

      // Add nodes & elements
      dombinding::Node::Init(env);
      dombinding::Element::Init(env);
      dombinding::HTMLElement::Init(env);
      dombinding::HTMLMediaElement::Init(env);
#define XX(tagNameStr, className) dombinding::className::Init(env);
      TYPED_ELEMENT_MAP(XX)
#undef XX
      dombinding::CharacterData::Init(env);
      dombinding::Text::Init(env);
      dombinding::DocumentFragment::Init(env);

      // Add documents
      dombinding::Document::Init(env, exports);
      dombinding::XMLDocument::Init(env, exports);

      // Add Web APIs
      dombinding::MutationRecord::Init(env);
      dombinding::MutationObserver::Init(env);

      // Add others
      dombinding::DOMParser::Init(env);
      dombinding::BrowsingContext::Init(env, exports);
      dombinding::WorkerContext::Init(env, exports);
      return exports;
    }
  }
}
