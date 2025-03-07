#pragma once

#include "MemoryBus.h"

class FlatRamBus : MemoryBus
{
public:
  uint8_t ReadAddress(uint16_t address) const;
  void WriteAddress(uint16_t address, uint8_t val);
};