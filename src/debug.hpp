#pragma once

#include <string>

#ifdef __ANDROID__
#include <android/log.h>
#endif

void DEBUG(const char *tag, const char *format, ...);
