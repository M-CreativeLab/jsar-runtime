#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace jsar {
namespace extensions {

/**
 * Extension states mirroring the TypeScript implementation
 */
enum class ExtensionState {
  LOADING,
  LOADED,
  RUNNING,
  DISABLED,
  ERROR,
  UNLOADED
};

/**
 * Extension manifest structure compatible with Chrome Extension v3
 */
struct ExtensionManifest {
  std::string name;
  std::string version;
  std::string description;
  int manifest_version = 3;
  
  struct Background {
    std::vector<std::string> scripts;
    bool persistent = false;
  } background;
  
  std::vector<std::string> permissions;
  
  // Default constructor
  ExtensionManifest() = default;
};

/**
 * Extension context containing runtime information
 */
struct ExtensionContext {
  std::string id;
  ExtensionManifest manifest;
  std::string base_path;
  ExtensionState state = ExtensionState::LOADING;
  
  ExtensionContext() = default;
  ExtensionContext(const std::string& ext_id, const ExtensionManifest& ext_manifest, const std::string& path)
    : id(ext_id), manifest(ext_manifest), base_path(path) {}
};

/**
 * Extension load options
 */
struct ExtensionLoadOptions {
  bool enabled = true;
  bool persistent = false;
  
  ExtensionLoadOptions() = default;
  ExtensionLoadOptions(bool enable, bool persist = false) : enabled(enable), persistent(persist) {}
};

/**
 * Extension event data
 */
struct ExtensionEvent {
  std::string type;
  std::string extension_id;
  std::unordered_map<std::string, std::string> data;
  
  ExtensionEvent() = default;
  ExtensionEvent(const std::string& event_type, const std::string& ext_id)
    : type(event_type), extension_id(ext_id) {}
};

/**
 * Extension event callback type
 */
using ExtensionEventCallback = std::function<void(const ExtensionEvent&)>;

/**
 * Convert ExtensionState to string for logging
 */
inline std::string ExtensionStateToString(ExtensionState state) {
  switch (state) {
    case ExtensionState::LOADING: return "loading";
    case ExtensionState::LOADED: return "loaded";
    case ExtensionState::RUNNING: return "running";
    case ExtensionState::DISABLED: return "disabled";
    case ExtensionState::ERROR: return "error";
    case ExtensionState::UNLOADED: return "unloaded";
    default: return "unknown";
  }
}

} // namespace extensions
} // namespace jsar