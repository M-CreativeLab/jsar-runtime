#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>

#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <runtime/embedder.hpp>

#include "./window_ctx.hpp"
#include "./screen_renderer.hpp"
#include "./content.hpp"
#include "./input_box.hpp"
#include "./stat_panel.hpp"
#include "./bar_component.hpp"
#include "./environment_renderer.hpp"

namespace jsar::example
{
  using namespace std;

  class DesktopEmbedder;
  class Content;

  /**
   * TransmuteBrowser manages multiple spatial content instances and provides
   * a screen-space GUI for interaction. It replaces the monolithic App class
   * with a more modular architecture.
   */
  class TransmuteBrowser
  {
  public:
    TransmuteBrowser() = default;

  public:
    void help(const char *programPath);
    bool init(int argc, char **argv);
    void prepareRenderTarget(int samples);
    void start();

    /**
     * Content management API
     */
    std::shared_ptr<Content> openContent(const std::string &url);
    bool closeContent(uint32_t contentId);
    std::shared_ptr<Content> getContentById(uint32_t contentId);
    std::shared_ptr<Content> getContentByPosition(const glm::vec3 &position, float tolerance = 0.5f);
    std::vector<std::shared_ptr<Content>> getAllContents() const;

  private:
    void processInput(GLFWwindow *window);
    void processFrameRateInput(GLFWwindow *window);
    void setupScreenRenderer();
    void renderContent();
    void updateContentDragging();

  public:
    // Configuration properties
    int width = 1600;
    int height = 900;
    bool xrEnabled = false;
    bool monoMode = true;
    bool multiPass = false;
    bool multisampleEnabled = true;
    bool envMapEnabled = true;
    string envMapPath = "";
    int nApps = 1;
    string requestUrl = "http://localhost:3000/spatial-element.xsml";

    // Frame rate control variables
    bool useVsync = false;
    int targetFps = 45;
    std::chrono::high_resolution_clock::time_point lastFrameTime;

  private:
    unique_ptr<WindowContext> windowCtx_;
    unique_ptr<DesktopEmbedder> embedder_;
    unique_ptr<EnvironmentRenderer> envRenderer_;
    unique_ptr<ScreenRenderer> screenRenderer_;

    GLuint render_target_;
    GLuint resolved_fbo_;

    // Content management
    std::unordered_map<uint32_t, std::shared_ptr<Content>> contents_;
    uint32_t nextContentId_ = 1;

    // Screen GUI components
    std::shared_ptr<StatPanel> statPanel_;
    std::shared_ptr<InputBox> urlInputBox_;
    std::shared_ptr<BarComponent> contentsBarComponent_;

    // Dragging state
    std::shared_ptr<Content> draggedContent_;
    glm::vec2 lastMousePosition_;
  };

  // Input processing functions
  void processInput(GLFWwindow *window, DesktopEmbedder &embedder);
  void processFrameRateInput(GLFWwindow *window, TransmuteBrowser &browser);
}