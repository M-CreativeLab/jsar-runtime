#include "logger.hpp"
#include "debug.hpp"

TransmuteLogger *TransmuteLogger::instance_;
TransmuteLogger *TransmuteLogger::GetInstance()
{
    if (instance_ == nullptr)
        instance_ = new TransmuteLogger();
    return instance_;
}

int TransmuteLogger::setDataCallback(LoggerDataCallback callback)
{
    if (isCallbackSet_ == true)
        return 0;

    callback_ = callback;
    isCallbackSet_ = true;
    return 0;
}

void TransmuteLogger::writeStdout(const char* data)
{
    if (isCallbackSet_ == false)
        return;
    callback_(1, data);
}

void TransmuteLogger::writeStderr(const char* data)
{
    if (isCallbackSet_ == false)
        return;
    callback_(2, data);
}

void TransmuteLogger::reset()
{
    isCallbackSet_ = false;
    callback_ = nullptr;
}
