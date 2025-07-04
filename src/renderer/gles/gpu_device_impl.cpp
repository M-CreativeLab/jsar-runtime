#include <string>
#include <memory>
#include <sstream>

#include "./common.hpp"
#include "./gpu_device_impl.hpp"
#include "./gpu_command_encoder_impl.hpp"

namespace gles
{
  using namespace std;
  using namespace commandbuffers;

  GPUQueueImpl::GPUQueueImpl()
      : GPUQueue()
  {
    // Initialize the queue if needed.
    // For GLES, this might not require any specific initialization.
  }

  void GPUQueueImpl::submit(const std::vector<commandbuffers::GPUCommandBuffer> &command_buffers)
  {
    cout << "Submitting GPU command buffers to the queue: " << command_buffers.size() << endl;
  }

  GPUDeviceImpl::GPUDeviceImpl()
      : GPUDevice()
  {
    queue_ = make_unique<GPUQueueImpl>();

    auto get_gl_string = [](GLenum name) -> string
    {
      const char *str = (const char *)glGetString(name);
      return str == nullptr ? "" : string(str);
    };

    // Initialize adapter_info_, features_, and limits_ with appropriate values.
    adapter_info_.architecture = "";
    adapter_info_.vendor = get_gl_string(GL_VENDOR);
    adapter_info_.device = get_gl_string(GL_RENDERER);
    {
      stringstream description_ss;
      description_ss << adapter_info_.vendor << " "
                     << adapter_info_.device << " "
                     << get_gl_string(GL_VERSION) << " "
                     << "GLSL(" << get_gl_string(GL_SHADING_LANGUAGE_VERSION) << ") ";

      description_ss << "Extensions(";
      GLint extensions_len;
      glGetIntegerv(GL_NUM_EXTENSIONS, &extensions_len);
      for (int i = 0; i < extensions_len; i++)
      {
        const GLubyte *s = glGetStringi(GL_EXTENSIONS, i);
        if (s != nullptr)
        {
          description_ss << string((const char *)s);
          if (i < extensions_len - 1)
            description_ss << ", ";
        }
      }
      description_ss << ")";
      adapter_info_.description = description_ss.str();
    }
    DEBUG(LOG_TAG_RENDERER, "GPU Device Info: %s", adapter_info_.toString().c_str());
  }

  unique_ptr<GPUCommandEncoder> GPUDeviceImpl::createCommandEncoder(optional<string> label)
  {
    return unique_ptr<GPUCommandEncoder>(new GPUCommandEncoderImpl(label.value_or("")));
  }
}
