# 开发文档

## 日志

通过 `adb logcat -s jsar -s DEBUG` 可以查看 JSAR 运行时的日志，可用的通道：

- `jsar` 基础日志
- `jsar.error` 错误日志，包括渲染器报错都会统一打印到该通道，建议开启
- `renderer` JSAR 渲染器日志
- `DEBUG` Android 平台的崩溃日志，建议开启

### 应用进程

JSAR 会将每个应用进程的日志也打印到 `jsar` 中，比如：

```
07-12 22:12:23.326  9826  9850 D jsar    : client(9957): Detected a long frame(#135) takes 341ms in session(2)
```

它的格式表示为 `client(<进程ID>): <日志内容>`。

### 文档事件

为了方便获取每次文档加载耗时，提供了以下两个事件日志的打印：

- `loaded` 事件，表示文档加载完成，比如："dispatching XSML(2) event(loaded)"
- `fcp` 事件，表示首次内容绘制完成，比如："dispatching XSML(2) event(fcp)"

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

| 指标 | 说明 |
| ---- | ---- |
| `spawnprocess` | 表示创建应用进程的时间，即 `fork()` 调用成功后 |
| `beforescripting` | 表示 Node.js 开始执行脚本的时间（包括 v8/Node.js 内部启动时间） |
| `beforeloading` | 表示开始加载文档的时间 |
| `load`/`loaded` | 表示文档加载完成的时间 |
| `DOMContentLoaded` | 表示文档依赖的内容（如样式表、脚本、图片、模型等）加载完成 |
| `fcp`              | 即 First Contentful Paint，表示首次内容绘制完成，在 JSAR 运行时，它表示渲染器第一次接收到绘制请求（Draw Call）时标记 |
| `lcp`              | 即 Largest Contentful Paint，表示最大内容绘制完成，它是页面中最大的绘制元素（如图片、视频等）绘制完成的时间点        |

参考文档：

- https://web.dev/articles/fcp

### 运行时性能

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

| 文件名 | 说明 |
| ---- | ---- |
| `host_fps` | 渲染器帧率，一般来说需要与宿主引擎的渲染帧率一致 |
| `host_drawcalls_per_frame` | 渲染器的平均绘制指令数（所有应用总和） |
| `host_drawcalls_count_per_frame` | 渲染器的绘制指令的绘制顶点数（所有应用总和） |
| `host_frame_duration` | 渲染器的帧时间，单位为毫秒 |
| `${pid}/fps` | 应用进程帧率 |
| `${pid}/xrframe_duration` | 应用进程的 XR 帧时间，单位为毫秒 |
| `${pid}/long_xrframes` | 应用进程渲染过程中长渲染帧次数，长渲染帧表示耗时超过 16ms 的渲染帧 |
