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

void startWindowDragging(GLFWwindow* window)
{
  NSWindow* nsWindow = glfwGetCocoaWindow(window);
  if (nsWindow)
  {
    // Get the current mouse location in screen coordinates
    NSPoint mouseLocation = [NSEvent mouseLocation];
    
    // Create a mouse down event to initiate window dragging
    NSEvent* mouseDownEvent = [NSEvent mouseEventWithType:NSEventTypeLeftMouseDown
                                                location:mouseLocation
                                           modifierFlags:0
                                               timestamp:[[NSProcessInfo processInfo] systemUptime]
                                            windowNumber:[nsWindow windowNumber]
                                                 context:nil
                                             eventNumber:0
                                              clickCount:1
                                                pressure:1.0];
    
    // Start the window drag operation
    [nsWindow performWindowDragWithEvent:mouseDownEvent];
  }
}

bool isMouseInDragRegion(GLFWwindow* window, double xpos, double ypos)
{
  // Check if mouse is in the top 40 pixels of the window
  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);
  
  // GLFW uses top-left origin, so check if y-coordinate is within the top 40 pixels
  return (ypos >= 0 && ypos <= 40 && xpos >= 0 && xpos <= windowWidth);
}

} // extern "C"

#endif
