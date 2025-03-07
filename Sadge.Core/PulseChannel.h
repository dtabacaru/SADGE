#pragma once

#include "AudioChannel.h"

#include <array>
#include <stdint.h>

class PulseChannel : public AudioChannel
{
public:

  enum class Nrx1BitMask : uint8_t
  {
    WAVE_DUTY = 0b11000000,
    INIT_LEN_TIM = 0b00111111,
  };

  inline constexpr uint8_t GetNrx1BitMask(Nrx1BitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline uint8_t GetWaveDuty()
  {
    return (nrx1 & GetNrx1BitMask(Nrx1BitMask::WAVE_DUTY)) >> 6;
  }

  inline void Update()
  {
    if (enabled)
    {
      period_counter += 1;

      if (period_counter == MAX_PERIOD_COUNT)
      {
        UpdateChOut();
        period_counter = GetPeriodCounter();
      }
    }
  }

  inline void UpdateChOut()
  {
    wave_form_index += 1;
    wave_form_index %= 8;

    uint8_t level = 0;
    switch (GetWaveDuty())
    {
      case 0b00:
        level = DUTY_12_5_WAVE_FORM[wave_form_index] ? volume : 0;
        break;
      case 0b01:
        level = DUTY_25_WAVE_FORM[wave_form_index]   ? volume : 0;
        break;
      case 0b10:
        level = DUTY_50_WAVE_FORM[wave_form_index]   ? volume : 0;
        break;
      case 0b11:
        level = DUTY_75_WAVE_FORM[wave_form_index]   ? volume : 0;
        break;
      default:
        break;
    }

    ch_out = Dac(level, volume);
  }

  inline void TickVolSweep()
  {
    bool direction = nrx2 & GetNrx2BitMask(Nrx2BitMask::ENV_DIR);
    if (direction && volume < 0xF)
      volume += 1;
    else if (!direction && volume > 0) // TODO: >1?
      volume -= 1;
  }

  inline virtual void ApuDivTick()
  {
    AudioChannel::ApuDivTick();

    if (enabled)
    {
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
  }

  inline virtual void Disable()
  {
    AudioChannel::Disable();
    wave_form_index = 0;
  }

  inline virtual void Trigger()
  {
    AudioChannel::Trigger();
    period_counter = GetPeriodCounter();
    envelope_tick = 0;
  }

  int period_counter{};
  int wave_form_index{};
  int envelope_tick{};

  constexpr static std::array<uint8_t, 8> DUTY_12_5_WAVE_FORM{0, 0, 0, 0, 0, 0, 0, 1};
  constexpr static std::array<uint8_t, 8> DUTY_25_WAVE_FORM  {1, 0, 0, 0, 0, 0, 0, 1};
  constexpr static std::array<uint8_t, 8> DUTY_50_WAVE_FORM  {1, 0, 0, 0, 0, 1, 1, 1};
  constexpr static std::array<uint8_t, 8> DUTY_75_WAVE_FORM  {0, 1, 1, 1, 1, 1, 1, 0};
};
