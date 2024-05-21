#pragma once

#include <chrono>

using namespace std;

class CustomEvent
{
public:
  CustomEvent() : type(0) {}
  CustomEvent(int type) : type(type) {}

  int type;
  char *data;
};

class AnimationFrameRequest
{
public:
  AnimationFrameRequest()
  {
    auto now = chrono::system_clock::now();
    time = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
  }

public:
  uint32_t time;
};
