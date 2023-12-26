#pragma once

#include <string>

#include "base.hpp"

extern "C"
{
    typedef DLL_PUBLIC void (*LoggerDataCallback)(int fd, const char *data);
}

class TransmuteLogger
{

protected:
    LoggerDataCallback callback_;

public:
    static TransmuteLogger *GetInstance();
    TransmuteLogger()
    {
    }

    int setDataCallback(LoggerDataCallback callback);
    void writeStdout(const char *data);
    void writeStderr(const char *data);
    void reset();

private:
    bool isCallbackSet_ = false;
    static TransmuteLogger *instance_;
};
