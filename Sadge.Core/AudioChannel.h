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

  inline bool DacEnabled() const
  {
    return nrx2 & (static_cast<uint8_t>(Nrx2BitMask::INIT_VOL) | static_cast<uint8_t>(Nrx2BitMask::ENV_DIR));
  }

  inline void HandleNrx2Write(uint8_t val)
  {
    nrx2 = val;

    if (!DacEnabled())
    {
      Disable();
    }
  }

  inline double Dac(uint8_t level, double dc_offset)
  {
    return ((level - dc_offset) / 7.5);
  }

  inline virtual void HandleNrx4Write(uint8_t val)
  {
    bool was_length_enabled = GetLengthEnable();

    nrx4 = val;

    bool tick_length = !was_length_enabled && GetLengthEnable() && length_timer_tick == 1 && sound_length_tick != MaxLengthTick();

    if (tick_length)
      TickSoundLength();
      
    if (nrx4 & GetNrx4BitMask(Nrx4BitMask::TRIGGER))
      Trigger();
  }

  inline void ResetLengthTimer()
  {
    sound_length_tick = GetInitLengthTimer();
  }

  inline virtual void Disable()
  {
    enabled = {};
    volume = {};
    ch_out = {};
    length_timer_tick = {};
  }

  inline virtual void Trigger()
  {
    if (!DacEnabled())
      return;

    enabled = true;
    vol_sweep_pace_tick = 0;
    volume = GetVolume();
    double dc_offset = volume / 2;
    ch_out = Dac(0, dc_offset);
  }

  inline virtual void TickSoundLength()
  {
    if (GetLengthEnable())
    {
      length_timer_tick += 1;

      if (length_timer_tick == 2)
      {
        sound_length_tick += 1;

        if (sound_length_tick == MaxLengthTick())
        {
          Disable();
          sound_length_tick = 0;
        }

        length_timer_tick = 0;
      }
    }
  }

  inline virtual void ApuDivTick()
  {
    TickSoundLength();
  }

  inline virtual void Reset()
  {
    enabled={};
    volume={};
    vol_sweep_pace_tick={};
    ch_out = {};
    length_timer_tick = {};
    sound_length_tick = {};

    nrx1={};
    nrx2={};
    nrx3={};
    nrx4={};
  }

  virtual int MaxLengthTick() const { return 64; }

  inline virtual void UpdateChOut() = 0;
  inline virtual void Update() = 0;

  bool     enabled{};
  uint8_t  volume{};
  int      vol_sweep_pace_tick{};
  double   ch_out{};
  int      length_timer_tick{};
  int      sound_length_tick{};

  uint8_t nrx1{};
  uint8_t nrx2{};
  uint8_t nrx3{};
  uint8_t nrx4{};
};