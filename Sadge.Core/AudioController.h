#pragma once

#include "PulseSweepChannel.h"
#include "NoiseChannel.h"
#include "WaveChannel.h"

#include "Constants.h"

#include <vector>

constexpr static uint32_t AUDIO_BITS = 16;
constexpr static uint32_t AUDIO_FREQUENCY = 48000;
constexpr static uint32_t NUM_CYCLES_TO_BUFFER = 65536 * 3; // TODO: Non-integer cycles
constexpr static uint8_t  AUDIO_CHANNELS = 2;
constexpr static uint32_t AUDIO_STREAM_BUFFER_SIZE = static_cast<uint32_t>((NUM_CYCLES_TO_BUFFER / 4) * (AUDIO_FREQUENCY / T_RATE)) * AUDIO_CHANNELS;

class AudioController
{
public:
  constexpr static uint16_t DIV_APU_BIT_MASK = 0b10000000000;
  constexpr static uint8_t  NUM_CHANNELS = 4;
  constexpr static uint8_t  DEFAULT_READ = 0xFF;

  enum class Nr52BitMask : uint8_t
  {
    AUDIO_ON = 0b10000000, 
    CH4_ON   = 0b00001000,
    CH3_ON   = 0b00000100,
    CH2_ON   = 0b00000010,
    CH1_ON   = 0b00000001
  };

  constexpr uint8_t GetNr52BitMask(Nr52BitMask bit_mask);

  enum class AudioWaveAddress : uint16_t
  {
    START = 0xFF30,
    END   = 0xFF3F
  };

  enum class AudioAddress : uint16_t
  {
    NR10  = 0xFF10,
    NR11  = 0xFF11,
    NR12  = 0xFF12,
    NR13  = 0xFF13,
    NR14  = 0xFF14,
    NR21  = 0xFF16,
    NR22  = 0xFF17,
    NR23  = 0xFF18,
    NR24  = 0xFF19,
    NR30  = 0xFF1A,
    NR31  = 0xFF1B,
    NR32  = 0xFF1C,
    NR33  = 0xFF1D,
    NR34  = 0xFF1E,
    NR41  = 0xFF20,
    NR42  = 0xFF21,
    NR43  = 0xFF22,
    NR44  = 0xFF23,
    NR50  = 0xFF24,
    NR51  = 0xFF25,
    NR52  = 0xFF26,
    START = NR10,
    END   = NR52
  };

  void ClearSamples();

  typedef void (*AudioCallback)(std::vector<short>& audio_buffer);

  AudioCallback m_audio_callback = NULL;

  void SetAudioCallback(AudioCallback audio_callback);

  AudioController();

  void ApuDivTick();

  void UpdateApu();

  inline void Mixer(double& left, double& right);

  uint8_t HandleRead(uint16_t address) const;
  void HandleWrite(uint16_t address, uint8_t val);
  void Update(uint16_t clk);

private:
  uint8_t GetChOnBits() const;

  void HandleNr52Write(uint8_t val);
  void SubSample();
  void Reset();
  
  std::vector<double> mLBuf;
  std::vector<double> mRBuf;

  std::vector<short>  mSubsampleBuf;
  uint64_t mCycleCount{};

  PulseSweepChannel mCH1;
  PulseChannel      mCH2;
  WaveChannel       mCH3;
  NoiseChannel      mCH4;

  uint8_t mNR50{};
  uint8_t mNR51{};
  uint8_t mNR52{};

  uint16_t mClk{};

  bool mEnabled{};
};