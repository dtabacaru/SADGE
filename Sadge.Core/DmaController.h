#pragma once

#include <stdint.h>

class DmaController
{
public:
  DmaController(uint8_t& dataBus, uint16_t& addrBus);

  void Tick();

private:

  uint8_t& mDataBus;
  uint16_t& mAddrBus;
};