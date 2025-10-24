#include "./variable_reference_tracker.hpp"
#include "./values/computed/context.hpp"

namespace endor
{
  namespace client_cssom
  {
    using namespace std;

    // Static empty set for returning when no dependencies exist
    const unordered_set<string> VariableReferenceTracker::empty_set_;

    void VariableReferenceTracker::trackDependency(const string &variable_name, const string &property_name)
    {
      variable_dependencies_[variable_name].insert(property_name);
    }

    void VariableReferenceTracker::clearDependencies(const string &property_name)
    {
      // Remove this property from all variable dependency sets
      for (auto &pair : variable_dependencies_)
      {
        pair.second.erase(property_name);
      }

      // Clean up empty dependency sets
      for (auto it = variable_dependencies_.begin(); it != variable_dependencies_.end();)
      {
        if (it->second.empty())
        {
          it = variable_dependencies_.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }

    void VariableReferenceTracker::clearVariableDependencies(const string &variable_name)
    {
      variable_dependencies_.erase(variable_name);
    }

    const unordered_set<string> &VariableReferenceTracker::getDependents(const string &variable_name) const
    {
      auto it = variable_dependencies_.find(variable_name);
      return (it != variable_dependencies_.end()) ? it->second : empty_set_;
    }

    bool VariableReferenceTracker::hasDependents(const string &variable_name) const
    {
      auto it = variable_dependencies_.find(variable_name);
      return it != variable_dependencies_.end() && !it->second.empty();
    }

    void VariableReferenceTracker::setVariableChangeCallback(VariableChangeCallback callback)
    {
      change_callback_ = move(callback);
    }

    void VariableReferenceTracker::notifyVariableChanged(const string &variable_name,
                                                         const string &new_value,
                                                         const values::computed::Context &context)
    {
      if (change_callback_ && hasDependents(variable_name))
      {
        const auto &dependents = getDependents(variable_name);
        for (const string &property_name : dependents)
        {
          change_callback_(property_name, new_value, context);
        }
      }
    }

    size_t VariableReferenceTracker::getDependencyCount() const
    {
      size_t count = 0;
      for (const auto &pair : variable_dependencies_)
      {
        count += pair.second.size();
      }
      return count;
    }

    void VariableReferenceTracker::clear()
    {
      variable_dependencies_.clear();
    }

  } // namespace client_cssom
} // namespace endor