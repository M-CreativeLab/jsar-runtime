#ifdef __APPLE__

#import <Cocoa/Cocoa.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

// Store window position and mouse offset for dragging
static NSPoint dragStartMouseLocation;
static NSPoint dragStartWindowOrigin;
static NSPoint lastMouseLocation;
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
    lastMouseLocation = dragStartMouseLocation;
    dragStartWindowOrigin = [nsWindow frame].origin;
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
    // Get the current mouse location for immediate, responsive dragging
    NSPoint currentMouseLocation = [NSEvent mouseLocation];
    
    // Calculate delta from last position for smoother, more responsive movement
    NSPoint delta = NSMakePoint(
      currentMouseLocation.x - lastMouseLocation.x,
      currentMouseLocation.y - lastMouseLocation.y
    );
    
    // Only update if there's actual movement to avoid unnecessary operations
    if (delta.x != 0.0 || delta.y != 0.0)
    {
      // Get current window frame and update origin by delta
      NSRect currentFrame = [nsWindow frame];
      NSPoint newOrigin = NSMakePoint(
        currentFrame.origin.x + delta.x,
        currentFrame.origin.y + delta.y
      );
      
      // Use setFrameOrigin for immediate positioning without animation
      [nsWindow setFrameOrigin:newOrigin];
      
      // Update last mouse position for next delta calculation
      lastMouseLocation = currentMouseLocation;
    }
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
