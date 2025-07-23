#include "extension.hpp"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <regex>

namespace jsar {
namespace extensions {

Extension::Extension(const std::string& id, const ExtensionManifest& manifest, const std::string& base_path)
  : context_(id, manifest, base_path) {
}

Extension::~Extension() {
  if (context_.state != ExtensionState::UNLOADED) {
    unload();
  }
  // Ensure all background processes are terminated
  killBackgroundProcesses();
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
    // Terminate all background processes
    killBackgroundProcesses();
    
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

bool Extension::injectContentScript(const std::string& url, const ContentScriptContext& context) {
  std::filesystem::path full_script_path = std::filesystem::path(context_.base_path) / context.script_path;
  
  if (!std::filesystem::exists(full_script_path)) {
    std::cerr << "[Extension:" << context_.id << "] Content script not found: " << context.script_path << std::endl;
    return false;
  }
  
  try {
    // Read the script content
    std::ifstream script_file(full_script_path);
    if (!script_file.is_open()) {
      std::cerr << "[Extension:" << context_.id << "] Failed to open content script: " << context.script_path << std::endl;
      return false;
    }
    
    std::stringstream script_content;
    script_content << script_file.rdbuf();
    script_file.close();
    
    std::cout << "[Extension:" << context_.id << "] Injecting content script: " << context.script_path 
              << " into " << url << " at " << ContentScriptRunAtToString(context.run_at) << std::endl;
    
    // TODO: Implement actual content script injection into the page's renderer process
    // This would involve:
    // 1. Finding the target renderer process for the URL
    // 2. Creating an isolated JavaScript context
    // 3. Injecting the script with limited Chrome APIs
    // 4. Setting up communication channels with background script
    
    return true;
    
  } catch (const std::exception& e) {
    std::cerr << "[Extension:" << context_.id << "] Failed to inject content script " 
              << context.script_path << ": " << e.what() << std::endl;
    return false;
  }
}

std::vector<ContentScriptContext> Extension::getMatchingContentScripts(const std::string& url) const {
  std::vector<ContentScriptContext> matching_scripts;
  
  for (const auto& content_script : context_.manifest.content_scripts) {
    // Check if URL matches any of the patterns
    for (const auto& pattern : content_script.matches) {
      if (urlMatchesPattern(url, pattern)) {
        // Add all JavaScript files from this content script
        for (const auto& js_file : content_script.js) {
          ContentScriptContext context(url, js_file, content_script.run_at, content_script.all_frames);
          matching_scripts.push_back(context);
        }
        break; // Only match once per content_script
      }
    }
  }
  
  return matching_scripts;
}

bool Extension::killBackgroundProcesses() {
  bool all_killed = true;
  
  for (auto& process : context_.background_processes) {
    if (process.is_running && process.pid > 0) {
      std::cout << "[Extension:" << context_.id << "] Terminating background process " << process.pid << std::endl;
      
      // Send SIGTERM first, then SIGKILL if needed
      if (kill(process.pid, SIGTERM) == 0) {
        // Wait for process to terminate
        int status;
        int result = waitpid(process.pid, &status, WNOHANG);
        
        if (result == 0) {
          // Process still running, force kill
          usleep(100000); // Wait 100ms
          kill(process.pid, SIGKILL);
          waitpid(process.pid, &status, 0);
        }
        
        process.is_running = false;
        std::cout << "[Extension:" << context_.id << "] Background process " << process.pid << " terminated" << std::endl;
      } else {
        std::cerr << "[Extension:" << context_.id << "] Failed to terminate background process " << process.pid << std::endl;
        all_killed = false;
      }
    }
  }
  
  // Clear the process list
  context_.background_processes.clear();
  return all_killed;
}

std::vector<BackgroundProcess> Extension::getActiveBackgroundProcesses() const {
  std::vector<BackgroundProcess> active_processes;
  
  for (const auto& process : context_.background_processes) {
    if (process.is_running) {
      active_processes.push_back(process);
    }
  }
  
  return active_processes;
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
    std::cout << "[Extension:" << context_.id << "] Forking background process for script: " << script_path << std::endl;
    
    pid_t pid = forkBackgroundProcess(script_path);
    if (pid > 0) {
      // Parent process - store the background process info
      BackgroundProcess process(pid, script_path);
      context_.background_processes.push_back(process);
      
      loaded_scripts_.push_back(script_path);
      std::cout << "[Extension:" << context_.id << "] Background process " << pid << " started for " << script_path << std::endl;
      return true;
    } else {
      std::cerr << "[Extension:" << context_.id << "] Failed to fork background process for " << script_path << std::endl;
      return false;
    }
    
  } catch (const std::exception& e) {
    std::cerr << "[Extension:" << context_.id << "] Failed to execute background script " << script_path << ": " << e.what() << std::endl;
    return false;
  }
}

pid_t Extension::forkBackgroundProcess(const std::string& script_path) {
  std::filesystem::path full_script_path = std::filesystem::path(context_.base_path) / script_path;
  
  pid_t pid = fork();
  
  if (pid == 0) {
    // Child process - execute the background script
    std::cout << "[Extension:" << context_.id << "] Background process starting for " << script_path << std::endl;
    
    // Set up environment for the background script
    setenv("EXTENSION_ID", context_.id.c_str(), 1);
    setenv("EXTENSION_NAME", context_.manifest.name.c_str(), 1);
    setenv("EXTENSION_VERSION", context_.manifest.version.c_str(), 1);
    setenv("EXTENSION_BASE_PATH", context_.base_path.c_str(), 1);
    
    // For now, execute as a Node.js script (in a real implementation, this could be V8 or another JS engine)
    // This provides the dedicated Extension Process with full Chrome API access
    const char* args[] = {"node", full_script_path.c_str(), nullptr};
    
    if (execvp("node", const_cast<char* const*>(args)) == -1) {
      std::cerr << "[Extension:" << context_.id << "] Failed to exec background script: " << script_path << std::endl;
      _exit(1);
    }
    
    // This line should never be reached
    _exit(0);
    
  } else if (pid > 0) {
    // Parent process - return the child PID
    return pid;
  } else {
    // Fork failed
    std::cerr << "[Extension:" << context_.id << "] Fork failed for background script: " << script_path << std::endl;
    return -1;
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
    
    // Parse content scripts
    if (doc.HasMember("content_scripts") && doc["content_scripts"].IsArray()) {
      if (!parseContentScripts(&doc["content_scripts"], manifest)) {
        std::cerr << "Failed to parse content_scripts in manifest: " << manifest_path << std::endl;
        return false;
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

bool Extension::urlMatchesPattern(const std::string& url, const std::string& pattern) const {
  // Convert Chrome extension match pattern to regex
  // For now, simple wildcard matching (in a real implementation, this would be more sophisticated)
  std::string regex_pattern = pattern;
  
  // Replace * with .*
  size_t pos = 0;
  while ((pos = regex_pattern.find("*", pos)) != std::string::npos) {
    regex_pattern.replace(pos, 1, ".*");
    pos += 2;
  }
  
  try {
    std::regex url_regex(regex_pattern);
    return std::regex_match(url, url_regex);
  } catch (const std::exception& e) {
    std::cerr << "[Extension:" << context_.id << "] Invalid URL pattern: " << pattern << std::endl;
    return false;
  }
}

bool Extension::parseContentScripts(const void* content_scripts_ptr, ExtensionManifest& manifest) {
  const rapidjson::Value& content_scripts_array = *static_cast<const rapidjson::Value*>(content_scripts_ptr);
  for (const auto& cs : content_scripts_array.GetArray()) {
    if (!cs.IsObject()) {
      std::cerr << "Content script entry is not an object" << std::endl;
      continue;
    }
    
    ContentScript content_script;
    
    // Parse matches
    if (cs.HasMember("matches") && cs["matches"].IsArray()) {
      for (const auto& match : cs["matches"].GetArray()) {
        if (match.IsString()) {
          content_script.matches.push_back(match.GetString());
        }
      }
    }
    
    // Parse js files
    if (cs.HasMember("js") && cs["js"].IsArray()) {
      for (const auto& js : cs["js"].GetArray()) {
        if (js.IsString()) {
          content_script.js.push_back(js.GetString());
        }
      }
    }
    
    // Parse css files
    if (cs.HasMember("css") && cs["css"].IsArray()) {
      for (const auto& css : cs["css"].GetArray()) {
        if (css.IsString()) {
          content_script.css.push_back(css.GetString());
        }
      }
    }
    
    // Parse run_at
    if (cs.HasMember("run_at") && cs["run_at"].IsString()) {
      content_script.run_at = StringToContentScriptRunAt(cs["run_at"].GetString());
    }
    
    // Parse all_frames
    if (cs.HasMember("all_frames") && cs["all_frames"].IsBool()) {
      content_script.all_frames = cs["all_frames"].GetBool();
    }
    
    // Only add if we have matches and scripts
    if (!content_script.matches.empty() && (!content_script.js.empty() || !content_script.css.empty())) {
      manifest.content_scripts.push_back(content_script);
    }
  }
  
  return true;
}

} // namespace extensions
} // namespace jsar