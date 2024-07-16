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

开发者可以基于 AOSP 的 adb 工具进行调试，需要使用到 `adb setprop` 和 `adb logcat` 命令。

进入调试模式：

```sh
$ adb setprop jsar.debug.enabled yes
```

**设置下一次加载地址**

```sh
$ adb setprop jsar.example.url http://localhost:3000/spatial-externalmesh-glb.xsml
```

**禁用应用资源缓存**

```sh
$ adb setprop jsar.resources.caching no
```

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

运行时目录用于存放运行时所需的缓存和临时文件。要获取 `applicationCacheDirectory`，首先打开日志输出，找到如下：

```
07-12 22:14:52.517 10055 10075 D jsar    : client(10174): The context init is: ClientContext {
07-12 22:14:52.517 10055 10075 D jsar    : client(10174):   id: 2,
07-12 22:14:52.517 10055 10075 D jsar    : client(10174):   url: 'http://0.0.0.0:3000/spatial-externalmesh-glb.xsml',
07-12 22:14:52.517 10055 10075 D jsar    : client(10174):   applicationCacheDirectory: '/path/to/your/cache/directory',
07-12 22:14:52.517 10055 10075 D jsar    : client(10174):   httpsProxyServer: '',
07-12 22:14:52.517 10055 10075 D jsar    : client(10174):   webglVersion: 2,
07-12 22:14:52.517 10055 10075 D jsar    : client(10174):   xrDevice: { enabled: true, active: true, stereoRenderingMode: 'multipass' }
07-12 22:14:52.517 10055 10075 D jsar    : client(10174): }
```

其中 `applicationCacheDirectory` 就是运行时目录。

以下是运行时目录的子目录结构：

- `.zones` 存放共享内存数据块，用于在 Host 进程与应用进程之间同步手势数据，不建议手动删除
- `.res_cache` 存放资源缓存，比如 glTF 文件、纹理等，清空该文件夹内容等价于清空缓存
- `.stat` 与 `.zones` 类似，也是基于 mmap 的共享内存，供开发者查看当前状态，不建议删除
