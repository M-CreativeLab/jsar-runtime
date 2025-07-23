#pragma once

/**
 * JSAR Extension System
 * 
 * Provides Chrome Extension-like functionality for JSAR runtime,
 * allowing extensions to be loaded, unloaded, and execute scripts.
 * 
 * @example
 * ```cpp
 * #include "extensions/extensions.hpp"
 * 
 * jsar::extensions::ExtensionManager extensionManager;
 * extensionManager.loadExtension("/path/to/extension");
 * ```
 */

#include "extension_types.hpp"
#include "extension.hpp"
#include "extension_manager.hpp"