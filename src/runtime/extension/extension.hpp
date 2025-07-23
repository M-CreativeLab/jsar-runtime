#pragma once

#include "extension_types.hpp"
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <sys/types.h>

namespace jsar {
namespace extensions {

/**
 * Individual extension class managing the lifecycle of a single extension
 * Inspired by Chrome Extension architecture with process separation
 */
class Extension {
public:
  Extension(const std::string& id, const ExtensionManifest& manifest, const std::string& base_path);
  ~Extension();

  // Getters
  const std::string& getId() const { return context_.id; }
  const ExtensionManifest& getManifest() const { return context_.manifest; }
  ExtensionState getState() const { return context_.state; }
  const ExtensionContext& getContext() const { return context_; }

  // Lifecycle methods
  bool load();
  bool enable();
  bool disable();
  bool unload();

  // Content script injection methods
  bool injectContentScript(const std::string& url, const ContentScriptContext& context);
  std::vector<ContentScriptContext> getMatchingContentScripts(const std::string& url) const;
  
  // Background process management
  bool killBackgroundProcesses();
  std::vector<BackgroundProcess> getActiveBackgroundProcesses() const;

  // Event system
  void addEventListener(const std::string& event_type, ExtensionEventCallback callback);
  void removeEventListener(const std::string& event_type);
  void emit(const std::string& event_type, const std::unordered_map<std::string, std::string>& data = {});

  // Static factory method
  static std::unique_ptr<Extension> loadFromDirectory(const std::string& extension_path);

private:
  ExtensionContext context_;
  std::unordered_map<std::string, ExtensionEventCallback> event_listeners_;
  std::vector<std::string> loaded_scripts_;

  // Internal methods
  bool executeBackgroundScript(const std::string& script_path);
  bool parseManifest(const std::string& manifest_path, ExtensionManifest& manifest);
  void setState(ExtensionState new_state);
  std::string createExtensionId(const std::string& name) const;
  
  // Process management
  pid_t forkBackgroundProcess(const std::string& script_path);
  bool urlMatchesPattern(const std::string& url, const std::string& pattern) const;
  bool parseContentScripts(const void* content_scripts_array, ExtensionManifest& manifest);
};

} // namespace extensions
} // namespace jsar