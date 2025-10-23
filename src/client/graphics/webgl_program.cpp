#include "./webgl_program.hpp"

namespace endor
{
  namespace client_graphics
  {
    using namespace std;

    void WebGLProgram::waitForCompleted(int timeout) const
    {
      std::unique_lock<std::mutex> lock(setCompletedMutex_);
      setCompletedCv_.wait_for(lock, std::chrono::milliseconds(timeout), [this]()
                               { return !incomplete_; });
    }

    void WebGLProgram::setCompleted(bool linkStatus)
    {
      linkStatus_ = linkStatus;
      incomplete_ = false;
      {
        std::unique_lock<std::mutex> lock(setCompletedMutex_);
        setCompletedCv_.notify_all();
      }
    }

    bool WebGLProgram::getLinkStatus(bool sync) const
    {
      if (sync)
        waitForCompleted();
      return linkStatus_;
    }

    bool WebGLProgram::getValidateStatus(bool sync) const
    {
      if (sync)
        waitForCompleted();
      return validateStatus_;
    }

    void WebGLProgram::printInfo() const
    {
      cout << "Program " << id << " info:" << endl;
      cout << "Link status: " << (linkStatus_ ? "true" : "false") << endl;
      cout << "Active attributes(" << activeAttribs_.size() << "):" << endl;
      for (auto &pair : activeAttribs_)
      {
        auto activeInfo = pair.second;
        cout << "  " << pair.first << ": " << activeInfo.name << ", type: " << activeInfo.type << ", size: " << activeInfo.size << endl;
      }

      cout << "Active uniforms(" << activeUniforms_.size() << "):" << endl;
      for (auto &pair : activeUniforms_)
      {
        auto activeInfo = pair.second;
        cout << "  " << pair.first << ": " << activeInfo.name << ", type: " << activeInfo.type << ", size: " << activeInfo.size << endl;
      }

      cout << "Attribute locations:" << endl;
      for (auto &pair : attribLocations_)
        cout << "  " << pair.first << ": " << pair.second.index.value_or(-1) << endl;

      cout << "Uniform locations:" << endl;
      for (auto &pair : uniformLocations_)
        cout << "  " << pair.first << ": " << pair.second.index.value_or(-1) << endl;
    }
  }
} // namespace endor
