#include <string>
#include <client/dom/events/error_event.hpp>

using namespace std;

namespace dom::events
{
  ErrorEvent::ErrorEvent(const string &type, const ErrorEvent::Options &options)
      : dom::Event(DOMEventConstructorType::kErrorEvent, DOMEventType::Error)
      , message_(options.message)
      , filename_(options.filename)
      , lineno_(options.lineno)
      , colno_(options.colno)
  {
  }

  string ErrorEvent::message() const
  {
    return message_;
  }

  string ErrorEvent::filename() const
  {
    return filename_;
  }

  int ErrorEvent::lineno() const
  {
    return lineno_;
  }

  int ErrorEvent::colno() const
  {
    return colno_;
  }
}
