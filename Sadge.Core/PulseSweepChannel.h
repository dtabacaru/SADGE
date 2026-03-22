#pragma once

#include "PulseChannel.h"

class PulseSweepChannel : public PulseChannel
{
public:

  enum class Nr10BitMask : uint8_t
  {
    PACE = 0b01110000,
    DIRECTION = 0b00001000,
    STEP = 0b00000111
  };

  constexpr uint8_t GetNRX0BitMask(Nr10BitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  PulseSweepChannel(uint8_t& dataBus,
                    uint16_t& addrBus);

  uint8_t GetFreqSweepPace();
  uint8_t GetFreqStep();

  void TickFreqSweep();
  void DivTick();

  void Trigger();
  virtual void Reset();

  uint8_t mNR10{};
  int mFreqTick{};
  int mFreqSweepPaceTick{};
};
