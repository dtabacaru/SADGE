#pragma once

#include <stdint.h>

class Cartridge
{
public:
  virtual ~Cartridge() {}

  virtual void MBC7FFF(uint8_t val)  = 0;

  virtual void MMBC5FFF(uint8_t val) = 0;

  virtual void MBC3FFF(uint8_t val)  = 0;

  virtual void MBC1FFF(uint8_t val)  = 0;

};