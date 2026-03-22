#pragma once

#include "AudioChannel.h"

#include <array>

class WaveChannel : public AudioChannel
{
public:
  constexpr static uint16_t WAVE_RAM_SIZE = 16; // bytes
  constexpr static std::array<uint8_t, WAVE_RAM_SIZE> WAVE_RAM_INIT = { 0xE2, 0xB7, 0x10, 0x95, 0xC8, 0x6B, 0x0A, 0xF7, 0x02, 0xF6, 0x63, 0xCB, 0x59, 0xE3, 0x90, 0x2F };

  enum class Nr30BitMask : uint8_t
  {
    DAC_ENABLE = 0b10000000,
  };

  enum class Nr32BitMask : uint8_t
  {
    INIT_VOL = 0b01100000,
  };

  WaveChannel(uint8_t& dataBus, uint16_t& addressBus);

  int MaxLengthTick() const;
  uint8_t GetInitLengthTimer();
  uint8_t GetVolume();
  bool DacEnabled() const;

  void HandleNR30Write(uint8_t val);

  void OutputCurrentSample();

  void Trigger();

  void UpdateChOut();

  void Reset();

  void ApuTick();

  uint8_t  mNR30{};
  uint8_t  mCurrentSample{};
  int      mPeriodCounter{};
  int      mIdx{};
  double   mDcOffset{};

  std::array<uint8_t, WAVE_RAM_SIZE> mWaveRam{};
};