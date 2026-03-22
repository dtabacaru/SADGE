#pragma once

#include "InterruptProvider.h"

class SerialController : public InterruptProvider
{
public:

  enum class Address : uint16_t
  {
    DATA    = 0xFF01,
    CONTROL = 0xFF02,
    START   = DATA,
    END     = CONTROL
  };

  constexpr static uint16_t GetAddress(Address addr)
  {
    return static_cast<uint16_t>(addr);
  }

  SerialController(InterruptReceiver& intRec,
                   uint8_t& dataBus,
                   uint16_t& addrBus);

  void Write();
  void Read() const;

private:

  enum class ControlBitMask : uint8_t
  {
    CLOCK_SELECT = 0b00000001, // R/W 0 = External (slave), 1 = Internal (master)
    ENABLE       = 0b10000000  // R/W 1 = Transfer requested/in-progress
  };

  uint8_t mSB{};
  uint8_t mSC{};

  uint8_t& mDataBus;
  uint16_t& mAddressBus;
};
