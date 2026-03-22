#pragma once

#include <stdint.h>

class AudioChannel
{
public:
  constexpr static uint16_t MAX_PERIOD_COUNT = 2048;

  enum class NRX1BitMask : uint8_t
  {
    INIT_LEN_TIM = 0b00111111,
  };

  enum class NRX2BitMask : uint8_t
  {
    INIT_VOL = 0b11110000,
    ENV_DIR = 0b00001000,
    SWEEP_PACE = 0b00000111
  };

  enum class NRX4BitMask : uint8_t
  {
    TRIGGER = 0b10000000,
    LENGTH_ENABLE = 0b01000000,
    PERIOD_UPPER = 0b00000111
  };

  virtual constexpr uint8_t GetNRX1BitMask(NRX1BitMask bit_mask) const
  {
    return static_cast<uint8_t>(bit_mask);
  }

  constexpr uint8_t GetNRX2BitMask(NRX2BitMask bit_mask) const
  {
    return static_cast<uint8_t>(bit_mask);
  }

  constexpr uint8_t GetNRX4BitMask(NRX4BitMask bit_mask) const
  {
    return static_cast<uint8_t>(bit_mask);
  }

  AudioChannel(uint8_t& dataBus,
               uint16_t& addrBus);

  virtual int MaxLengthTick() const;
  virtual uint8_t GetInitLengthTimer();
  virtual uint8_t GetVolume();
  bool GetLengthEnable();
  uint16_t GetPeriodCounter();
  virtual bool DacEnabled() const;

  void ZombieMode(uint8_t val);
  void HandleNRX2Write(uint8_t val);
  virtual void HandleNRX4Write(uint8_t val);
  
  double Dac(uint8_t level, double dc_offset);

  void ResetLengthTimer();

  virtual void Trigger();

  virtual void Reset();
  virtual void Disable();

  virtual void TickSoundLength();
  void TickVolSweep();

  virtual void UpdateChOut() = 0;

  virtual void DivTick();
  virtual void ApuTick() = 0;

  uint8_t  mVolume{};
  int      mVolSweepPaceTick{};
  double   mCHOut{};
  int      mLengthTimerTick{};
  int      mSoundLengthTick{};
  int      mEnvelopeTick{};
  bool     mEnvelopeSaturated{};

  bool     mEnabled{};

  uint8_t mNRX1{};
  uint8_t mNRX2{};
  uint8_t mNRX3{};
  uint8_t mNRX4{};

  uint8_t&  mDataBus;
  uint16_t& mAddressBus;
};