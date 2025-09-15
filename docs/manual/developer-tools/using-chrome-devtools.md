# Using Chrome DevTools

## Overview

This document describes how to use [Chrome DevTools](https://developer.chrome.com/docs/devtools/) to debug JSAR applications. Chrome DevTools is a set of developer tools built into the Chrome browser that provides powerful debugging, performance analysis, and development assistance features.

## What is Chrome DevTools

[Chrome DevTools](https://developer.chrome.com/docs/devtools/) is a set of developer tools built into the Google Chrome browser. Its main features include:

- **[Elements Panel](https://developer.chrome.com/docs/devtools/dom/)**: View and edit [DOM](https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model) structure and [CSS](https://developer.mozilla.org/en-US/docs/Web/CSS) styles
- **[Console Panel](https://developer.chrome.com/docs/devtools/console/)**: Execute [JavaScript](https://developer.mozilla.org/en-US/docs/Web/JavaScript) code and view log output
- **[Sources Panel](https://developer.chrome.com/docs/devtools/javascript/)**: Debug [JavaScript](https://developer.mozilla.org/en-US/docs/Web/JavaScript) code and set breakpoints
- **[Network Panel](https://developer.chrome.com/docs/devtools/network/)**: Monitor network requests and responses
- **[Performance Panel](https://developer.chrome.com/docs/devtools/performance/)**: Analyze application performance and rendering performance
- **[Memory Panel](https://developer.chrome.com/docs/devtools/memory/)**: Analyze memory usage
- **[Application Panel](https://developer.chrome.com/docs/devtools/application/)**: Manage storage, cache, and service workers

Chrome DevTools provides an open protocol called [CDP (Chrome DevTools Protocol)](https://chromedevtools.github.io/devtools-protocol/) that enables any browser or application supporting CDP to be debugged using Chrome DevTools. This means developers can use the familiar Chrome DevTools interface to debug applications running in different environments, not just Chrome browser.

**JSAR CDP Support**: JSAR currently supports a subset of the CDP protocol, allowing developers to use Chrome DevTools to debug JSAR applications with features like console logging, breakpoint debugging, and performance monitoring.

## Start using Chrome DevTools

### Official Release Builds

Official JSAR release builds have CDP (Chrome DevTools Protocol) enabled by default on port **9423**. You can directly connect to JSAR applications without any additional configuration.

### Self-Compiled Builds

If you're building JSAR from source, you need to enable the inspector during compilation:

```bash
# Enable inspector support during build
make darwin INSPECTOR=yes
```

### Connecting to DevTools

1. **Open Chrome Browser**
2. **Access Debug Page**: Enter [`chrome://inspect`](chrome://inspect) in the address bar
3. **Configure Network Target**: Click "Configure" and add `localhost:9423`
4. **Connect to Application**: Find your JSAR application in the "Remote Target" list and click "inspect"

![Chrome DevTools Inspect Screenshot](https://ar.rokidcdn.com/web-assets/pages/screenshot-chrome-devtools-inspect.png)

**For Remote Devices**

If your JSAR application is running on another device, you can replace `localhost` with the device's IP address. However, make sure to verify that the port is accessible:

```bash
# Check if the debug port is accessible
curl http://localhost:9423/json/list

# For remote device, replace localhost with the device IP
curl http://192.168.1.100:9423/json/list
```

Once connected, click "inspect" to start debugging your JSAR application with Chrome DevTools.

![Chrome DevTools Remote Debugging Screenshot](http://ar.rokidcdn.com/web-assets/pages/screenshot-chrome-devtools-console.png)
