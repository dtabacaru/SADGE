#pragma once

#include "PulseSweepChannel.h"
#include "NoiseChannel.h"
#include "WaveChannel.h"

#include "Constants.h"

#include <array>

constexpr static auto W_RATE = T_RATE / 2;

constexpr static uint32_t AUDIO_BITS = 16;
constexpr static uint32_t A_RATE = 48000; // Hz
constexpr static uint32_t NUM_SAMPLE = 98304; // @ W Rate = 196608 T cycles = 2.8 frames
constexpr static uint8_t  AUDIO_CHANNELS = 2;
constexpr static uint64_t NUM_SUB_SAMPLE = static_cast<uint64_t>(NUM_SAMPLE * (A_RATE / W_RATE)) * 2; // * 2 for stereo

class AudioController
{
public:
  struct Sample
  {
    double left{};
    double right{};
  };

  typedef void (*AudioCallback)(std::array<short, NUM_SUB_SAMPLE>& buf);

  constexpr static uint16_t DIV_APU_BIT_MASK = 0b10000000000;
  constexpr static uint8_t  NUM_CHANNELS = 4;

  constexpr static uint8_t NR52_MASK = 0b10000000;
  constexpr static uint8_t VOL_MASK  = 0b00000111;

  enum class Nr50BitMask : uint8_t
  {
    VIN_L  = 0b10000000,
    L_VOL  = 0b01110000,
    VIN_R  = 0b00001000,
    R_VOL  = 0b00000111
  };

  constexpr static uint8_t GetNr50BitMask(Nr50BitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  enum class Nr51BitMask : uint8_t
  {
    CH4_L  = 0b10000000,
    CH3_L  = 0b01000000,
    CH2_L  = 0b00100000,
    CH1_L  = 0b00010000,
    CH4_R  = 0b00001000,
    CH3_R  = 0b00000100,
    CH2_R  = 0b00000010,
    CH1_R  = 0b00000001
  };

  constexpr static uint8_t GetNr51BitMask(Nr51BitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  enum class Nr52BitMask : uint8_t
  {
    AUDIO_ON = 0b10000000,
    CH4_ON   = 0b00001000,
    CH3_ON   = 0b00000100,
    CH2_ON   = 0b00000010,
    CH1_ON   = 0b00000001
  };

  constexpr static uint8_t GetNr52BitMask(Nr52BitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  enum class UnusedReadBits : uint8_t
  {
    NR10 = 0b10000000,
    NRX1 = 0b00111111,
    NRX4 = 0b10111111,
    NR30 = 0b01111111,
    NR32 = 0b10011111,
    NR52 = 0b01110000
  };

  constexpr static uint8_t GetUnusedBits(UnusedReadBits bits)
  {
    return static_cast<uint8_t>(bits);
  }

  enum class WaveAddress : uint16_t
  {
    START = 0xFF30,
    END   = 0xFF3F
  };

  constexpr static uint16_t GetWaveAddress(WaveAddress addr)
  {
    return static_cast<uint16_t>(addr);
  }

  enum class Address : uint16_t
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

  constexpr static uint16_t GetAddress(Address addr)
  {
    return static_cast<uint16_t>(addr);
  }

  AudioController(uint8_t& dataBus,
                  uint16_t& addrBus);

  void SetAudioCallback(AudioCallback callback);

  void ClearSamples();

  void Read() const;
  void Write();

  void UpdateClk(uint16_t clk);

  void Tick();
  void DivTick();

  void TickCh1();
  void TickCh2();
  void TickCh3();
  void TickCh4();

private:
  constexpr static double HP_CAP_CHARGE_CONSTANT = 0.999916001764; // From pan docs, converted for W rate
  constexpr static double LP_CAP_CHARGE_CONSTANT = 0.033;          // Sounds good

  bool DacsEnabled() const;
  uint8_t GetChOnBits() const;

  void HandleNr52Write();
  void HandleNr51Write();
  void HandleNr50Write();

  Sample Mixer();
  void BandPass(Sample& in);
  void SubSample();

  void Reset();

  double mLHPCap{};
  double mRHPCap{};
  double mLLPCap{};
  double mRLPCap{};

  std::array<Sample, NUM_SAMPLE>    mSampleBuf{};
  std::array<short, NUM_SUB_SAMPLE> mSubsampleBuf{};

  uint64_t mWCycleCount{};

  AudioCallback mCb = NULL;

  PulseSweepChannel mCh1;
  PulseChannel      mCh2;
  WaveChannel       mCh3;
  NoiseChannel      mCh4;

  uint8_t mNR50{};
  uint8_t mNR51{};
  uint8_t mNR52{};

  uint16_t mClk{};

  bool mEnabled{};

  bool mCh1L{};
  bool mCh2L{};
  bool mCh3L{};
  bool mCh4L{};
  bool mCh1R{};
  bool mCh2R{};
  bool mCh3R{};
  bool mCh4R{};

  double mLeftVolScale{};
  double mRightVolScale{};

  uint8_t& mDataBus;
  uint16_t& mAddressBus;
};