#pragma once

#include "AudioChannel.h"

#include <array>

class WaveChannel : public AudioChannel
{
public:

  constexpr static uint16_t WAVE_RAM_SIZE = 16; // bytes

  WaveChannel(uint8_t& dataBus, uint16_t& addressBus);

  void HandleNR30Write();

  void Reset();

  void ApuTick();

  uint8_t  NR30{};
  std::array<uint8_t, WAVE_RAM_SIZE> WaveRam{};

private:

  constexpr static std::array<uint8_t, WAVE_RAM_SIZE> WAVE_RAM_INIT = {
    0xE2, 0xB7,
    0x10, 0x95,
    0xC8, 0x6B,
    0x0A, 0xF7,
    0x02, 0xF6,
    0x63, 0xCB,
    0x59, 0xE3,
    0x90, 0x2F};

  enum class Nr30BitMask : uint8_t
  {
    DAC_ENABLE = 0b10000000,
  };

  enum class Nr32BitMask : uint8_t
  {
    INIT_VOL = 0b01100000,
  };

  int MaxLengthTick() const;
  int MaxPeriodCount() const;
  uint8_t GetInitLengthTimer();
  uint8_t GetVolume();
  bool DacEnabled() const;

  void Trigger();

  void OutputCurrentSample();
  void UpdateChOut();

  uint8_t  mCurrentSample{};
  int      mPeriodCounter{};
  int      mIdx{};
  double   mDcOffset{};
};