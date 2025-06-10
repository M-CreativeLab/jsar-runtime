#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>

#include "./per_process.hpp"
#include "./entry.hpp"
#include "./hive_server.hpp"

// Constants for return values
constexpr int SUCCESS = 0;
constexpr int INVALID_ARGUMENTS = 1;

/**
 * Prints the command-line arguments to stdout.
 *
 * @param argc The number of arguments.
 * @param argv The array of arguments.
 */
void printArguments(int argc, char **argv)
{
  for (uint32_t i = 0; i < argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);
}

/**
 * Main entry point for the JSAR client.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return The exit code of the program.
 */
int main(int argc, char **argv)
{
  // Set the stdout/stderr to line-buffered mode.
  setvbuf(stdout, NULL, _IOLBF, 0);
  setvbuf(stderr, NULL, _IOLBF, 0);

  // Check for valid arguments
  if (argc <= 2)
  {
    printArguments(argc, argv);
    fprintf(stderr, "Exited, reason: invalid arguments to JSAR client.\n");
    return INVALID_ARGUMENTS;
  }

  // Enable backtrace and ignore SIGPIPE signal
  ENABLE_BACKTRACE();
  signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE signal and leverage EPIPE error for socket handling.

  // Determine the mode and run the client
  if (strcmp(argv[1], "--hive") == 0)
  {
    TrClientEntry clientEntry(TrClientMode::Hive);
    return clientEntry.run(argv[2], "");
  }
  else
  {
    // TODO: This might be deprecated.
    TrClientEntry clientEntry(TrClientMode::Client);
    std::string url = argv[1];
    return clientEntry.run(argv[2], url);
  }

  return SUCCESS;
}