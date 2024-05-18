#pragma once

#include <string>
#include "constellation.hpp"

using namespace std;

class TrEmbedder
{
public:
  TrEmbedder();
  ~TrEmbedder();

public:
  bool initialize(string argJson);
  bool start();
  bool frame();
  bool dispatchNativeEvent();

protected:
  TrConstellation* constellation;
};
