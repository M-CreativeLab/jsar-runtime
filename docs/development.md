# Development Guide

## Building

Currently, development is only supported on macOS, with build targets for macOS and Android platforms.

### macOS

```
$ make darwin CLEAN=yes
```

#### Building with Debug Symbols

To generate a build with dSYM debugging information, use Debug mode:

```sh
$ make darwin CLEAN=yes RELEASE=no
```

This will:
- Compile with `-g -fno-limit-debug-info` flags to include complete debug information
- Automatically generate `.dSYM` debug bundles for each target
- Install dSYM files alongside binaries in the output directory

The generated dSYM files can be used for:
- Source-level debugging with `lldb`
- Crash log symbolication
- Performance profiling tools

#### Debugging with lldb

After building with debug symbols, follow these steps to debug with lldb:

**1. Start lldb with your binary:**

```sh
$ lldb ./build/output/release/universal-apple-darwin/transmute_browser
```

**2. Load dSYM files (if not automatically detected):**

lldb usually loads dSYM files automatically if they're in the same directory as the binary. To verify or manually load them:

```sh
# Check if debug symbols are loaded
(lldb) image list transmute_browser

# If debug symbols aren't shown, manually add the dSYM bundle
(lldb) target symbols add ./build/output/release/universal-apple-darwin/transmute_browser.dSYM
```

**3. Set breakpoints:**

```sh
# Set a breakpoint on a function by name
(lldb) breakpoint set --name main
(lldb) b main

# Set a breakpoint at a specific file and line
(lldb) breakpoint set --file main.cpp --line 42
(lldb) b main.cpp:42

# List all breakpoints
(lldb) breakpoint list
```

**4. Run your program:**

```sh
# Run with arguments
(lldb) run /path/to/test.html

# Or use 'r' as shorthand
(lldb) r /path/to/test.html
```

**5. Debug commands:**

```sh
# Continue execution
(lldb) continue
(lldb) c

# Step over (execute current line)
(lldb) next
(lldb) n

# Step into (enter function calls)
(lldb) step
(lldb) s

# Step out (finish current function)
(lldb) finish

# Print backtrace (stack trace)
(lldb) backtrace
(lldb) bt

# Print variable value
(lldb) print variable_name
(lldb) p variable_name

# Print frame variables
(lldb) frame variable

# Examine memory
(lldb) memory read 0x12345678
(lldb) x 0x12345678
```

**6. Debugging a crash:**

If your program crashes, lldb will stop at the crash point:

```sh
# View the crash location
(lldb) bt

# Move to a specific frame
(lldb) frame select 2
(lldb) f 2

# View local variables
(lldb) frame variable

# View source code at crash point
(lldb) source list
```

**7. Advanced debugging:**

```sh
# Attach to a running process
$ lldb -p <pid>

# Or from within lldb
(lldb) process attach --pid <pid>

# Set conditional breakpoint
(lldb) breakpoint set --name foo --condition 'x > 10'

# Set watchpoint on a variable
(lldb) watchpoint set variable my_var

# View all loaded modules
(lldb) image list
```

**8. Debugging with VSCode:**

A VSCode launch configuration is provided in `.vscode/launch.json` for easy debugging. To use it:

1. **Install the CodeLLDB extension** in VSCode (required for lldb debugging on macOS)

2. **Build with debug symbols:**
   ```sh
   $ make darwin CLEAN=yes RELEASE=no
   ```

3. **Open the project in VSCode** and select one of the available debug configurations:
   - **Debug transmute_browser**: Launches transmute_browser with `fixtures/html/document.html`
   - **Debug transmute_browser (Custom HTML)**: Prompts for a custom HTML file path
   - **Debug TransmuteClient**: Launches TransmuteClient
   - **Attach to Process**: Attaches to a running process

4. **Start debugging**: Press `F5` or use the Debug panel to start debugging

The launch configuration automatically loads dSYM files and sets up the debugger for optimal debugging experience.

### Android

To build for Android, download the NDK first, then open a new terminal and configure the environment:

```sh
$ sh ./build/configure.sh android --print-toolchain
```

Apply the output command to the current terminal, then execute:

```sh
$ make android CLEAN=yes RELEASE=yes
```

## Logging

View JSAR runtime logs via `adb logcat -s jsar -s DEBUG`. Available log channels:

