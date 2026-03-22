#pragma once

#include <stdint.h>

enum class InterruptBitMask : uint8_t
{
  VBLANK = 0b00000001, // R/W
  LCD    = 0b00000010, // R/W
  TIMER  = 0b00000100, // R/W
  SERIAL = 0b00001000, // R/W
  JOYPAD = 0b00010000, // R/W
};

constexpr static uint8_t GetInterruptBitMask(InterruptBitMask mask)
{
  return static_cast<uint8_t>(mask);
}

class InterruptReceiver
{
public:
  virtual void ReceiveInterrupt(InterruptBitMask bit_mask) = 0;
  virtual ~InterruptReceiver() = default;
};
