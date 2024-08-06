#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./per_process.hpp"
#include "./entry.hpp"
#include "./hive_server.hpp"

using namespace std;

int main(int argc, char **argv)
{
  // Set the stdout/stderr to line-buffered mode.
  setvbuf(stdout, NULL, _IOLBF, 0);
  setvbuf(stderr, NULL, _IOLBF, 0);

  for (uint32_t i = 0; i < argc; i++)
    fprintf(stdout, "argv[%d] = %s\n", i, argv[i]);

  if (argc <= 2)
  {
    fprintf(stderr, "Exited, reason: invalid arguments to JSAR client.\n");
    return 1;
  }

  ENABLE_BACKTRACE();
  signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE signal and leverage EPIPE error to socket handling.

  fprintf(stdout, "Copy the following command to restart the client:\n");
  fprintf(stdout, "  %s '%s' '%s'\n", argv[0], argv[1], argv[2]);

  if (strcmp(argv[1], "--hive") == 0)
  {
    TrClientEntry clientEntry(TrClientMode::Hive);
    return clientEntry.run(argv[2], "");
  }
  else
  {
    // TODO: This might be deprecated.
    TrClientEntry clientEntry(TrClientMode::Client);
    string url = argv[1];
    return clientEntry.run(argv[2], url);
  }
}
