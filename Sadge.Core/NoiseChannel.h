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

  inline constexpr uint8_t GetNr43BitMask(Nr43BitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline bool GetLfsrWidth()
  {
    return nrx3 & GetNr43BitMask(Nr43BitMask::LFSR_WIDTH);
  }

  inline uint8_t GetClockDivider()
  {
    return nrx3 & GetNr43BitMask(Nr43BitMask::CLOCK_DIV);
  }

  inline uint8_t GetClockShift()
  {
    return (nrx3 & GetNr43BitMask(Nr43BitMask::CLOCK_SHIFT)) >> 4;
  }

  inline void Update()
  {
    lfsr_tick += 1;

    float divider = GetClockDivider();
    if (divider == 0)
      divider = 0.5;

    uint32_t lfsr_period_count = static_cast<uint32_t>((1 << 20) / ((1<<18) / (divider * (1 << GetClockShift()))));

    if (lfsr_tick >= lfsr_period_count)
    {
      UpdateChOut();

      lfsr_tick = 0;
    }
  }

  inline void UpdateChOut()
  {
    uint16_t xor_result = ((lfsr & 0x1) ^ ((lfsr & 0x2) >> 1)) & 0x1;
    
    lfsr &= ~0x8000;
    lfsr |= xor_result << 15;
    if (GetLfsrWidth())
    {
      lfsr &= ~0x80;
      lfsr |= xor_result << 7;
    }
    lfsr >>= 1;

    uint16_t bit_0 = lfsr & 0x1;
    uint8_t level = bit_0 ? volume : 0;
    double dc_offset = volume / 2;
    ch_out = Dac(level, dc_offset);
  }

  inline virtual void ApuDivTick()
  {
    AudioChannel::ApuDivTick();

    envelope_tick += 1;

    if (envelope_tick == 8)
    {
      vol_sweep_pace_tick += 1;

      if (vol_sweep_pace_tick == (nrx2 & GetNrx2BitMask(Nrx2BitMask::SWEEP_PACE)))
      {
        TickVolSweep();
        vol_sweep_pace_tick = 0;
      }

      envelope_tick = 0;
    }
  }

  inline virtual void Trigger()
  {
    AudioChannel::Trigger();
    lfsr = 0xFFFF;
  }

  inline virtual void Disable()
  {
    AudioChannel::Disable();
    lfsr_tick = 0;
  }

  inline virtual void Reset()
  {
    AudioChannel::Reset();
    lfsr_tick = {};
    lfsr = {0xFFFF};
  }

  uint32_t lfsr_tick{};
  uint16_t lfsr{0xFFFF};
};