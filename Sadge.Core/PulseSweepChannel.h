#pragma once

#include "PulseChannel.h"

class PulseSweepChannel : public PulseChannel
{
public:

  PulseSweepChannel(uint8_t& dataBus,
                    uint16_t& addrBus);

  void HandleNR10Write();

  virtual void Reset();

  void DivTick();
  
  uint8_t NR10{0};
  
private:
  constexpr static auto MAX_FREQ_COUNT = 4;

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

  void Trigger();

  void TickFreqSweep();

  int     mFreqTick{0};
  int     mFreqSweepPaceTick{0};
  bool    mDir{false};
  uint8_t mFreqPace{0};
  uint8_t mFreqStep{0};
};
