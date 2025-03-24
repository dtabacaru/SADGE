#pragma once

#include <stdint.h>

class AudioChannel
{
public:

  constexpr static uint16_t MAX_PERIOD_COUNT = 2048;

  enum class Nrx1BitMask : uint8_t
  {
    INIT_LEN_TIM = 0b00111111,
  };

  enum class Nrx2BitMask : uint8_t
  {
    INIT_VOL = 0b11110000,
    ENV_DIR = 0b00001000,
    SWEEP_PACE = 0b00000111
  };

  enum class Nrx4BitMask : uint8_t
  {
    TRIGGER = 0b10000000,
    LENGTH_ENABLE = 0b01000000,
    PERIOD_UPPER = 0b00000111
  };

  inline virtual constexpr uint8_t GetNrx1BitMask(Nrx1BitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline constexpr uint8_t GetNrx2BitMask(Nrx2BitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline constexpr uint8_t GetNrx4BitMask(Nrx4BitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline virtual uint8_t GetInitLengthTimer()
  {
    return nrx1 & GetNrx1BitMask(Nrx1BitMask::INIT_LEN_TIM);
  }

  inline virtual uint8_t GetVolume()
  {
    return (nrx2 & GetNrx2BitMask(Nrx2BitMask::INIT_VOL)) >> 4;
  }

  inline bool GetLengthEnable()
  {
    return nrx4 & GetNrx4BitMask(Nrx4BitMask::LENGTH_ENABLE);
  }

  inline uint16_t GetPeriodCounter()
  {
    return ((nrx4 & GetNrx4BitMask(Nrx4BitMask::PERIOD_UPPER)) << 8) | nrx3;
  }

  inline double Dac(uint8_t level, uint8_t volume)
  {
    return ((level - (volume / 2.0)) / 12.5);
  }

  inline virtual void HandleNrx4Write(uint8_t val)
  {
    nrx4 = val;

    if (nrx4 & GetNrx4BitMask(Nrx4BitMask::TRIGGER))
      Trigger();
  }

  inline virtual void Disable()
  {
    enabled = false;
    volume = 0;
    ch_out = 0;
  }

  inline virtual void Trigger()
  {
    enabled = true;
    if (length_timer_tick >= 64)
      length_timer_tick = GetInitLengthTimer();
    vol_sweep_pace_tick = 0;
    volume = GetVolume();
    ch_out = Dac(0, volume);
  }
  
  inline virtual void ApuDivTick()
  {
    if (GetLengthEnable() && enabled)
    {
      apu_div_tick += 1;

      if (apu_div_tick == 2)
      {
        length_timer_tick += 1;

        if (length_timer_tick == 64)
        {
          Disable();
        }
        apu_div_tick = 0;
      }

    }
  }

  inline virtual void UpdateChOut() = 0;
  inline virtual void Update() = 0;

  bool     enabled{};
  uint8_t  volume{};
  int      vol_sweep_pace_tick{};
  double   ch_out{};
  int      length_timer_tick{};
  int      apu_div_tick{};

  uint8_t nrx1{};
  uint8_t nrx2{};
  uint8_t nrx3{};
  uint8_t nrx4{};
};