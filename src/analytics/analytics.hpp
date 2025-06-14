#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <chrono>
#include <string>
#include <filesystem>
#include <map>
#include <algorithm>

namespace analytics
{
  using namespace std;
  using namespace chrono;

  class Analytics;
  class PerformanceMeasure
  {
  public:
    PerformanceMeasure(const char *name, Analytics *analytics, int writeFd);
    ~PerformanceMeasure();

  public:
    void stop();
    microseconds duration();

  private:
    void write();

  public:
    const char *name;
    high_resolution_clock::time_point start;
    high_resolution_clock::time_point end;

  private:
    int fd;
    Analytics *analytics;
  };

  class Analytics
  {
  public:
    Analytics();
    ~Analytics();

  public:
    void start();
    void stop();
    PerformanceMeasure createPerformanceMeasure(const char *name);

  private:
    bool ensurePipeDir(const std::string &path);
    int createNamedPipe(const char *name, int *fd);
    void writeToPipe(int fd, string text);
    void closeNamedPipe(int fd);

  private:
    high_resolution_clock::time_point startTime;
    map<string, int> namedPipes;
    bool disabled = false;

    friend class PerformanceMeasure;
  };
} // namespace analytics
