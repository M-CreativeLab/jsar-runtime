#ifdef __APPLE__

#import <Cocoa/Cocoa.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

extern "C" {

void customizeMacOSWindow(GLFWwindow* window)
{
  NSWindow* nsWindow = glfwGetCocoaWindow(window);
  if (nsWindow)
  {
    // Set window style to hide title bar but keep system buttons and rounded corners
    [nsWindow setStyleMask:([nsWindow styleMask] | NSWindowStyleMaskFullSizeContentView)];
    [nsWindow setTitlebarAppearsTransparent:YES];
    [nsWindow setTitleVisibility:NSWindowTitleHidden];
    
    // Preserve system buttons and rounded corners
    // The window will still have close/minimize/maximize buttons but no title bar
  }
}

} // extern "C"

#endif
