#include "./intersection_observer.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <regex>
#include <common/utility.hpp>

namespace dom
{
  using namespace std;

  IntersectionObserverEntry::IntersectionObserverEntry(shared_ptr<Element> target,
                                                       const geometry::DOMRect &boundingClientRect,
                                                       const geometry::DOMRect &intersectionRect,
                                                       const optional<geometry::DOMRect> &rootBounds,
                                                       double intersectionRatio,
                                                       bool isIntersecting,
                                                       double time)
      : target(target)
      , boundingClientRect(boundingClientRect)
      , intersectionRect(intersectionRect)
      , rootBounds(rootBounds)
      , intersectionRatio(intersectionRatio)
      , isIntersecting(isIntersecting)
      , time(time)
  {
  }

  IntersectionObserver::IntersectionObserver(IntersectionCallback callback, const ObserveOptions &options)
      : callback_(callback)
      , options_(options)
  {
  }

  IntersectionObserver::~IntersectionObserver()
  {
    disconnect();
  }

  void IntersectionObserver::disconnect()
  {
    lock_guard<shared_mutex> lock(targetsMutex_);
    observedTargets_.clear();
  }

  void IntersectionObserver::observe(shared_ptr<Element> target)
  {
    if (TR_UNLIKELY(target == nullptr))
      throw invalid_argument("target must not be null");

    {
      lock_guard<shared_mutex> lock(targetsMutex_);
      auto it = find(observedTargets_.begin(), observedTargets_.end(), target);
      if (it == observedTargets_.end())
      {
        observedTargets_.push_back(target);
      }
    }

    // Perform initial intersection check
    checkIntersection(target);
  }

  void IntersectionObserver::unobserve(shared_ptr<Element> target)
  {
    if (TR_UNLIKELY(target == nullptr))
      return;

    lock_guard<shared_mutex> lock(targetsMutex_);
    auto it = find(observedTargets_.begin(), observedTargets_.end(), target);
    if (it != observedTargets_.end())
    {
      observedTargets_.erase(it);
    }
  }

  vector<IntersectionObserverEntry> IntersectionObserver::takeRecords()
  {
    lock_guard<shared_mutex> lock(entriesMutex_);
    vector<IntersectionObserverEntry> records = move(entriesQueue_);
    entriesQueue_.clear();
    return records;
  }

  shared_ptr<Element> IntersectionObserver::getRoot() const
  {
    return options_.root;
  }

  const string &IntersectionObserver::getRootMargin() const
  {
    return options_.rootMargin;
  }

  const vector<double> &IntersectionObserver::getThreshold() const
  {
    return options_.threshold;
  }

  void IntersectionObserver::checkIntersection(shared_ptr<Element> target)
  {
    if (target == nullptr)
      return;

    // Get target bounding rect
    geometry::DOMRect targetRect = target->getBoundingClientRect();

    // Get root bounds
    optional<geometry::DOMRect> rootBounds = getRootBounds();
    geometry::DOMRect effectiveRootBounds;

    if (rootBounds.has_value())
    {
      effectiveRootBounds = applyRootMargin(rootBounds.value());
    }
    else
    {
      // Use viewport bounds when no root specified
      // For now, use a default viewport size - this should be obtained from the browser window
      effectiveRootBounds = geometry::DOMRect(0, 0, 1920, 1080);
      effectiveRootBounds = applyRootMargin(effectiveRootBounds);
    }

    // Calculate intersection
    geometry::DOMRect intersectionRect = calculateIntersection(targetRect, effectiveRootBounds);
    double intersectionRatio = calculateIntersectionRatio(targetRect, intersectionRect);

    // Check if intersection ratio crosses any threshold
    bool isIntersecting = false;
    for (double threshold : options_.threshold)
    {
      if (intersectionRatio >= threshold)
      {
        isIntersecting = true;
        break;
      }
    }

    // Create entry
    IntersectionObserverEntry entry(
      target,
      targetRect,
      intersectionRect,
      rootBounds,
      intersectionRatio,
      isIntersecting,
      getCurrentTime());

    queueEntry(entry);
  }

  geometry::DOMRect IntersectionObserver::calculateIntersection(const geometry::DOMRect &rect1, const geometry::DOMRect &rect2)
  {
    float left = max(rect1.x(), rect2.x());
    float top = max(rect1.y(), rect2.y());
    float right = min(rect1.x() + rect1.width(), rect2.x() + rect2.width());
    float bottom = min(rect1.y() + rect1.height(), rect2.y() + rect2.height());

    if (left >= right || top >= bottom)
    {
      // No intersection
      return geometry::DOMRect(0, 0, 0, 0);
    }

    return geometry::DOMRect(left, top, right - left, bottom - top);
  }

  double IntersectionObserver::calculateIntersectionRatio(const geometry::DOMRect &targetRect, const geometry::DOMRect &intersectionRect)
  {
    double targetArea = targetRect.width() * targetRect.height();
    if (targetArea <= 0)
      return 0.0;

    double intersectionArea = intersectionRect.width() * intersectionRect.height();
    return intersectionArea / targetArea;
  }

  optional<geometry::DOMRect> IntersectionObserver::getRootBounds()
  {
    if (options_.root != nullptr)
    {
      return options_.root->getBoundingClientRect();
    }
    return nullopt; // Indicates viewport should be used
  }

  geometry::DOMRect IntersectionObserver::applyRootMargin(const geometry::DOMRect &bounds)
  {
    // Parse rootMargin - for now, just support "0px" or simple pixel values
    // Full CSS margin parsing would be more complex
    geometry::DOMRect result = bounds;

    if (options_.rootMargin != "0px")
    {
      // Simple parsing for format like "10px" or "10px 20px 30px 40px"
      regex marginRegex(R"((-?\d+(?:\.\d+)?)px)");
      sregex_iterator begin(options_.rootMargin.begin(), options_.rootMargin.end(), marginRegex);
      sregex_iterator end;

      vector<double> margins;
      for (sregex_iterator i = begin; i != end; ++i)
      {
        smatch match = *i;
        margins.push_back(stod(match.str(1)));
      }

      if (margins.size() == 1)
      {
        // All sides same
        result.x() -= margins[0];
        result.y() -= margins[0];
        result.width() += 2 * margins[0];
        result.height() += 2 * margins[0];
      }
      else if (margins.size() == 4)
      {
        // top, right, bottom, left
        result.y() -= margins[0];      // top
        result.width() += margins[1];  // right
        result.height() += margins[2]; // bottom
        result.x() -= margins[3];      // left
        result.width() += margins[3];  // add left to width too
        result.height() += margins[0]; // add top to height too
      }
    }

    return result;
  }

  bool IntersectionObserver::queueEntry(const IntersectionObserverEntry &entry)
  {
    lock_guard<shared_mutex> lock(entriesMutex_);
    entriesQueue_.push_back(entry);

    // TODO: Schedule callback execution in microtask queue
    // For now, just return true
    return true;
  }

  double IntersectionObserver::getCurrentTime()
  {
    // Return time in milliseconds since some epoch
    // This should ideally be time since page load, but for now use system time
    auto now = chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
    return static_cast<double>(millis);
  }
}