#include <assert.h>
#include <stdio.h>
#include <string>
#include "common/command_buffers/shared.hpp"

using namespace commandbuffers;

class FooCommand
{
public:
  FooCommand(int a) : a(a)
  {
  }

public:
  int a;
};

int main()
{
  FooCommand foo(10);
  TrCommandBufferMessage message(CommandBufferType::COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ, sizeof(FooCommand), &foo);

  std::string str = "foobar";
  message.addStringSegment(str);

  void *data;
  size_t size;
  if (message.serialize(&data, &size))
  {
    assert(size > 0);
    fprintf(stdout, "message: [");
    for (size_t i = 0; i < size; i++)
      fprintf(stdout, "%x ", ((char *)data)[i]);
    fprintf(stdout, "]\n");
  }

  {
    FooCommand* foo2 = nullptr;
    {
      TrCommandBufferMessage m2;
      m2.deserialize((char *)data, size);
      foo2 = m2.createInstanceFromBase<FooCommand>(); // new instance

      auto str2 = m2.getSegment(0)->c_str();
      assert(strcmp(str2, str.c_str()) == 0);
    }
    assert(foo2 != nullptr);
    assert(foo2->a == 10);
    delete foo2;
  }
  return 0;
}
