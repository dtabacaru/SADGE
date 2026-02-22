#pragma once

#include "PulseSweepChannel.h"
#include "NoiseChannel.h"
#include "WaveChannel.h"

#include <vector>

constexpr static uint32_t AUDIO_BITS = 16;
constexpr static uint32_t AUDIO_FREQUENCY = 48000;
constexpr static uint32_t NUM_CYCLES_TO_BUFFER = 65536; // TODO: Non-integer cycles
constexpr static double   T_RATE = (1 << 22);
constexpr static double   M_RATE = (1 << 20);
constexpr static uint32_t AUDIO_STREAM_BUFFER_SIZE = (NUM_CYCLES_TO_BUFFER / 2) * (AUDIO_FREQUENCY / T_RATE);
constexpr static uint8_t  AUDIO_CHANNELS = 1; // TODO: Stereo

class AudioController
{
public:
  constexpr static uint8_t DEFAULT_READ = 0xFF;

  struct AudioSample
  {
    uint64_t cycle{};
    double   level{};
  };

  enum class Nr52BitMask : uint8_t
  {
    AUDIO_ON = 0b10000000, 
    CH4_ON   = 0b00001000,
    CH3_ON   = 0b00000100,
    CH2_ON   = 0b00000010,
    CH1_ON   = 0b00000001
  };

  inline constexpr uint8_t GetNr52BitMask(Nr52BitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

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

  void Init();

  void ClearSamples();

  typedef void (*AudioCallback)(std::vector<short>& audio_buffer);

  AudioCallback m_audio_callback = NULL;

  void SetAudioCallback(AudioCallback audio_callback)
  {
    m_audio_callback = audio_callback;
  }

  AudioController();
  ~AudioController();

  inline void ApuDivTick()
  {
    if(m_ch1.enabled)
      m_ch1.ApuDivTick();
    if (m_ch2.enabled)
      m_ch2.ApuDivTick();
    if (m_ch3.enabled)
      m_ch3.ApuDivTick();
    if (m_ch4.enabled)
      m_ch4.ApuDivTick();
  }

  inline void UpdateApu()
  {
    if (m_ch1.enabled)
      m_ch1.Update();
    if (m_ch2.enabled)
      m_ch2.Update();
    if (m_ch3.enabled)
      m_ch3.Update();
    if (m_ch4.enabled)
      m_ch4.Update();
  }

  inline double Mixer()
  {
    CheckPanning();

    return (m_ch1.ch_out + m_ch2.ch_out + m_ch3.ch_out + m_ch4.ch_out) / 4.0;
  }

  uint8_t HandleRead(uint16_t address) const;
  void HandleWrite(uint16_t address, uint8_t val);
  void Update();

private:
  inline uint8_t GetChOnBits() const
  {
    return (static_cast<int>(m_ch4.enabled) << 3) | (static_cast<int>(m_ch3.enabled) << 2) | (static_cast<int>(m_ch2.enabled) << 1) | static_cast<int>(m_ch1.enabled);
  }

  void HandleNr52Write(uint8_t val);
  void CheckPanning();
  void SubSample();
  void Reset();
  
  std::vector<double> m_samples_buffer;
  std::vector<short>  m_subsample_buffer;
  uint64_t m_cycle_count{};

  PulseSweepChannel m_ch1;
  PulseChannel      m_ch2;
  WaveChannel       m_ch3;
  NoiseChannel      m_ch4;

  uint8_t m_nr50{};
  uint8_t m_nr51{};
  uint8_t m_nr52{};

  bool m_audio_enabled{};
};