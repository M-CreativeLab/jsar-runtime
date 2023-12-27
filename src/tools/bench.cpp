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
   * Start initialize the runtime and execute the given xsml file.
   */
  for (int i = 0; i < 100; i++)
  {
    TransmuteNative_Initialize(false);
    TransmuteNative_SetLoggerCallback([](int fd, const char *text)
                                      {
        if (fd == 1)
            fprintf(stdout, "[jsar]: %s\n", text);
        else
            fprintf(stderr, "[jsar]: %s\n", text); });

    TransmuteNative_StartAndWaitAvailable();

    for (int j = 0; j < 3; j++)
    {
      const char *uuid = "bench";
      TransmuteNative_InstantiateGameObjectModelBuffer(uuid);

      char requestJson[1024];
      sprintf(requestJson, "{\"uri\":\"%s\", \"uuid\":\"%s\"}", xsmlUrl.c_str(), uuid);

      TransmuteNative_ExecuteAsync(requestJson);
      sleep(1);

      TransmuteNative_SetFirstFrameReady(uuid);
      /** Read the buffer */
      TransmuteNative_LockGameObjectModelBuffer(uuid);

      size_t size = TransmuteNative_GetGameObjectModelBufferSize(uuid);
      uint8_t *buffer = new uint8_t[size];

      TransmuteNative_LoadGameObjectModelBuffer(uuid, buffer);
      TransmuteNative_UnlockGameObjectModelBuffer(uuid);
      delete[] buffer;

      TransmuteNative_DisposeGameObjectModelBuffer(uuid);
    }
    sleep(1);

    TransmuteNative_Dispose(true);
    sleep(1);
  }

  TransmuteNative_Dispose(false);
  fprintf(stdout, "tests finished\n");
}
