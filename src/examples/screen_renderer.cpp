#include "./screen_renderer.hpp"
#include "./stat_panel.hpp"
#include "./input_box.hpp"

namespace jsar::example
{
  ScreenRenderer::ScreenRenderer(WindowContext *windowCtx)
      : windowCtx_(windowCtx)
  {
  }

  ScreenRenderer::~ScreenRenderer()
  {
    components_.clear();
  }

  void ScreenRenderer::addComponent(std::shared_ptr<ScreenComponent> component)
  {
    components_.push_back(component);
  }

  void ScreenRenderer::removeComponent(std::shared_ptr<ScreenComponent> component)
  {
    auto it = std::find(components_.begin(), components_.end(), component);
    if (it != components_.end())
    {
      components_.erase(it);
    }
  }

  void ScreenRenderer::render()
  {
    for (auto &component : components_)
    {
      component->render();
    }
  }

  void ScreenRenderer::handleInput(GLFWwindow *window, int key, int action)
  {
    for (auto &component : components_)
    {
      component->handleInput(window, key, action);
    }
  }

  void ScreenRenderer::handleMouseButton(GLFWwindow *window, int button, int action, double xpos, double ypos)
  {
    for (auto &component : components_)
    {
      component->handleMouseButton(window, button, action, xpos, ypos);
    }
  }

  void ScreenRenderer::handleCharInput(GLFWwindow *window, unsigned int codepoint)
  {
    for (auto &component : components_)
    {
      component->handleCharInput(window, codepoint);
    }
  }

  std::shared_ptr<StatPanel> ScreenRenderer::createStatPanel()
  {
    auto panel = std::make_shared<StatPanel>(windowCtx_);
    addComponent(panel);
    return panel;
  }

  std::shared_ptr<InputBox> ScreenRenderer::createInputBox(const std::string &placeholder,
                                                           std::function<void(const std::string &)> onSubmit)
  {
    auto inputBox = std::make_shared<InputBox>(windowCtx_, placeholder, onSubmit);
    addComponent(inputBox);
    return inputBox;
  }
}