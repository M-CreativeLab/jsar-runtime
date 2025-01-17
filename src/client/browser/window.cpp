#include "./window.hpp"

namespace browser
{
  Window::Window(TrClientContextPerProcess *clientContext)
      : dom::DOMEventTarget(),
        clientContext_(clientContext)
  {
    assert(clientContext_ != nullptr);
  }
}