- `jsar` - Basic logs
- `jsar.error` - Error logs, including renderer errors; recommended to enable
- `renderer` - JSAR renderer logs
- `DEBUG` - Android crash logs; recommended to enable
- `TR_GLES` - OpenGLES-related logs

### Application Process

JSAR application logs won't appear in the `jsar` channel. Check them at `${runtime_directory}/logs/${process_id}.(out|err).log`.

### Document Events

To track document loading times, the following log format is provided:

```
08-01 15:56:34.370 13616 13638 D jsar.metrics: [1722498994370] DocumentEvent document#256 received spawnprocess +0ms
08-01 15:56:34.770 13616 13638 D jsar.metrics: [1722498994756] DocumentEvent document#256 received beforescripting +386ms
08-01 15:56:36.616 13616 13638 D jsar.metrics: [1722498996601] DocumentEvent document#256 received beforeloading +1845ms
08-01 15:56:37.500 13616 13638 D jsar.metrics: [1722498997479] DocumentEvent document#256 received load +878ms
08-01 15:56:37.503 13616 13638 D jsar.metrics: [1722498997480] DocumentEvent document#256 received loaded +1ms
08-01 15:56:39.061 13616 13638 D jsar.metrics: [1722498999048] DocumentEvent document#256 received DOMContentLoaded +1568ms
08-01 15:56:39.333 13616 13638 D jsar.metrics: [1722498999333] DocumentEvent document#256 received fcp +285ms
```

Format:

```
[timestamp] DocumentEvent document#<document_id> received <event_name> +<duration>
```

Where:
- Event names are defined in the Loading Performance section below
- Duration is the time difference from the previous event

## Debugging

### AOSP Device Debugging

Developers can debug using AOSP's adb tool with `adb shell setprop` and `adb logcat` commands.

**Enter debug mode:**

```sh
$ adb shell setprop jsar.debug.enabled yes
```

**Set next load URL:**

```sh
$ adb shell setprop jsar.example.url http://localhost:3000/spatial-externalmesh-glb.xsml
```

**Disable application resource caching:**

```sh
$ adb shell setprop jsar.resources.caching no
```

**Enable renderer command tracing:**

```sh
$ adb shell setprop jsar.renderer.tracing yes
```

After enabling, you can see renderer command calls in the `TR_GLES` channel, e.g.:

```
07-18 17:49:16.204 TR_GLES: [0] GL::ActiveTexture(0)
07-18 17:49:16.204 TR_GLES: [0] GL::BindTexture(0xde1, 24) for active(33984) program(24)
07-18 17:49:16.204 TR_GLES: [0] GL::ActiveTexture(1)
07-18 17:49:16.204 TR_GLES: [0] GL::BindTexture(0xde1, 3) for active(33985) program(24)
07-18 17:49:16.204 TR_GLES: [0] GL::DrawElements(mode=GL_TRIANGLES, count=5220, type=GL_UNSIGNED_INT, indices=0x0)
```

You can then use WebGL Inspector and similar tools for further debugging.

**Enable graphics interface debugging:**

When encountering rendering errors, enable graphics interface debug logs:

```sh
$ adb shell setprop jsar.renderer.graphics.debug yes
```

