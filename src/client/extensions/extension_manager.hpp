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
 * ExtensionManager manages the lifecycle of all extensions
 * Inspired by Chrome Extension management
 */
class ExtensionManager {
public:
  ExtensionManager();
  ~ExtensionManager();

  // Extension management
  bool loadExtension(const std::string& extension_path, const ExtensionLoadOptions& options = ExtensionLoadOptions());
  bool loadExtensionsFromDirectory(const std::string& extensions_dir);
  bool enableExtension(const std::string& extension_id);
  bool disableExtension(const std::string& extension_id);
  bool unloadExtension(const std::string& extension_id);
  bool unloadAllExtensions();

  // Extension access
  std::vector<std::shared_ptr<Extension>> getExtensions() const;
  std::shared_ptr<Extension> getExtension(const std::string& extension_id) const;
  bool hasExtension(const std::string& extension_id) const;
  std::unordered_map<std::string, ExtensionState> getExtensionStates() const;
  std::vector<std::shared_ptr<Extension>> getExtensionsByState(ExtensionState state) const;

  // Event system
  void addEventListener(const std::string& event_type, ExtensionEventCallback callback);
  void removeEventListener(const std::string& event_type); 
  void emit(const std::string& event_type, const std::string& extension_id, 
            const std::unordered_map<std::string, std::string>& data = {});

private:
  std::unordered_map<std::string, std::shared_ptr<Extension>> extensions_;
  std::unordered_map<std::string, std::string> extension_paths_;
  std::unordered_map<std::string, ExtensionEventCallback> event_listeners_;

  // Internal methods
  void setupExtensionEventForwarding(std::shared_ptr<Extension> extension);
  bool isValidExtensionDirectory(const std::string& path) const;
};

} // namespace extensions
} // namespace jsar