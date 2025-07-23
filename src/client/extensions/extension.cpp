#include "extension.hpp"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace jsar {
namespace extensions {

Extension::Extension(const std::string& id, const ExtensionManifest& manifest, const std::string& base_path)
  : context_(id, manifest, base_path) {
}

Extension::~Extension() {
  if (context_.state != ExtensionState::UNLOADED) {
    unload();
  }
}

bool Extension::load() {
  try {
    setState(ExtensionState::LOADING);
    
    // Execute background scripts if defined
    for (const auto& script_path : context_.manifest.background.scripts) {
      if (!executeBackgroundScript(script_path)) {
        setState(ExtensionState::ERROR);
        return false;
      }
    }
    
    setState(ExtensionState::LOADED);
    emit("loaded");
    
    std::cout << "[Extension:" << context_.id << "] Loaded successfully" << std::endl;
    return true;
    
  } catch (const std::exception& e) {
    setState(ExtensionState::ERROR);
    emit("error", {{"message", e.what()}});
    std::cerr << "[Extension:" << context_.id << "] Load failed: " << e.what() << std::endl;
    return false;
  }
}

bool Extension::enable() {
  if (context_.state != ExtensionState::LOADED && context_.state != ExtensionState::DISABLED) {
    std::cerr << "[Extension:" << context_.id << "] Cannot enable extension in state: " 
              << ExtensionStateToString(context_.state) << std::endl;
    return false;
  }
  
  try {
    setState(ExtensionState::RUNNING);
    emit("enabled");
    
    std::cout << "[Extension:" << context_.id << "] Enabled successfully" << std::endl;
    return true;
    
  } catch (const std::exception& e) {
    setState(ExtensionState::ERROR);
    emit("error", {{"message", e.what()}});
    std::cerr << "[Extension:" << context_.id << "] Enable failed: " << e.what() << std::endl;
    return false;
  }
}

bool Extension::disable() {
  if (context_.state != ExtensionState::RUNNING) {
    std::cerr << "[Extension:" << context_.id << "] Cannot disable extension in state: " 
              << ExtensionStateToString(context_.state) << std::endl;
    return false;
  }
  
  try {
    setState(ExtensionState::DISABLED);
    emit("disabled");
    
    std::cout << "[Extension:" << context_.id << "] Disabled successfully" << std::endl;
    return true;
    
  } catch (const std::exception& e) {
    setState(ExtensionState::ERROR);
    emit("error", {{"message", e.what()}});
    std::cerr << "[Extension:" << context_.id << "] Disable failed: " << e.what() << std::endl;
    return false;
  }
}

bool Extension::unload() {
  try {
    // Clean up loaded scripts
    loaded_scripts_.clear();
    
    setState(ExtensionState::UNLOADED);
    emit("unloaded");
    
    std::cout << "[Extension:" << context_.id << "] Unloaded successfully" << std::endl;
    return true;
    
  } catch (const std::exception& e) {
    setState(ExtensionState::ERROR);
    emit("error", {{"message", e.what()}});
    std::cerr << "[Extension:" << context_.id << "] Unload failed: " << e.what() << std::endl;
    return false;
  }
}

void Extension::addEventListener(const std::string& event_type, ExtensionEventCallback callback) {
  event_listeners_[event_type] = callback;
}

void Extension::removeEventListener(const std::string& event_type) {
  event_listeners_.erase(event_type);
}

void Extension::emit(const std::string& event_type, const std::unordered_map<std::string, std::string>& data) {
  auto it = event_listeners_.find(event_type);
  if (it != event_listeners_.end()) {
    ExtensionEvent event(event_type, context_.id);
    event.data = data;
    it->second(event);
  }
}

std::unique_ptr<Extension> Extension::loadFromDirectory(const std::string& extension_path) {
  std::filesystem::path manifest_path = std::filesystem::path(extension_path) / "manifest.json";
  
  if (!std::filesystem::exists(manifest_path)) {
    throw std::runtime_error("manifest.json not found in " + extension_path);
  }
  
  ExtensionManifest manifest;
  Extension temp_extension("", manifest, extension_path);
  
  if (!temp_extension.parseManifest(manifest_path.string(), manifest)) {
    throw std::runtime_error("Failed to parse manifest.json from " + extension_path);
  }
  
  // Validate required fields
  if (manifest.name.empty() || manifest.version.empty()) {
    throw std::runtime_error("Invalid manifest: missing required fields (name, version)");
  }
  
  // Create extension ID from name
  std::string extension_id = temp_extension.createExtensionId(manifest.name);
  
  return std::make_unique<Extension>(extension_id, manifest, extension_path);
}

bool Extension::executeBackgroundScript(const std::string& script_path) {
  std::filesystem::path full_script_path = std::filesystem::path(context_.base_path) / script_path;
  
  if (!std::filesystem::exists(full_script_path)) {
    std::cerr << "[Extension:" << context_.id << "] Background script not found: " << script_path << std::endl;
    return false;
  }
  
  try {
    // Read the script content
    std::ifstream script_file(full_script_path);
    if (!script_file.is_open()) {
      std::cerr << "[Extension:" << context_.id << "] Failed to open background script: " << script_path << std::endl;
      return false;
    }
    
    std::stringstream script_content;
    script_content << script_file.rdbuf();
    script_file.close();
    
    // For now, we'll log the script execution (in a real implementation, this would execute in a JavaScript context)
    std::cout << "[Extension:" << context_.id << "] Executing background script: " << script_path << std::endl;
    std::cout << "[Extension:" << context_.id << "] Script content size: " << script_content.str().length() << " bytes" << std::endl;
    
    // TODO: Implement JavaScript execution context with chrome APIs
    // This would involve embedding a JavaScript engine like V8
    
    loaded_scripts_.push_back(script_path);
    return true;
    
  } catch (const std::exception& e) {
    std::cerr << "[Extension:" << context_.id << "] Failed to execute background script " << script_path << ": " << e.what() << std::endl;
    return false;
  }
}

bool Extension::parseManifest(const std::string& manifest_path, ExtensionManifest& manifest) {
  try {
    std::ifstream manifest_file(manifest_path);
    if (!manifest_file.is_open()) {
      std::cerr << "Failed to open manifest file: " << manifest_path << std::endl;
      return false;
    }
    
    std::stringstream manifest_content;
    manifest_content << manifest_file.rdbuf();
    manifest_file.close();
    
    rapidjson::Document doc;
    doc.Parse(manifest_content.str().c_str());
    
    if (doc.HasParseError()) {
      std::cerr << "Failed to parse manifest JSON: " << manifest_path << std::endl;
      return false;
    }
    
    // Parse required fields
    if (doc.HasMember("name") && doc["name"].IsString()) {
      manifest.name = doc["name"].GetString();
    }
    
    if (doc.HasMember("version") && doc["version"].IsString()) {
      manifest.version = doc["version"].GetString();
    }
    
    if (doc.HasMember("description") && doc["description"].IsString()) {
      manifest.description = doc["description"].GetString();
    }
    
    if (doc.HasMember("manifest_version") && doc["manifest_version"].IsInt()) {
      manifest.manifest_version = doc["manifest_version"].GetInt();
    }
    
    // Parse background scripts
    if (doc.HasMember("background") && doc["background"].IsObject()) {
      const auto& background = doc["background"];
      
      if (background.HasMember("scripts") && background["scripts"].IsArray()) {
        const auto& scripts = background["scripts"];
        for (const auto& script : scripts.GetArray()) {
          if (script.IsString()) {
            manifest.background.scripts.push_back(script.GetString());
          }
        }
      }
      
      if (background.HasMember("persistent") && background["persistent"].IsBool()) {
        manifest.background.persistent = background["persistent"].GetBool();
      }
    }
    
    // Parse permissions
    if (doc.HasMember("permissions") && doc["permissions"].IsArray()) {
      const auto& permissions = doc["permissions"];
      for (const auto& permission : permissions.GetArray()) {
        if (permission.IsString()) {
          manifest.permissions.push_back(permission.GetString());
        }
      }
    }
    
    return true;
    
  } catch (const std::exception& e) {
    std::cerr << "Exception parsing manifest " << manifest_path << ": " << e.what() << std::endl;
    return false;
  }
}

void Extension::setState(ExtensionState new_state) {
  ExtensionState old_state = context_.state;
  context_.state = new_state;
  
  if (old_state != new_state) {
    emit("stateChanged", {{"oldState", ExtensionStateToString(old_state)}, 
                          {"newState", ExtensionStateToString(new_state)}});
  }
}

std::string Extension::createExtensionId(const std::string& name) const {
  std::string id = name;
  
  // Convert to lowercase
  std::transform(id.begin(), id.end(), id.begin(), ::tolower);
  
  // Replace non-alphanumeric characters with hyphens
  std::replace_if(id.begin(), id.end(), [](char c) {
    return !std::isalnum(c);
  }, '-');
  
  return id;
}

} // namespace extensions
} // namespace jsar