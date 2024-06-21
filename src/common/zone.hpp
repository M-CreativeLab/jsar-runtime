#pragma once

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

enum class TrZoneType
{
  Server,
  Client,
};

class TrZone
{
public:
  TrZone(string filename, TrZoneType type) : type(type), filename(filename)
  {
    handleFd = open(filename.c_str(), O_CREAT | O_RDWR, 0666);
    if (type == TrZoneType::Server)
      ftruncate(handleFd, size);

    int mode = type == TrZoneType::Server ? PROT_READ | PROT_WRITE : PROT_READ;
    void *addr = mmap(NULL, size, mode, MAP_SHARED, handleFd, 0);
    if (addr == MAP_FAILED)
      fprintf(stdout, "failed to create shared map.\n");
    else
      memoryAddr = addr;
  }
  ~TrZone()
  {
    if (handleFd > 0)
    {
      close(handleFd);
      if (type == TrZoneType::Server)
        unlink(filename.c_str());
    }
    if (memoryAddr != nullptr)
    {
      munmap(memoryAddr, size);
    }
  }

public:
  string getFilename()
  {
    return filename;
  }
  void write()
  {
    std::string message = "Hello from parent process!";
    memcpy(memoryAddr, message.c_str(), message.size());
  }
  void read()
  {
    char *message = reinterpret_cast<char *>(memoryAddr);
    fprintf(stdout, "read zone: %s\n", message);
  }

private:
  TrZoneType type;
  string filename;
  int handleFd;
  size_t size = 4096;
  void *memoryAddr = nullptr;
};
