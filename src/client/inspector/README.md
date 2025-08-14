# Content Process Inspector Implementation

This directory contains CDP (Chrome DevTools Protocol) domain implementations that run within content processes, separate from the host/runtime process.

## Overview

The JSAR Runtime uses a multi-process architecture where:
- **Host Process**: Manages the overall runtime, renderer, and protocol handling
- **Content Processes**: Handle individual documents/web content and their JavaScript execution

This directory provides CDP domain handlers that run in content processes, allowing debugging and inspection of content-specific functionality like JavaScript runtime, DOM manipulation, and content-specific features.

## Architecture

### Files

- **`content_cdp_handler.hpp/cpp`**: Main CDP handler for content processes
  - Manages domain registration and message routing
  - Parses incoming CDP messages from the host proxy
  - Dispatches to appropriate domain handlers

- **`content_runtime_domain.hpp/cpp`**: Runtime domain implementation
  - Handles JavaScript runtime inspection (Runtime.enable, Runtime.getVersion, etc.)
  - Provides content-process-specific runtime information
  - Operates on the actual JavaScript engine within the content process

- **`content_example_domain.hpp/cpp`**: Example domain for testing
  - Demonstrates content-side CDP domain implementation
  - Provides ping/pong functionality for connectivity testing
  - Shows how to implement custom domains in content processes

### Communication Flow

1. CDP client sends request to host process
2. Host `CdpHandler` receives request
3. `ContentDomainProxy` forwards request to content process via IPC
4. Content process `ContentCdpHandler` receives and processes request
5. Content domain handler executes the requested method
6. Response sent back through IPC to host process
7. Host process returns response to CDP client

### Adding New Content Domains

To add a new CDP domain that runs in content processes:

1. Create domain header file (e.g., `content_my_domain.hpp`)
2. Extend `ContentCdpDomainHandler` base class
3. Implement required virtual methods
4. Create implementation file (e.g., `content_my_domain.cpp`)
5. Register domain in `ContentCdpHandler` constructor
6. Add domain name to forwarded domains in `ContentDomainProxy`

### Key Differences from Host Domains

- **Process Context**: Run within content processes, have access to content-specific state
- **IPC Communication**: Receive requests via IPC rather than direct function calls
- **Async Nature**: All communication is asynchronous through message passing
- **Isolation**: Each content process has its own domain instances
- **Lifecycle**: Tied to content process lifecycle

This design enables better isolation, scalability, and debugging capabilities by allowing direct inspection of content process state without cross-process synchronization overhead.