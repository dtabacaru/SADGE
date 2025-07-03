#pragma once

#include "AudioChannel.h"

#include <vector>
#include <iostream>
class WaveChannel : public AudioChannel
{
public:
  constexpr static uint16_t WAVE_RAM_SIZE = 16; // bytes

  enum class Nr30BitMask : uint8_t
  {
    DAC_ENABLE = 0b10000000,
  };

  enum class Nr32BitMask : uint8_t
  {
    INIT_VOL = 0b01100000,
  };

  inline bool DacEnabled() const
  {
    return nr30 & static_cast<uint8_t>(Nr30BitMask::DAC_ENABLE);
  }

  inline uint8_t GetInitLengthTimer()
  {
    return nrx1;
  }
  
  inline uint8_t GetVolume()
  {
    return (nrx2 & static_cast<uint8_t>(Nr32BitMask::INIT_VOL)) >> 5;
  }

  inline void OutputCurrentSample()
  {
    uint8_t level = GetVolume() == 0 ? 0 : current_sample >> (GetVolume() - 1);

    ch_out = Dac(level, dc_offset);
  }

  inline double Dac(uint8_t level, double dc_offset)
  {
    return (level - dc_offset) / 15.0;
  }

  inline void UpdateChOut()
  {
    wave_form_index += 1;
    wave_form_index %= 32;

    int wave_address = wave_form_index / 2;

    current_sample = wave_form_index % 2 == 0 ? wave_ram[wave_address] >> 4
                                              : wave_ram[wave_address] & 0xF;

    OutputCurrentSample();
  } 

  inline void Update()
  {
    if (enabled)
    {
      period_counter += 2;

      if (period_counter >= MAX_PERIOD_COUNT)
      {
        UpdateChOut();
        period_counter = period_counter > MAX_PERIOD_COUNT? GetPeriodCounter() + 1 : GetPeriodCounter();
      }
    }
  }

  inline void ApuDivTick()
  {
    if (GetLengthEnable() && enabled)
    {
      length_timer_tick += 1;

      if (length_timer_tick == 256)
      {
        Disable();
      }
    }
  }

  inline void HandleNr30Write(uint8_t val)
  {
    nr30 = val;

    if (!DacEnabled())
      Disable();
  }

  inline virtual void HandleNrx4Write(uint8_t val)
  {
    nrx4 = val;

    if (nrx4 & GetNrx4BitMask(Nrx4BitMask::TRIGGER))
      Trigger();
  }

  inline virtual void Trigger()
  {
    enabled = true;
    if (length_timer_tick >= 256)
      length_timer_tick = GetInitLengthTimer();
    period_counter = GetPeriodCounter();
    OutputCurrentSample();

    double sum = 0;
    for (int i = 0; i < WAVE_RAM_SIZE; i += 1)
    {
      sum += wave_ram[i] >> 4;
      sum += wave_ram[i] & 0xF;
    }
    dc_offset = sum / (WAVE_RAM_SIZE * 2);

    wave_form_index = 0;
  }

  uint8_t  nr30{};
  uint8_t  current_sample{};
  int      period_counter{};
  int      wave_form_index{};
  double   dc_offset{};

  std::vector<uint8_t> wave_ram = std::vector<uint8_t>(WAVE_RAM_SIZE);
};