#pragma once

#include <string>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#ifndef TRANSMUTE_STANDALONE
#include <Unity/IUnityLog.h>
static IUnityLog *s_UnityLog;
void SET_UNITY_LOG_HANDLE(IUnityLog *log);
IUnityLog *GET_UNITY_LOG_HANDLE();
#endif

#define LOG_TAG_UNITY "Unity"
#define LOG_TAG_RENDERER "jsar.renderer"
#define LOG_TAG_XR "jsar.xr"
#define LOG_TAG_SCRIPT "jsar.script"

void DEBUG(const char *tag, const char *format, ...);
void SET_THREAD_NAME(const std::string &name);
