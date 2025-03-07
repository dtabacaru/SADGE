#pragma once

#include "InterruptProvider.h"

#include <stdint.h>

class SerialController : public InterruptProvider
{
public:
  constexpr static uint8_t DEFAULT_READ = 0xFF;

  enum class ControlBitMask : uint8_t
  {
    CLOCK_SELECT = 0b00000001, // R/W 0 = External (slave), 1 = Internal (master)
    ENABLE       = 0b10000000  // R/W 1 = Transfer requested/in-progress
  };

  enum class Address : uint16_t
  {
    DATA    = 0xFF01,
    CONTROL = 0xFF02,
    START   = DATA,
    END     = CONTROL
  };
  
  SerialController(InterruptReceiver& interrupt_receiver);
  ~SerialController();

  void HandleWrite(uint16_t address, uint8_t val);
  uint8_t HandleRead(uint16_t address) const;

private:

  uint8_t m_data{};
  uint8_t m_control{};
};
