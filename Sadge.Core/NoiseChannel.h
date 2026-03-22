#pragma once

#include "AudioChannel.h"

class NoiseChannel : public AudioChannel
{
public:

  enum class Nr43BitMask : uint8_t
  {
    CLOCK_SHIFT = 0b11110000,
    LFSR_WIDTH  = 0b00001000,
    CLOCK_DIV   = 0b00000111
  };

  constexpr uint8_t GetNr43BitMask(Nr43BitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  NoiseChannel(uint8_t& dataBus,
               uint16_t& addrBus);

  bool GetLfsrWidth();
  uint8_t GetClockDivider();
  uint8_t GetClockShift();

  void Trigger();

  void Reset();
  void Disable();
  
  void UpdateChOut();

  void DivTick();
  void ApuTick();

  uint32_t mLfsrTick{};
  uint16_t mLfsr{0xFFFF};
};