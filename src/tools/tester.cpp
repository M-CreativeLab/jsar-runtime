#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "runtime/entry.hpp"

using namespace std;

#define TRANSMUTE_DEBUGGER 1

int main(int argc, char **argv)
{
  std::string xsmlUrl = "";
  if (argc >= 2)
  {
    xsmlUrl = argv[1];
    fprintf(stdout, "read xsml file from %s\n", xsmlUrl.c_str());
  }

  /**
   * Set the run mode which will be read at Node.js script.
   */
  if (argc == 2 || (argc > 2 && strcmp(argv[2], "--debug") != 0))
  {
    setenv("JSAR_RUN_MODE", "EXIT_ON_FIRST_FRAME", 1);
  }

  /**
   * Start initialize the runtime and execute the given xsml file.
   */
  if (TransmuteNative_Initialize(true) != 0)
  {
    fprintf(stderr, "failed to initialize the runtime\n");
    return -1;
  }
  TransmuteNative_SetLoggerCallback([](int fd, const char *text)
                                    {
        if (fd == 1)
            fprintf(stdout, "[jsar]: %s\n", text);
        else
            fprintf(stderr, "[jsar]: %s\n", text); });

  auto handle = TransmuteNative_StartAndWaitAvailable();
  fprintf(stdout, "runtime is available\n");

  const char *uuid = "test01";
  TransmuteNative_InstantiateGameObjectModelBuffer(uuid);

  char requestJson[1024];
  sprintf(requestJson, "{\"uri\":\"%s\", \"channelId\":\"%s\"}", xsmlUrl.c_str(), uuid);
  fprintf(stdout, "execute xsml file: %s\n", requestJson);

  TransmuteNative_ExecuteAsync(requestJson);
  sleep(10);

  TransmuteNative_SetFirstFrameReady(uuid);
  TransmuteNative_WaitAtExecuted(handle);

  /** Read the buffer */
  TransmuteNative_LockGameObjectModelBuffer(uuid);

  size_t size = TransmuteNative_GetGameObjectModelBufferSize(uuid);
  uint8_t *buffer = new uint8_t[size];

  TransmuteNative_LoadGameObjectModelBuffer(uuid, buffer);
  TransmuteNative_UnlockGameObjectModelBuffer(uuid);
  delete[] buffer;

  TransmuteNative_Dispose(false);
  fprintf(stdout, "tests finished\n");
}
