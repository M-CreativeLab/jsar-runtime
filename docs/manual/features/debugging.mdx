# Debugging your application

Each application in JSAR is a standalone [Node.js][nodejs] process, and each process will start a debugging server to provide the debugging interface for the developers. The debugging server is based on the [Chrome DevTools Protocol][cdp], which allows developers to debug the application in the browser.

[nodejs]: https://nodejs.org/
[cdp]: https://chromedevtools.github.io/devtools-protocol/

## Prerequisites

Before you start debugging, you need to learn how to install adb tool and use `logcat` to view the console output.

### Install adb

The `adb` tool is a command-line utility that allows you to communicate with an Android Open Source Project (AOSP) device. You can install the `adb` tool by following the instructions on the [Android Developers website][adb].

When you have installed the `adb` tool, you can use the following command to connect to the device via network:

```sh
adb connect <device-ip>
```

### View the console output

JSAR outputs the console messages to the Android logcat, you can use the following command to view the console output:

```sh
adb logcat -s jsar
```

Then you can see the console output in the terminal:

```sh
07-12 22:12:23.326  9826  9850 D jsar    : client(9957): Detected a long frame(#135) takes 341ms in session(2)
```

### Other `logcat` channels

There are other log channels used by JSAR, you can listen to them on-demand: `adb logcat -s jsar -s jsar.error`, the following table lists the log channels:

| Channel      | Description |
| ------------ | --- |
| `jsar`       | The main log channel for JSAR |
| `jsar.error` | The error log channel for JSAR |
| `TR_GLES`    | The OpenGL ES log channel |

[adb]: https://developer.android.com/studio/command-line/adb

## Looking for the debugging server

When you start the application, you will see the following message in the console:

```sh
07-12 22:14:34.649 10055 10075 D jsar    : client(10142): Debugger listening on ws://0.0.0.0:9229/6f969656-e8a2-4e93-a904-c9e1fe940c13
```

You can extract the debugging server port from the message, in this case, the port is `9229`.

```sh
adb forward tcp:9229 tcp:9229
```

The above command forwards the port `9229` from the device to the host, then you can open the Chrome browser and navigate to `chrome://inspect` to connect to the debugging server.
