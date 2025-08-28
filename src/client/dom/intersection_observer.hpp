#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <functional>
#include <shared_mutex>
#include <chrono>

#include "./node.hpp"
#include "./element.hpp"
#include "./geometry/dom_rect.hpp"

namespace dom
{
  class IntersectionObserverEntry;
  class IntersectionObserver;
  typedef std::function<void(const std::vector<const IntersectionObserverEntry> &, IntersectionObserver &)> IntersectionCallback;

  class IntersectionObserverEntry
  {
    friend class IntersectionObserver;

  public:
    /**
     * Create a new IntersectionObserverEntry.
     */
    IntersectionObserverEntry(std::shared_ptr<Element> target,
                              const geometry::DOMRect &boundingClientRect,
                              const geometry::DOMRect &intersectionRect,
                              const std::optional<geometry::DOMRect> &rootBounds,
                              double intersectionRatio,
                              bool isIntersecting,
                              double time);

  public:
    friend std::ostream &operator<<(std::ostream &os, const IntersectionObserverEntry &entry)
    {
      os << "IntersectionObserverEntry { "
         << "target: " << (entry.target.lock() ? entry.target.lock()->nodeName : "null")
         << ", intersectionRatio: " << entry.intersectionRatio
         << ", isIntersecting: " << (entry.isIntersecting ? "true" : "false")
         << ", time: " << entry.time
         << " }";
      return os;
    }

  public:
    std::weak_ptr<Element> target;
    geometry::DOMRect boundingClientRect;
    geometry::DOMRect intersectionRect;
    std::optional<geometry::DOMRect> rootBounds;
    double intersectionRatio = 0.0;
    bool isIntersecting = false;
    double time = 0.0;
  };

  class IntersectionObserver : public std::enable_shared_from_this<IntersectionObserver>
  {
  public:
    struct ObserveOptions
    {
      std::shared_ptr<Element> root = nullptr;
      std::string rootMargin = "0px";
      std::vector<double> threshold = {0.0};
    };

  public:
    IntersectionObserver(IntersectionCallback callback = nullptr, const ObserveOptions &options = ObserveOptions{});
    ~IntersectionObserver();

  public:
    void disconnect();
    void observe(std::shared_ptr<Element> target);
    void unobserve(std::shared_ptr<Element> target);
    std::vector<IntersectionObserverEntry> takeRecords();

  public:
    /**
     * Get the root element for intersection calculations.
     */
    std::shared_ptr<Element> getRoot() const;

    /**
     * Get the root margin.
     */
    const std::string &getRootMargin() const;

    /**
     * Get the threshold values.
     */
    const std::vector<double> &getThreshold() const;

  private:
    /**
     * Check intersection for a target element.
     */
    void checkIntersection(std::shared_ptr<Element> target);

    /**
     * Calculate intersection between two rectangles.
     */
    geometry::DOMRect calculateIntersection(const geometry::DOMRect &rect1, const geometry::DOMRect &rect2);

    /**
     * Calculate intersection ratio.
     */
    double calculateIntersectionRatio(const geometry::DOMRect &targetRect, const geometry::DOMRect &intersectionRect);

    /**
     * Get the root bounds for intersection calculations.
     */
    std::optional<geometry::DOMRect> getRootBounds();

    /**
     * Parse root margin string and apply to bounds.
     */
    geometry::DOMRect applyRootMargin(const geometry::DOMRect &bounds);

    /**
     * Queue a new intersection entry.
     */
    bool queueEntry(const IntersectionObserverEntry &entry);

    /**
     * Get current time in milliseconds since page load.
     */
    double getCurrentTime();

  private:
    IntersectionCallback callback_;
    ObserveOptions options_;
    std::vector<std::shared_ptr<Element>> observedTargets_;
    std::vector<IntersectionObserverEntry> entriesQueue_;
    std::shared_mutex entriesMutex_;
    std::shared_mutex targetsMutex_;
  };
}