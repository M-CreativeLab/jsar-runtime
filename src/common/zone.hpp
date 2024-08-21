#pragma once

#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#endif

using namespace std;

enum class TrZoneType
{
  Server,
  Client,
};

/**
 * Zone is a mmap-based method to share the C/C++ struct/class from server to client, it means these objects are read-only for the client.
 */
template <typename DataType>
class TrZone
{
public:
  TrZone(string filename, TrZoneType type) : type(type), filename(filename)
  {
    initHandle();
    resize();
    remap();
  }
  TrZone(TrZone &that) : type(that.type), filename(that.filename)
  {
  }
  virtual ~TrZone()
  {
    deinitHandle();
    unmap();
  }

protected:
  virtual size_t getDataSize() { return sizeof(DataType); }

public:
  string getFilename() { return filename; }
  /**
   * Sync the server-side object to the shared memory to protect the data consistency at the client-side.
   */
  void syncData()
  {
    if (memoryAddr != nullptr && data != nullptr)
      memcpy(memoryAddr, reinterpret_cast<void *>(data.get()), memorySize);
  }
  /**
   * Get the data from the shared memory.
   *
   * @return The data pointer.
   */
  DataType *getData() { return reinterpret_cast<DataType *>(memoryAddr); }

private:
  void initHandle()
  {
    handleFd = open(filename.c_str(), O_CREAT | O_RDWR, 0666);
  }
  void deinitHandle()
  {
    if (handleFd > 0)
    {
      close(handleFd);
      if (type == TrZoneType::Server)
        unlink(filename.c_str());
    }
  }
  void resize()
  {
    memorySize = getDataSize();
    if (type == TrZoneType::Server)
      ftruncate(handleFd, memorySize);
  }
  void remap()
  {
    unmap(); // unmap first if needed.

    int mode = type == TrZoneType::Server ? PROT_READ | PROT_WRITE : PROT_READ;
    void *addr = mmap(NULL, memorySize, mode, MAP_SHARED, handleFd, 0);
    if (addr == MAP_FAILED || addr == nullptr)
    {
      DEBUG(LOG_TAG_ERROR, "Failed to create map for %s", filename.c_str());
      return;
    }
    memoryAddr = addr;
  }
  void unmap()
  {
    if (memoryAddr == nullptr)
      return;
    munmap(memoryAddr, memorySize);
    memoryAddr = nullptr;
  }

protected:
  std::unique_ptr<DataType> data = nullptr;

private:
  TrZoneType type;
  string filename;
  int handleFd;
  sem_t *semaphore = nullptr;

private:
  void *memoryAddr = nullptr;
  size_t memorySize = 1024;
};
