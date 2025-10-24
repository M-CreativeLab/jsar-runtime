#pragma once

#include <string>
#include <client/dom/dom_event.hpp>
#include <client/dom/dom_event_target.hpp>

namespace endor
{
  namespace dom::events
  {
    class ErrorEvent : public dom::Event
    {
      using dom::Event::Event;

    public:
      class Options
      {
      public:
        std::string message;
        std::string filename;
        int lineno = 0;
        int colno = 0;
      };

      ErrorEvent(const std::string &type, const Options &options);
      bool isErrorEvent() const override final
      {
        return true;
      }

    public:
      std::string message() const;
      std::string filename() const;
      int lineno() const;
      int colno() const;

    private:
      std::string message_;
      std::string filename_;
      int lineno_ = 0;
      int colno_ = 0;
    };
  }
} // namespace endor
