#pragma once

#include <string>
#include <map>
#include <memory>
#include <filesystem>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

namespace analytics
{
  template <typename ValueType>
  class PerformanceValue
  {
  public:
    PerformanceValue() = default;
    PerformanceValue(std::string &filename, ValueType initialValue)
        : filename(filename)
        , initialValue(initialValue)
        , valueSize(64)
    {
      fileDescriptor = open(filename.c_str(), O_CREAT | O_RDWR, 0666);
      if (fileDescriptor != -1)
      {
        ftruncate(fileDescriptor, valueSize);
        void *addr = mmap(nullptr, valueSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0);
        if (addr != MAP_FAILED)
        {
          valueAddr = addr;
          set(initialValue);
        }
      }
    }
    ~PerformanceValue()
    {
      if (fileDescriptor != -1)
      {
        close(fileDescriptor);
        unlink(filename.c_str());
      }
      if (valueAddr != nullptr)
      {
        munmap(valueAddr, valueSize);
        valueAddr = nullptr;
      }
    }

  public:
    void set(ValueType value)
    {
      std::string valueString = std::to_string(value);
      size_t strLen = valueString.size();
      if (valueAddr != nullptr && strLen < valueSize)
      {
        memset(valueAddr, 0, valueSize);
        memcpy(valueAddr, valueString.c_str(), strLen);
        static_cast<char *>(valueAddr)[strLen] = '\n';
      }
    }

  private:
    std::string filename;
    int fileDescriptor;
    ValueType initialValue;
    size_t valueSize;
    void *valueAddr = nullptr;
  };

  class PerformanceFileSystem
  {
  public:
    PerformanceFileSystem(std::string cacheDir, const char *clientPidStr = nullptr)
    {
      std::filesystem::path dirPath = cacheDir + "/perf";
      if (clientPidStr == nullptr) // Clear the perf directory if starting as non-client mode.
        std::filesystem::remove_all(dirPath);
      if (clientPidStr != nullptr)
        dirPath = dirPath / clientPidStr;
      dir = dirPath.string();
      if (!std::filesystem::exists(dir))
        std::filesystem::create_directory(dir);
    }
    ~PerformanceFileSystem()
    {
      for (auto &it : intValues)
        delete it.second;
    }

  public:
    template <typename ValueType>
    inline std::unique_ptr<analytics::PerformanceValue<ValueType>> makeValue(const char *name, ValueType initialValue)
    {
      std::string filename = dir + "/" + name;
      return std::make_unique<PerformanceValue<ValueType>>(filename, initialValue);
    }
    inline PerformanceValue<int> *addIntValue(const char *name, int initialValue)
    {
      std::string filename = dir + "/" + name;
      auto newIntValue = intValues[name] = new PerformanceValue<int>(filename, initialValue);
      return newIntValue;
    }
    inline PerformanceValue<int> *findIntValue(const char *name)
    {
      auto it = intValues.find(name);
      if (it != intValues.end())
        return it->second;
      return nullptr;
    }
    inline bool findAndSetIntValue(const char *name, int value)
    {
      auto it = intValues.find(name);
      if (it != intValues.end())
      {
        auto &intValue = *it->second;
        intValue.set(value);
        return true;
      }
      return false;
    }

  public:
    std::string dir;
    std::map<std::string, PerformanceValue<int> *> intValues;
  };
}
