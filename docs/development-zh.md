# 开发文档

## 编译

当前开发仅支持使用 macOS 开发，编译到 macOS 和 Android 平台。

### macOS

```
$ make darwin CLEAN=yes
```

### Android

编译 Android 平台需要下载 NDK，下载完成后，新开一个终端配置参数：

```sh
$ sh ./build/configure.sh android --print-toolchain
```

将输出的命令设置到当前终端，然后执行：

```sh
$ make android CLEAN=yes RELEASE=yes
```

## 日志

通过 `adb logcat -s jsar -s DEBUG` 可以查看 JSAR 运行时的日志，可用的通道：

- `jsar` 基础日志
- `jsar.error` 错误日志，包括渲染器报错都会统一打印到该通道，建议开启
- `renderer` JSAR 渲染器日志
- `DEBUG` Android 平台的崩溃日志，建议开启
- `TR_GLES` 与 OpenGLES 相关的日志

### 应用进程

JSAR 的应用日志不会在 `jsar` 通道中出现，需要开发者在 `${运行时目录}/logs/${应用进程ID}.(out|err).log` 中查看。

### 文档事件

为了方便获取每次文档加载耗时，提供了以下日志打印：

```
08-01 15:56:34.370 13616 13638 D jsar.metrics: [1722498994370] DocumentEvent document#256 received spawnprocess +0ms
08-01 15:56:34.770 13616 13638 D jsar.metrics: [1722498994756] DocumentEvent document#256 received beforescripting +386ms
08-01 15:56:36.616 13616 13638 D jsar.metrics: [1722498996601] DocumentEvent document#256 received beforeloading +1845ms
08-01 15:56:37.500 13616 13638 D jsar.metrics: [1722498997479] DocumentEvent document#256 received load +878ms
08-01 15:56:37.503 13616 13638 D jsar.metrics: [1722498997480] DocumentEvent document#256 received loaded +1ms
08-01 15:56:39.061 13616 13638 D jsar.metrics: [1722498999048] DocumentEvent document#256 received DOMContentLoaded +1568ms
08-01 15:56:39.333 13616 13638 D jsar.metrics: [1722498999333] DocumentEvent document#256 received fcp +285ms
```

格式如下：

```
[时间戳] DocumentEvent document#<文档ID> received <事件名称> +<耗时>
```

其中：

- 事件名称见下面的加载性能的部分
- 耗时为当前事件与上一个事件的时间差

## 调试

### AOSP 设备调试

开发者可以基于 AOSP 的 adb 工具进行调试，需要使用到 `adb shell setprop` 和 `adb logcat` 命令。

**进入调试模式**

```sh
$ adb shell setprop jsar.debug.enabled yes
```

**设置下一次加载地址**

```sh
$ adb shell setprop jsar.example.url http://localhost:3000/spatial-externalmesh-glb.xsml
```

**禁用应用资源缓存**

```sh
$ adb shell setprop jsar.resources.caching no
```

**打开渲染器指令跟踪**

```sh
$ adb shell setprop jsar.renderer.tracing yes
```

开启后可以在 `TR_GLES` 通道看到渲染器的指令调用，比如：

```
07-18 17:49:16.204 TR_GLES: [0] GL::ActiveTexture(0)
07-18 17:49:16.204 TR_GLES: [0] GL::BindTexture(0xde1, 24) for active(33984) program(24)
07-18 17:49:16.204 TR_GLES: [0] GL::ActiveTexture(1)
07-18 17:49:16.204 TR_GLES: [0] GL::BindTexture(0xde1, 3) for active(33985) program(24)
07-18 17:49:16.204 TR_GLES: [0] GL::DrawElements(mode=GL_TRIANGLES, count=5220, type=GL_UNSIGNED_INT, indices=0x0)
07-18 17:49:16.204 TR_GLES:     Program: 24
07-18 17:49:16.204 TR_GLES:     Program: LINK_STATUS=1
07-18 17:49:16.204 TR_GLES:     Program: VALIDATE_STATUS=0
07-18 17:49:16.204 TR_GLES:     Element Array Buffer: 92
07-18 17:49:16.204 TR_GLES:     Active Attribute(0): Enabled=Yes Size=1 Type=0x8b51 "normal"
07-18 17:49:16.204 TR_GLES:     Active Attribute(1): Enabled=Yes Size=1 Type=0x8b50 "uv"
07-18 17:49:16.204 TR_GLES:     Active Attribute(2): Enabled=Yes Size=1 Type=0x8b52 "matricesWeights"
07-18 17:49:16.204 TR_GLES:     Active Attribute(3): Enabled=Yes Size=1 Type=0x8b51 "position"
07-18 17:49:16.204 TR_GLES:     Active Attribute(4): Enabled=Yes Size=1 Type=0x8b52 "matricesIndices"
```

