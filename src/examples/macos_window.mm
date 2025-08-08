#ifdef __APPLE__

#import <Cocoa/Cocoa.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

// Store window position and mouse offset for dragging
static NSPoint dragStartMouseLocation;
static NSPoint dragStartWindowOrigin;
static NSPoint mouseToWindowOffset; // Offset from mouse to window origin when dragging starts
static bool isDragging = false;

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
    // Store the initial mouse and window positions for dragging
    dragStartMouseLocation = [NSEvent mouseLocation];
    dragStartWindowOrigin = [nsWindow frame].origin;
    
    // Calculate the offset from mouse position to window origin
    // This ensures 1:1 mapping between mouse movement and window movement
    mouseToWindowOffset = NSMakePoint(
      dragStartWindowOrigin.x - dragStartMouseLocation.x,
      dragStartWindowOrigin.y - dragStartMouseLocation.y
    );
    
    isDragging = true;
  }
}

void updateWindowDragging(GLFWwindow* window)
{
  if (!isDragging)
    return;
    
  NSWindow* nsWindow = glfwGetCocoaWindow(window);
  if (nsWindow)
  {
    // Get the current mouse location
    NSPoint currentMouseLocation = [NSEvent mouseLocation];
    
    // Calculate the new window position by applying the fixed offset
    // This ensures 1:1 mapping between mouse movement and window movement
    NSPoint newOrigin = NSMakePoint(
      currentMouseLocation.x + mouseToWindowOffset.x,
      currentMouseLocation.y + mouseToWindowOffset.y
    );
    
    // Use setFrameOrigin for immediate positioning without animation
    [nsWindow setFrameOrigin:newOrigin];
  }
}

void stopWindowDragging(GLFWwindow* window)
{
  isDragging = false;
}

bool isMouseInDragRegion(GLFWwindow* window, double xpos, double ypos, int dragRegionHeight)
{
  // Check if mouse is in the top dragRegionHeight pixels of the window  
  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);
  
  // GLFW uses top-left origin, so check if y-coordinate is within the top dragRegionHeight pixels
  return (ypos >= 0 && ypos <= dragRegionHeight && xpos >= 0 && xpos <= windowWidth);
}

} // extern "C"

#endif
