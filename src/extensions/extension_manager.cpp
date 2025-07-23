#include "extension_manager.hpp"
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace jsar {
namespace extensions {

ExtensionManager::ExtensionManager() {
}

ExtensionManager::~ExtensionManager() {
  unloadAllExtensions();
}

bool ExtensionManager::loadExtension(const std::string& extension_path, const ExtensionLoadOptions& options) {
  try {
    std::cout << "[ExtensionManager] Loading extension from: " << extension_path << std::endl;
    
    auto extension = Extension::loadFromDirectory(extension_path);
    const std::string extension_id = extension->getId();
    
    // Check if extension is already loaded
    if (extensions_.find(extension_id) != extensions_.end()) {
      std::cerr << "[ExtensionManager] Extension with ID '" << extension_id << "' is already loaded" << std::endl;
      return false;
    }
    
    // Store the extension and its path
    extensions_[extension_id] = std::shared_ptr<Extension>(extension.release());
    extension_paths_[extension_id] = extension_path;
    
    // Set up event forwarding from extension to manager
    setupExtensionEventForwarding(extensions_[extension_id]);
    
    // Load the extension
    if (!extensions_[extension_id]->load()) {
      extensions_.erase(extension_id);
      extension_paths_.erase(extension_id);
      return false;
    }
    
    // Enable immediately if requested (default: true)
    if (options.enabled) {
      if (!extensions_[extension_id]->enable()) {
        std::cerr << "[ExtensionManager] Failed to enable extension after loading: " << extension_id << std::endl;
        // Continue anyway, extension is loaded but not enabled
      }
    }
    
    // Emit extension loaded event
    emit("extensionLoaded", extension_id, {{"path", extension_path}});
    
    std::cout << "[ExtensionManager] Extension '" << extension_id << "' loaded successfully" << std::endl;
    return true;
    
  } catch (const std::exception& e) {
    std::cerr << "[ExtensionManager] Failed to load extension from " << extension_path << ": " << e.what() << std::endl;
    return false;
  }
}

bool ExtensionManager::loadExtensionsFromDirectory(const std::string& extensions_dir) {
  try {
    if (!std::filesystem::exists(extensions_dir) || !std::filesystem::is_directory(extensions_dir)) {
      std::cerr << "[ExtensionManager] Extensions directory does not exist: " << extensions_dir << std::endl;
      return false;
    }
    
    int loaded_count = 0;
    
    for (const auto& entry : std::filesystem::directory_iterator(extensions_dir)) {
      if (entry.is_directory()) {
        const std::string extension_path = entry.path().string();
        
        if (isValidExtensionDirectory(extension_path)) {
          try {
            if (loadExtension(extension_path)) {
              loaded_count++;
            }
          } catch (const std::exception& e) {
            std::cerr << "[ExtensionManager] Skipping directory '" << entry.path().filename().string() 
                      << "': " << e.what() << std::endl;
          }
        } else {
          std::cout << "[ExtensionManager] Skipping directory '" << entry.path().filename().string() 
                    << "': no manifest.json found" << std::endl;
        }
      }
    }
    
    std::cout << "[ExtensionManager] Loaded " << loaded_count << " extensions from " << extensions_dir << std::endl;
    return loaded_count > 0;
    
  } catch (const std::exception& e) {
    std::cerr << "[ExtensionManager] Failed to load extensions from directory " << extensions_dir 
              << ": " << e.what() << std::endl;
    return false;
  }
}

bool ExtensionManager::enableExtension(const std::string& extension_id) {
  auto it = extensions_.find(extension_id);
  if (it == extensions_.end()) {
    std::cerr << "[ExtensionManager] Extension '" << extension_id << "' not found" << std::endl;
    return false;
  }
  
  if (!it->second->enable()) {
    return false;
  }
  
  emit("extensionEnabled", extension_id);
  return true;
}

bool ExtensionManager::disableExtension(const std::string& extension_id) {
  auto it = extensions_.find(extension_id);
  if (it == extensions_.end()) {
    std::cerr << "[ExtensionManager] Extension '" << extension_id << "' not found" << std::endl;
    return false;
  }
  
  if (!it->second->disable()) {
    return false;
  }
  
  emit("extensionDisabled", extension_id);
  return true;
}

bool ExtensionManager::unloadExtension(const std::string& extension_id) {
  auto it = extensions_.find(extension_id);
  if (it == extensions_.end()) {
    std::cerr << "[ExtensionManager] Extension '" << extension_id << "' not found" << std::endl;
    return false;
  }
  
  try {
    if (!it->second->unload()) {
      return false;
    }
    
    // Remove from maps
    extensions_.erase(extension_id);
    extension_paths_.erase(extension_id);
    
    emit("extensionUnloaded", extension_id);
    
    std::cout << "[ExtensionManager] Extension '" << extension_id << "' unloaded successfully" << std::endl;
    return true;
    
  } catch (const std::exception& e) {
    std::cerr << "[ExtensionManager] Failed to unload extension '" << extension_id << "': " << e.what() << std::endl;
    return false;
  }
}

bool ExtensionManager::unloadAllExtensions() {
  std::vector<std::string> extension_ids;
  for (const auto& pair : extensions_) {
    extension_ids.push_back(pair.first);
  }
  
  bool all_success = true;
  for (const auto& extension_id : extension_ids) {
    try {
      if (!unloadExtension(extension_id)) {
        all_success = false;
      }
    } catch (const std::exception& e) {
      std::cerr << "[ExtensionManager] Failed to unload extension '" << extension_id << "': " << e.what() << std::endl;
      all_success = false;
    }
  }
  
  std::cout << "[ExtensionManager] All extensions unloaded" << std::endl;
  return all_success;
}

std::vector<std::shared_ptr<Extension>> ExtensionManager::getExtensions() const {
  std::vector<std::shared_ptr<Extension>> result;
  for (const auto& pair : extensions_) {
    result.push_back(pair.second);
  }
  return result;
}

std::shared_ptr<Extension> ExtensionManager::getExtension(const std::string& extension_id) const {
  auto it = extensions_.find(extension_id);
  return (it != extensions_.end()) ? it->second : nullptr;
}

bool ExtensionManager::hasExtension(const std::string& extension_id) const {
  return extensions_.find(extension_id) != extensions_.end();
}

std::unordered_map<std::string, ExtensionState> ExtensionManager::getExtensionStates() const {
  std::unordered_map<std::string, ExtensionState> states;
  for (const auto& pair : extensions_) {
    states[pair.first] = pair.second->getState();
  }
  return states;
}

std::vector<std::shared_ptr<Extension>> ExtensionManager::getExtensionsByState(ExtensionState state) const {
  std::vector<std::shared_ptr<Extension>> result;
  for (const auto& pair : extensions_) {
    if (pair.second->getState() == state) {
      result.push_back(pair.second);
    }
  }
  return result;
}

void ExtensionManager::addEventListener(const std::string& event_type, ExtensionEventCallback callback) {
  event_listeners_[event_type] = callback;
}

void ExtensionManager::removeEventListener(const std::string& event_type) {
  event_listeners_.erase(event_type);
}

void ExtensionManager::emit(const std::string& event_type, const std::string& extension_id, 
                           const std::unordered_map<std::string, std::string>& data) {
  auto it = event_listeners_.find(event_type);
  if (it != event_listeners_.end()) {
    ExtensionEvent event(event_type, extension_id);
    event.data = data;
    it->second(event);
  }
}

void ExtensionManager::setupExtensionEventForwarding(std::shared_ptr<Extension> extension) {
  const std::vector<std::string> event_types = {
    "loaded", "enabled", "disabled", "unloaded", "error", "stateChanged"
  };
  
  for (const auto& event_type : event_types) {
    extension->addEventListener(event_type, [this, event_type](const ExtensionEvent& event) {
      // Convert extension event to manager event
      std::string manager_event_type = "extension" + event_type;
      if (!event_type.empty()) {
        manager_event_type[9] = std::toupper(manager_event_type[9]); // Capitalize first letter after "extension"
      }
      
      emit(manager_event_type, event.extension_id, event.data);
    });
  }
}

bool ExtensionManager::isValidExtensionDirectory(const std::string& path) const {
  std::filesystem::path manifest_path = std::filesystem::path(path) / "manifest.json";
  return std::filesystem::exists(manifest_path) && std::filesystem::is_regular_file(manifest_path);
}

} // namespace extensions
} // namespace jsar