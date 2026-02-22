#pragma once

#include "AudioChannel.h"

#include <array>

class WaveChannel : public AudioChannel
{
public:
  constexpr static uint16_t WAVE_RAM_SIZE = 16; // bytes
  constexpr static std::array<uint8_t, WAVE_RAM_SIZE> WAVE_RAM_INIT = { 0xE2, 0xB7, 0x10, 0x95, 0xC8, 0x6B, 0x0A, 0xF7, 0x02, 0xF6, 0x63, 0xCB, 0x59, 0xE3, 0x90, 0x2F };

  WaveChannel()
  {
    // TODO randomize this, but this is one sample from my hardware
    wave_ram = WAVE_RAM_INIT;
    
    // CGB for testing only - will be set by boot-rom 
    //wave_ram = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
  }

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
    uint8_t shift = GetVolume() == 0 ? 4 : GetVolume() - 1;
    uint8_t level = current_sample >> shift;
    int scale = shift == 4 ? 1 : 1 << shift;
    ch_out = Dac(level, dc_offset/scale);
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
    period_counter += 2;

    if (period_counter >= MAX_PERIOD_COUNT)
    {
      UpdateChOut();
      period_counter = period_counter > MAX_PERIOD_COUNT ? GetPeriodCounter() - 1 : GetPeriodCounter();
    }
  }

  virtual int MaxLengthTick() const { return 256; }

  inline void HandleNr30Write(uint8_t val)
  {
    nr30 = val;

    if (!DacEnabled())
      Disable();
  }

  inline virtual void Trigger()
  {
    if (!DacEnabled())
      return;

    enabled = true;
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

  inline virtual void Reset()
  {
    AudioChannel::Reset();

    nr30 = {};
    current_sample = {};
    period_counter = {};
    wave_form_index = {};
    dc_offset = {};
  }

  uint8_t  nr30{};
  uint8_t  current_sample{};
  int      period_counter{};
  int      wave_form_index{};
  double   dc_offset{};

  std::array<uint8_t, WAVE_RAM_SIZE> wave_ram{};
};