On OpenGLES platforms, this will show graphics interface calls in the `TR_GLES` channel using the [`KHR_debug`](https://registry.khronos.org/OpenGL/extensions/KHR/KHR_debug.txt) extension.

**Set client target frame rate:**

```sh
$ adb shell setprop jsar.renderer.target_app_fps 60
```

Default is 45 fps.

**Using LLM:**

```sh
## Configure LLM API model, e.g., `qwen-plus-latest`
$ adb shell setprop jsar.threepio.api.modelid your-llm-modelid
## Configure LLM API service organization/platform, e.g., `qwen`
$ adb shell setprop jsar.threepio.api.provider your-llm-provider
## Configure LLM API key
$ adb shell setprop jsar.threepio.api.key your-api-key
## Configure LLM API endpoint, e.g., `https://dashscope.aliyuncs.com/compatible-mode/v1`
$ adb shell setprop jsar.threepio.api.endpoint your-api-endpoint
```

Example with [Qwen model](https://github.com/QwenLM/Qwen):
```sh
$ adb shell setprop jsar.threepio.api.modelid qwen-plus-latest
$ adb shell setprop jsar.threepio.api.provider qwen
$ adb shell setprop jsar.threepio.api.key your-api-key
$ adb shell setprop jsar.threepio.api.endpoint https://dashscope.aliyuncs.com/compatible-mode/v1
```

### Using Chrome DevTools

Find in application process logs:

```
07-12 22:14:34.649 10055 10075 D jsar    : client(10142): Debugger listening on ws://0.0.0.0:9229/6f969656-e8a2-4e93-a904-c9e1fe940c13
```

This indicates the process is listening on port 9229 for Node.js Inspector debugging. For device debugging, first execute:

```sh
adb forward tcp:9229 tcp:9229
```

This maps the device's 9229 port to your computer. Then open `chrome://inspect` in Chrome DevTools, click `Configure...`, add `localhost:9229`, and click `inspect` to enter the debugging interface.

## Runtime Directory

The runtime directory stores runtime cache and temporary files. View with the `getprop` command:

```sh
$ adb shell getprop | grep jsar
[jsar.debug.enabled]: [yes]
[jsar.example.url]: [http://localhost:3000/spatial-externalmesh-glb.xsml]
[jsar.init.cache_directory]: [/path/to/your/cache/directory]
[jsar.renderer.target_app_fps]: [45]
[jsar.renderer.tracing]: [no]
[jsar.resources.caching]: [no]
```

The `jsar.init.cache_directory` is the runtime directory.

Runtime directory structure:

- `scripts` - Application process script files: `jsar-bootstrap.js`, `jsar-runtime.js`, etc.
- `.zones` - Shared memory data blocks for synchronizing gesture data between host and application processes; don't delete manually
- `.res_cache` - Resource cache (glTF files, textures, etc.); clearing this folder clears the cache
- `.stat` - Similar to `.zones`, mmap-based shared memory for viewing current state; don't delete

## Performance

### Preparation

To view performance metric logs:

```sh
$ adb logcat -s jsar.metrics
```

### Loading Performance

This section explains the following metrics:

| Metric | Description |
| ------ | ----------- |
| `spawnprocess` | Time when application process is created, i.e., after `fork()` call succeeds |
| `beforescripting` | Time when Node.js starts executing scripts (includes v8/Node.js internal startup time) |
| `beforeloading` | Time when document loading begins |
| `dispatchrequest` | Time when request is received |
| `load`/`loaded` | Time when document loading completes |
| `DOMContentLoaded` | Time when document dependencies (stylesheets, scripts, images, models, etc.) finish loading |
| `fcp` | First Contentful Paint - marks when renderer first receives a draw call request |
| `lcp` | Largest Contentful Paint - marks when the largest element (image, video, etc.) finishes painting |

The metrics `spawnprocess`, `beforescripting`, and `beforeloading` are pre-execution metrics. In actual user requests, the process starts from `dispatchrequest`.

**Application Loading Process:**

A JSAR application goes through these main stages during loading:

1. Start process and initialize connections for WebGL, WebXR, and other functionality
2. Initialize Node.js environment
3. Load JavaScript framework code (Babylon.js, JSAR-DOM, etc.)
4. Process document request (URL) and begin loading and rendering

Stages 1-3 have no direct dependency on the document itself, so JSAR uses preloading: when the runtime starts, it creates an application process and executes stages 1-3. When a new request arrives, it uses the already-initialized process and starts directly from stage 4, greatly reducing response time. Meanwhile, a new application process is initialized for subsequent requests.

Reference:
- https://web.dev/articles/fcp

### Runtime Performance

**How to view runtime processes:**

```sh
$ adb shell top
  PID USER         PR  NI VIRT  RES  SHR S[%CPU] %MEM     TIME+ ARGS
13301 u0_a132      20   0  21G  92M  28M S  6.3   1.2   0:03.55 jsar-client --h+
```

Find processes with arguments starting with "jsar-client" - these are JSAR application processes.

**How to view runtime performance:**

In the runtime directory, find the `perf` folder containing performance metrics for each load. For example, to check current fps:

```sh
$ cat /path/to/your/cache/directory/perf/host_fps
75
```

The `perf` folder has two parts:

- Files starting with numbers represent application process metrics (numbers are process PIDs, e.g., 1000, 1001)
- Other files represent service process (renderer) metrics, e.g., `host_fps` for renderer fps

`perf` file list:

| Filename | Description |
| -------- | ----------- |
| `host_fps` | Renderer frame rate, should match host engine render rate |
| `host_drawcalls_per_frame` | Average renderer draw commands (sum of all applications) |
| `host_drawcalls_count_per_frame` | Renderer draw command vertex count (sum of all applications) |
| `host_frame_duration` | Renderer frame time in milliseconds |
| `${pid}/fps` | Application process frame rate |
| `${pid}/xrframe_duration` | Application process XR frame time in milliseconds |
| `${pid}/long_xrframes` | Count of long render frames (exceeding 16ms) in application process |

## GPU Debugging Tools

JSAR provides specialized GPU debugging tools for real-time monitoring and debugging of renderer command buffer execution, performance metrics, and GPU resource usage.

### Starting the Debug Tool

1. **Enable Inspector Support**

   Ensure inspector support is enabled during compilation:

   ```bash
   $ make INSPECTOR=yes
   ```

2. **Start JSAR Runtime**

   After starting JSAR runtime, the Inspector server automatically starts on port 9423 with CDP (Chrome DevTools Protocol) support.

3. **Open Debug Interface**

   Open the debug tool interface in a browser:

   ```
   file:///path/to/jsar-runtime/fixtures/inspector-client/jsar_universal_rendering_server_debugger.html
   ```

   Or deploy the file to a local HTTP server.

### Connecting to Renderer

1. Click the "Connect" button in the debug interface
2. The tool will automatically connect to `ws://localhost:9423/devtools/inspector/1`
3. After successful connection, the status indicator turns green

### Features

#### 1. Rendering Trace Control

- **Enable Tracing**: Click "Enable Tracing" button to start monitoring command buffer
- **Disable Tracing**: Click "Disable Tracing" button to stop monitoring
- **Real-time Command Stream**: View real-time OpenGL/WebGL command execution when tracing is enabled

#### 2. Frame Rate Control

- **Set Client Frame Rate**: Use FPS controller to adjust target frame rate (1-120 FPS)
- **Real-time FPS Display**: View current frame rate in performance metrics panel

#### 3. Event Throttle Control

To avoid excessive events consuming bandwidth and affecting performance, the tool provides event throttling:

- **Set Event Frequency**: Use "Event Throttle" controller to adjust event send frequency (0.1-100 events/sec)
- **Performance Statistics**: View real-time event count and drop rate

#### 4. Performance Monitoring Panel

Real-time display of performance metrics:

- **FPS**: Current renderer frame rate
- **Runtime**: Renderer uptime
- **Commands/sec**: Number of render commands executed per second
- **GPU Resources**: Count of textures, buffers, programs, etc.
- **Event Statistics**: Number of events received and processing efficiency

#### 5. Command Buffer Tracing

Real-time display of command buffer details:

- **Command Type**: Syntax highlighting for GL/WebGL/XR commands
- **Parameter Details**: Shows parameters and related resources for each command
- **Timestamp**: Precise execution time of commands
- **Context Information**: Related programs, shaders, textures, VAOs, buffers, etc.

#### Available CDP Methods

| Method Name | Parameters | Description |
|------------|------------|-------------|
| `enableTracing` | None | Enable rendering trace |
| `disableTracing` | None | Disable rendering trace |
| `setClientFrameRate` | `frameRate`: 1-120 | Set client target frame rate |
| `getRendererInfo` | None | Get renderer status information |
| `getContentRenderers` | None | Get list of all content renderers |
| `getCommandBuffers` | None | Get command buffer debug information |
| `setEventThrottle` | `eventsPerSecond`: 0.1-100 | Set event throttle frequency |

### Troubleshooting

**Tool cannot connect to renderer:**
- Ensure JSAR runtime is started with Inspector support enabled
- Check if port 9423 is occupied
- Verify firewall settings allow local connections

**Cannot see command buffer events:**
- Ensure "Enable Tracing" is clicked
- Check if an application is running and generating render commands
- Adjust event throttle settings to increase event frequency

**Performance Impact:**
- Adjust event throttle frequency to appropriate level (recommended 1-10 events/sec)
- Disable tracing when not debugging
- Close debug tool window to free resources
