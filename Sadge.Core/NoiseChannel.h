#pragma once

#include "AudioChannel.h"

class NoiseChannel : public AudioChannel
{
public:

  NoiseChannel(uint8_t& dataBus,
               uint16_t& addrBus);

  void HandleNr43Write();

  void Trigger();

  void Reset();

  void DivTick();
  void ApuTick();

private:

  enum class Nr43BitMask : uint8_t
  {
    CLOCK_SHIFT = 0b11110000,
    LFSR_WIDTH  = 0b00001000,
    CLOCK_DIV   = 0b00000111
  };

  constexpr uint8_t GetNr43BitMask(Nr43BitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  void UpdateChOut();

  int mLfsrTick{};
  bool mLfsrWidth{};
  uint16_t mLfsr{};
};