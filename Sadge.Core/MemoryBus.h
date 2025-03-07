#pragma once

#include <stdint.h>

class MemoryBus
{
public:
  virtual uint8_t ReadAddress(uint16_t address) const = 0;
  virtual void WriteAddress(uint16_t address, uint8_t val) = 0;
};