#pragma once

#include <stdint.h>

class Utils
{
public:
  static bool FallingEdgeDetect(uint16_t oldCLk, uint16_t newCLk, uint16_t mask)
  {
    return (oldCLk & mask) && !(newCLk & mask);
  }
};