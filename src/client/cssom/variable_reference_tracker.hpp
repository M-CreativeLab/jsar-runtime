#ifndef CLIENT_CSSOM_VARIABLE_REFERENCE_TRACKER_HPP_
#define CLIENT_CSSOM_VARIABLE_REFERENCE_TRACKER_HPP_

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace client_cssom
{
  namespace values
  {
    namespace computed
    {
      class Context;
    }
  }

  /**
   * Manages variable references and dependency tracking for CSS custom properties.
   * Provides functionality to track which properties depend on which variables
   * and trigger recomputation when variables change.
   */
  class VariableReferenceTracker
  {
  public:
    using VariableChangeCallback = std::function<void(const std::string &, const std::string &, const values::computed::Context &)>;

    /**
     * Track that a property depends on a variable.
     * @param variable_name The CSS variable name (e.g., "--primary-color")
     * @param property_name The property that uses this variable
     */
    void trackDependency(const std::string &variable_name, const std::string &property_name);

    /**
     * Clear all dependencies for a property.
     * @param property_name The property to clear dependencies for
     */
    void clearDependencies(const std::string &property_name);

    /**
     * Clear all dependencies for a variable.
     * @param variable_name The variable to clear dependencies for
     */
    void clearVariableDependencies(const std::string &variable_name);

    /**
     * Get all properties that depend on a given variable.
     * @param variable_name The variable name
     * @return Set of property names that depend on this variable
     */
    const std::unordered_set<std::string> &getDependents(const std::string &variable_name) const;

    /**
     * Check if a variable has any dependents.
     * @param variable_name The variable name
     * @return True if the variable has dependent properties
     */
    bool hasDependents(const std::string &variable_name) const;

    /**
     * Set a callback to be called when variable dependencies need recomputation.
     * @param callback Function to call when properties need to be recomputed
     */
    void setVariableChangeCallback(VariableChangeCallback callback);

    /**
     * Trigger recomputation of all properties that depend on a variable.
     * @param variable_name The variable that changed
     * @param new_value The new value of the variable
     * @param context The computation context
     */
    void notifyVariableChanged(const std::string &variable_name, const std::string &new_value, const values::computed::Context &context);

    /**
     * Get the total number of tracked dependencies.
     * @return Number of tracked variable->property dependencies
     */
    size_t getDependencyCount() const;

    /**
     * Clear all tracked dependencies.
     */
    void clear();

  private:
    // Maps variable names to sets of properties that depend on them
    std::unordered_map<std::string, std::unordered_set<std::string>> variable_dependencies_;

    // Callback for when variables change
    VariableChangeCallback change_callback_;

    // Empty set for returning when no dependencies exist
    static const std::unordered_set<std::string> empty_set_;
  };

} // namespace client_cssom

#endif // CLIENT_CSSOM_VARIABLE_REFERENCE_TRACKER_HPP_