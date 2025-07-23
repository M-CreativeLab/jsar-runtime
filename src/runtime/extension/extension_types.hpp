#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <sys/types.h>

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
 * Content script run timing
 */
enum class ContentScriptRunAt {
  DOCUMENT_START,
  DOCUMENT_END,
  DOCUMENT_IDLE
};

/**
 * Content script configuration for declarative injection
 */
struct ContentScript {
  std::vector<std::string> matches;     // URL patterns to match
  std::vector<std::string> js;          // JavaScript files to inject
  std::vector<std::string> css;         // CSS files to inject
  ContentScriptRunAt run_at = ContentScriptRunAt::DOCUMENT_IDLE;
  bool all_frames = false;              // Inject into subframes?
  
  ContentScript() = default;
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
  
  std::vector<ContentScript> content_scripts;  // Content script configurations
  std::vector<std::string> permissions;
  
  // Default constructor
  ExtensionManifest() = default;
};

/**
 * Background process information
 */
struct BackgroundProcess {
  pid_t pid = -1;                      // Process ID
  std::string script_path;             // Script being executed
  bool is_running = false;             // Process state
  
  BackgroundProcess() = default;
  BackgroundProcess(pid_t process_id, const std::string& path) 
    : pid(process_id), script_path(path), is_running(true) {}
};

/**
 * Content script injection context
 */
struct ContentScriptContext {
  std::string url;                     // Target URL
  std::string script_path;             // Script to inject
  ContentScriptRunAt run_at;           // When to inject
  bool all_frames;                     // Include subframes
  
  ContentScriptContext() = default;
  ContentScriptContext(const std::string& target_url, const std::string& script, 
                      ContentScriptRunAt timing, bool frames)
    : url(target_url), script_path(script), run_at(timing), all_frames(frames) {}
};

/**
 * Extension context containing runtime information
 */
struct ExtensionContext {
  std::string id;
  ExtensionManifest manifest;
  std::string base_path;
  ExtensionState state = ExtensionState::LOADING;
  std::vector<BackgroundProcess> background_processes;  // Forked background processes
  
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
 * Convert ContentScriptRunAt to string
 */
inline std::string ContentScriptRunAtToString(ContentScriptRunAt run_at) {
  switch (run_at) {
    case ContentScriptRunAt::DOCUMENT_START: return "document_start";
    case ContentScriptRunAt::DOCUMENT_END: return "document_end";
    case ContentScriptRunAt::DOCUMENT_IDLE: return "document_idle";
    default: return "document_idle";
  }
}

/**
 * Convert string to ContentScriptRunAt
 */
inline ContentScriptRunAt StringToContentScriptRunAt(const std::string& str) {
  if (str == "document_start") return ContentScriptRunAt::DOCUMENT_START;
  if (str == "document_end") return ContentScriptRunAt::DOCUMENT_END;
  return ContentScriptRunAt::DOCUMENT_IDLE; // default
}

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