以及后续可以使用 WebGL Inspector 等工具进行调试。

**开启图形接口调试**

当遇到渲染错误时，需要开启图形接口的调试日志，可以通过以下命令开启：

```sh
$ adb shell setprop jsar.renderer.graphics.debug yes
```

在 OpenGLES 平台上，开启后会在 `TR_GLES` 通道看到图形接口的调用，比如：

```
07-18 17:49:16.204 TR_GLES: [KHR_debug] GL::DebugMessageCallback(0x0, 0x0, 0x0, 0x0)
```

正如上面所见，在 OpenGLES 开启调试，会使用 [`KHR_debug`](https://registry.khronos.org/OpenGL/extensions/KHR/KHR_debug.txt) 扩展来输出调试信息。

**设置客户端目标帧率**

```sh
$ adb shell setprop jsar.renderer.target_app_fps 60
```

默认为 45 帧。

### 使用 Chrome DevTools 调试

在应用进程日志中找到：

```
07-12 22:14:34.649 10055 10075 D jsar    : client(10142): Debugger listening on ws://0.0.0.0:9229/6f969656-e8a2-4e93-a904-c9e1fe940c13
```

它表示当前进程监听了 9229 端口号用于 Node.js 的 Inspector 调试，如果用于设备调试，需要先执行：

```sh
adb forward tcp:9229 tcp:9229
```

将设备的 9229 端口映射到工作电脑，然后通过 `chrome://inspect` 打开 Chrome DevTools，点击 `Configure...`，添加 `localhost:9229`，然后点击 `inspect` 即可进入调试界面。

### 使用 LLM

```sh
##配置Llm Api Model Id
## model id（模型 ID）指的是你所使用的大语言模型（LLM，Large Language Model）一标识符。它用于指定 JSAR 运行时与哪个具体的 LLM 模型进行交互。
$ adb shell setprop jsar.setup.theepio.api.modelid your-llm-modelid
##配置Llm Api Provier
##provider（提供者）指的是提供 LLM 服务的组织或平台。它用于指定 JSAR 运行时与哪个具体的 LLM 服务提供者进行交互。
$ adb shell setprop jsar.setup.theepio.api.provider your-llm-provider
##配置Llm Api Key
$ adb shell setprop jsar.setup.theepio.api.key your-api-key
##配置Llm End Point
##Endpoint（端点）指的是 LLM 服务的访问地址。
$ adb shell setprop jsar.setup.threepio.api.endpoint your-api-endpoint
```

我们以阿里大模型举例  
```sh
$ adb shell setprop jsar.setup.threepio.api.modelid qwen-plus-latest
$ adb shell setprop jsar.setup.threepio.api.provider qwen
$ adb shell setprop jsar.setup.threepio.api.key your-api-key
$ adb shell setprop jsar.setup.threepio.api.endpoint https://dashscope.aliyuncs.com/compatible-mode/v1
```

## 运行时目录

运行时目录用于存放运行时所需的缓存和临时文件，可通过 `getprop` 命令查看：

```sh
$ adb shell getprop | grep jsar
[jsar.debug.enabled]: [yes]
[jsar.example.url]: [http://localhost:3000/spatial-externalmesh-glb.xsml]
[jsar.init.cache_directory]: [/path/to/your/cache/directory]
[jsar.renderer.target_app_fps]: [45]
[jsar.renderer.tracing]: [no]
[jsar.resources.caching]: [no]
```

其中 `jsar.init.cache_directory` 即运行时目录。

以下是运行时目录的子目录结构：

- `scripts` 存放应用进程的脚本文件：`jsar-bootstrap.js`, `jsar-runtime.js` 等
- `.zones` 存放共享内存数据块，用于在 Host 进程与应用进程之间同步手势数据，不建议手动删除
- `.res_cache` 存放资源缓存，比如 glTF 文件、纹理等，清空该文件夹内容等价于清空缓存
- `.stat` 与 `.zones` 类似，也是基于 mmap 的共享内存，供开发者查看当前状态，不建议删除

## 性能

### 准备工作

为了可以查看性能指标日志：

```sh
$ adb logcat -s jsar.metrics
```

### 加载性能

本节将针对以下几个指标作出说明。

| 指标               | 说明                                                                                                                 |
| ------------------ | -------------------------------------------------------------------------------------------------------------------- |
| `spawnprocess`     | 表示创建应用进程的时间，即 `fork()` 调用成功后                                                                       |
| `beforescripting`  | 表示 Node.js 开始执行脚本的时间（包括 v8/Node.js 内部启动时间）                                                      |
| `beforeloading`    | 表示开始加载文档的时间                                                                                               |
| `dispatchrequest`  | 表示开始接收到请求的时间                                                                                             |
| `load`/`loaded`    | 表示文档加载完成的时间                                                                                               |
| `DOMContentLoaded` | 表示文档依赖的内容（如样式表、脚本、图片、模型等）加载完成                                                           |
| `fcp`              | 即 First Contentful Paint，表示首次内容绘制完成，在 JSAR 运行时，它表示渲染器第一次接收到绘制请求（Draw Call）时标记 |
| `lcp`              | 即 Largest Contentful Paint，表示最大内容绘制完成，它是页面中最大的绘制元素（如图片、视频等）绘制完成的时间点        |

其中 `spawnprocess`、`beforescripting` 以及 `beforeloading` 三个指标是预执行时的指标，在每次真实的用户请求过程中是从 `dispatchrequest` 开始的。

**应用加载流程**

一个 JSAR 应用在加载过程中，需要经历以下几个主要阶段：

1. 启动进程并初始化一些连接，以让 WebGL、WebXR 以及其他功能能正常工作
2. 初始化 Node.js 环境
3. 加载 JavaScript 框架代码，如 Babylon.js、JSAR-DOM 等
4. 处理文档请求（URL）并开始加载并渲染

其中阶段一至阶段三与文档本身并不直接的依赖关系，即可以在不需要知道当前请求的 URL 的情况下完成，因此在 JSAR 运行过程中采取了预加载的方式，从运行时启动时就默认创建了一个应用进程，并执行阶段1-3，当有新的请求到来时，会直接使用已经初始化完成的应用进程，直接从阶段四开始，这样每次响应的耗时就能大量节省下来，与此同时，又会再初始化一个新的应用进程并执行预加载用于后续的请求响应。

参考文档：

- https://web.dev/articles/fcp

### 运行时性能

**如何查看运行时进程？**

```sh
$ adb shell top
  PID USER         PR  NI VIRT  RES  SHR S[%CPU] %MEM     TIME+ ARGS
13301 u0_a132      20   0  21G  92M  28M S  6.3   1.2   0:03.55 jsar-client --h+
```

找到如上进程，参数为 "jsar-client" 开头的部分为 JSAR 应用进程。

**如何查看运行时性能？**

在运行时目录下找到 `perf` 文件夹，里面存放了每次加载的性能指标，比如希望查看当前 fps 情况：

```sh
$ cat /path/to/your/cache/directory/perf/host_fps
75
```

`perf` 文件夹主要分为两个部分：

- 以数字开头的代表应用进程的性能指标，数字代表对应的应用进程 Pid，比如 1000, 1001 等
- 其他文件则表示服务进程（即渲染器）的性能指标，比如 `host_fps` 表示渲染器的 fps 等

`perf` 的具体列表如下：

| 文件名                           | 说明                                                               |
| -------------------------------- | ------------------------------------------------------------------ |
| `host_fps`                       | 渲染器帧率，一般来说需要与宿主引擎的渲染帧率一致                   |
| `host_drawcalls_per_frame`       | 渲染器的平均绘制指令数（所有应用总和）                             |
| `host_drawcalls_count_per_frame` | 渲染器的绘制指令的绘制顶点数（所有应用总和）                       |
| `host_frame_duration`            | 渲染器的帧时间，单位为毫秒                                         |
| `${pid}/fps`                     | 应用进程帧率                                                       |
| `${pid}/xrframe_duration`        | 应用进程的 XR 帧时间，单位为毫秒                                   |
| `${pid}/long_xrframes`           | 应用进程渲染过程中长渲染帧次数，长渲染帧表示耗时超过 16ms 的渲染帧 |
