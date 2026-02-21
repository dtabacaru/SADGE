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

  inline constexpr uint8_t GetNrx0BitMask(Nr10BitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline uint8_t GetFreqSweepPace()
  {
    return (nr10 & GetNrx0BitMask(Nr10BitMask::PACE)) >> 4;
  }

  inline uint8_t GetFreqStep()
  {
    return nr10 & GetNrx0BitMask(Nr10BitMask::STEP);
  }

  inline void TickFreqSweep()
  {
    period_counter = GetPeriodCounter();
    bool direction = nr10 & GetNrx0BitMask(Nr10BitMask::DIRECTION);
    uint8_t step = GetFreqStep();

    if (step > 0)
    {
      period_counter = direction ? period_counter - (period_counter >> step)
                                 : period_counter + (period_counter >> step);

      if (period_counter > 0x7FF)
        Disable();

      nrx3 = period_counter & 0xFF;
      nrx4 &= ~GetNrx4BitMask(Nrx4BitMask::PERIOD_UPPER);
      nrx4 |= (period_counter >> 8) & GetNrx4BitMask(Nrx4BitMask::PERIOD_UPPER);
    }
  }

  inline void Trigger()
  {
    PulseChannel::Trigger();
    freq_tick = 0;
    freq_sweep_pace_tick = 0;
    TickFreqSweep();
  }

  inline void ApuDivTick()
  {
    PulseChannel::ApuDivTick();

    if (!enabled)
      return;

    if (GetFreqSweepPace() > 0 && GetFreqStep() > 0)
    {
      freq_tick += 1;

      if (freq_tick == 4)
      {
        freq_sweep_pace_tick += 1;

        if (freq_sweep_pace_tick == GetFreqSweepPace())
        {
          TickFreqSweep();

          freq_sweep_pace_tick = 0;
        }

        freq_tick = 0;
      }
    }
  }

  inline virtual void Reset()
  {
    AudioChannel::Reset();
    nr10 = {};
    freq_tick = {};
    freq_sweep_pace_tick = {};
  }

  uint8_t nr10{};
  int freq_tick{};
  int freq_sweep_pace_tick{};
};
