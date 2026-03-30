#pragma once

#include "AudioChannel.h"

#include <array>

class PulseChannel : public AudioChannel
{
public:

  PulseChannel(uint8_t& dataBus,
               uint16_t& addrBus);
  
  void HandleNRX1Write();

  void DivTick();
  void ApuTick();

  void Reset();

protected:

  void Trigger();

private:

  constexpr static std::array<std::array<uint8_t, 8>, 4> DUTY
  {{
    {{0, 0, 0, 0, 0, 0, 0, 1}},
    {{1, 0, 0, 0, 0, 0, 0, 1}},
    {{1, 0, 0, 0, 0, 1, 1, 1}},
    {{0, 1, 1, 1, 1, 1, 1, 0}}
  }};

  enum class NRX1BitMask : uint8_t
  {
    WAVE_DUTY = 0b11000000,
    INIT_LEN_TIM = 0b00111111,
  };

  constexpr uint8_t GetNRX1BitMask(NRX1BitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  void UpdateChOut();
  
  uint8_t mWaveDuty{0};
  int     mIdx{0};
};
