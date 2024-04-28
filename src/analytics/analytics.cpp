#include "analytics/analytics.hpp"
#include "debug.hpp"

namespace analytics
{
  using namespace std;
  using namespace chrono;
  using namespace filesystem;

  const char *LOG_KEY = "jsar.analytics";
  const char *PIPE_PREFIX = "/dev/shm/jsar/analytics";

  PerformanceMeasure::PerformanceMeasure(const char *name, Analytics *analytics, int writeFd)
      : name(name), analytics(analytics), fd(writeFd)
  {
    start = high_resolution_clock::now();
  }

  PerformanceMeasure::~PerformanceMeasure()
  {
    fd = -1;
  }

  void PerformanceMeasure::stop()
  {
    end = high_resolution_clock::now();
    write();
  }

  void PerformanceMeasure::write()
  {
    auto duration = duration_cast<microseconds>(end - start);
    auto durationMsCount = duration.count();
    if (fd != -1)
      analytics->writeToPipe(fd, to_string(duration.count()));
  }

  Analytics::Analytics()
  {
    start();
  }

  Analytics::~Analytics()
  {
    stop();
  }

  void Analytics::start()
  {
    startTime = high_resolution_clock::now();
  }

  void Analytics::stop()
  {
    for (auto it = namedPipes.begin(); it != namedPipes.end(); it++)
    {
      auto name = it->first;
      auto fd = it->second;
      close(fd);
      unlink(name.c_str());
    }
    namedPipes.clear();
  }

  PerformanceMeasure Analytics::createPerformanceMeasure(const char *name)
  {
    int writeFd = -1;

    // Works only if the analytics is not disabled
    if (disabled == false)
    {
      if (namedPipes.find(name) != namedPipes.end())
      {
        writeFd = namedPipes[name];
      }
      else
      {
        if (createNamedPipe(name, &writeFd) == -1)
          disabled = true; // disable analytics if failed to create pipe to avoid frequent error messages
      }
    }
    return PerformanceMeasure(name, this, writeFd);
  }

  bool Analytics::ensurePipeDir(const std::string &path)
  {
    filesystem::path dirPath(path);
    if (!filesystem::exists(dirPath))
    {
      if (!filesystem::create_directories(dirPath))
      {
        DEBUG(LOG_KEY, "failed to create dir \"%d\" for analytics pipes", path.c_str());
        return false;
      }
    }
    return true;
  }

  int Analytics::createNamedPipe(const char *name, int *outFd)
  {
    string pipeDir = string(PIPE_PREFIX);
    if (!ensurePipeDir(pipeDir))
      return -1; // just returns -1 if failed to create dir.

    string pipeName = pipeDir + "/" + string(name);
    if (mkfifo(pipeName.c_str(), S_IRWXO) != 0)
    {
      DEBUG(LOG_KEY, "failed to create named pipe: %s, error(%d): %s",
            pipeName.c_str(), errno, strerror(errno));
      return -1;
    }
    else
    {
      DEBUG(LOG_KEY, "created named pipe: %s", pipeName.c_str());
    }

    int fd = open(name, O_WRONLY | O_NONBLOCK);
    if (fd == -1)
    {
      unlink(pipeName.c_str());
      DEBUG(LOG_KEY, "failed to open named pipe: %s", pipeName.c_str());
      return -1;
    }

    *outFd = fd;
    namedPipes.insert(pair<string, int>(name, fd));
    return 0;
  }

  void Analytics::writeToPipe(int fd, std::string text)
  {
    if (fd == -1)
      return;

    // check if fd in fileDescriptors, if not just return
    for (auto it = namedPipes.begin(); it != namedPipes.end(); it++)
    {
      if (it->second == fd)
      {
        write(fd, text.c_str(), text.size());
        return;
      }
    }
  }

  void Analytics::closeNamedPipe(int fd)
  {
    if (fd == -1)
      return;

    for (auto it = namedPipes.begin(); it != namedPipes.end();)
    {
      if (it->second == fd)
      {
        close(fd);
        it = namedPipes.erase(it);
        return;
      }
      else
      {
        it++;
      }
    }
  }

} // namespace analytics
