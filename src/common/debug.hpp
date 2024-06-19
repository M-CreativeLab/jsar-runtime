#pragma once

#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

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
#define LOG_TAG_ERROR "jsar.error"
#define LOG_TAG_RENDERER "renderer"
#define LOG_TAG_XR "jsar.xr"
#define LOG_TAG_FONT "jsar.font"

#ifdef TR_LOGGING_SIMPLE_TAG
// Simple tag
#define LOG_TAG_IPC "jsar"
#define LOG_TAG_CONSTELLATION "jsar"
#define LOG_TAG_CONTENT "jsar"
#define LOG_TAG_SCRIPT "jsar"
#define LOG_TAG_CLIENT_ENTRY "jsar"
#define LOG_TAG_CLIENT_CANVAS "jsar"
#else
// Normal tag
#define LOG_TAG_IPC "jsar.ipc"
#define LOG_TAG_CONSTELLATION "jsar.constellation"
#define LOG_TAG_CONTENT "jsar.content"
#define LOG_TAG_SCRIPT "jsar.script"
#define LOG_TAG_CLIENT_ENTRY "jsar.client_entry"
#define LOG_TAG_CLIENT_CANVAS "jsar.client.canvas"
#endif

/**
 * Print a debug message to the console.
 */
void DEBUG(const char *tag, const char *format, ...);

/**
 * Set the name of the current thread.
 */
void SET_THREAD_NAME(const std::string &name);

/**
 * Enable backtrace logging on current process.
 */
void ENABLE_BACKTRACE();
