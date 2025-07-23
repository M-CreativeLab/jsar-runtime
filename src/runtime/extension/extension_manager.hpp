#pragma once

#include "extension.hpp"
#include "extension_types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace jsar {
namespace extensions {

/**
 * Central manager for extension operations in the JSAR runtime
 * Handles loading, unloading, and lifecycle management of extensions
 * Browser-scoped concept managing extension processes
 */
class ExtensionManager {
public:
  ExtensionManager();
  ~ExtensionManager();

  // Extension lifecycle management
  bool loadExtension(const std::string& extension_path, const ExtensionLoadOptions& options = ExtensionLoadOptions());
  bool unloadExtension(const std::string& extension_id);
  bool enableExtension(const std::string& extension_id);
  bool disableExtension(const std::string& extension_id);
  
  // Batch operations
  bool loadExtensionsFromDirectory(const std::string& extensions_dir, const ExtensionLoadOptions& options = ExtensionLoadOptions());
  bool unloadAllExtensions();
  
  // Content script management
  bool injectContentScriptsForUrl(const std::string& url);
  std::vector<ContentScriptContext> getAllMatchingContentScripts(const std::string& url) const;
  
  // Query methods
  std::shared_ptr<Extension> getExtension(const std::string& extension_id) const;
  std::vector<std::shared_ptr<Extension>> getAllExtensions() const;
  std::vector<std::shared_ptr<Extension>> getExtensionsByState(ExtensionState state) const;
  bool hasExtension(const std::string& extension_id) const;
  
  // Statistics
  size_t getExtensionCount() const { return extensions_.size(); }
  size_t getActiveBackgroundProcessCount() const;
  
  // Event system
  void addEventListener(const std::string& event_type, ExtensionEventCallback callback);
  void removeEventListener(const std::string& event_type);

private:
  std::unordered_map<std::string, std::shared_ptr<Extension>> extensions_;
  std::unordered_map<std::string, std::string> extension_paths_;
  std::unordered_map<std::string, ExtensionEventCallback> event_listeners_;

  // Internal methods
  void setupExtensionEventForwarding(std::shared_ptr<Extension> extension);
  void forwardExtensionEvent(const ExtensionEvent& event);
  bool isValidExtensionDirectory(const std::string& path) const;
};

} // namespace extensions
} // namespace jsar