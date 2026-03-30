#pragma once

#include <stdint.h>

class AudioChannel
{
public:
  AudioChannel(uint8_t& dataBus,
               uint16_t& addrBus);

  virtual bool DacEnabled() const;
  bool Enabled() const;
  double ChOut() const;

  virtual void HandleNRX1Write();
  virtual void HandleNRX2Write();
  void HandleNRX4Write();

  virtual void DivTick();
  virtual void ApuTick() = 0;

  uint8_t NRX1{0};
  uint8_t NRX2{0};
  uint8_t NRX3{0};
  uint8_t NRX4{0};

protected:

  constexpr static auto MAX_PERIOD_COUNT = 2048;
  constexpr static auto MAX_LENGTH_COUNT = 2;
  constexpr static auto MAX_ENV_COUNT = 8;

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

  virtual constexpr uint8_t GetNRX1BitMask(NRX1BitMask mask) const
  {
    return static_cast<uint8_t>(mask);
  }

  constexpr uint8_t GetNRX2BitMask(NRX2BitMask mask) const
  {
    return static_cast<uint8_t>(mask);
  }

  constexpr uint8_t GetNRX4BitMask(NRX4BitMask mask) const
  {
    return static_cast<uint8_t>(mask);
  }

  virtual int MaxLengthTick() const;

  virtual void Trigger();

  virtual void Reset();
  void Disable();

  void TickVolSweep();
  void TickVolSweepPace();

  double Dac(uint8_t level, double dcOffset = 0);
  virtual void UpdateChOut() = 0;

  bool mEnabled{false};
  bool mDacEnabled{};

  int  mEnvelopeTick{0};
  bool mEnvelopeSaturated{false};
  
  int mVolSweepPaceTick{0};
  int mLengthTimerTick{0};
  int mSoundLengthTick{0};
  int mPeriodCounterTick{0};
  
  uint16_t mPeriodCounter{0};
  bool mLengthEnable{false};

  uint8_t mInitLenTimer{0};
  uint8_t mVolume{0};
  uint8_t mSetVol{0};
  uint8_t mLevel{0};
  
  double  mChOut{};
  double  mDcOffset{};

  uint8_t& mDataBus;
  uint16_t& mAddressBus;

private:

  void ZombieMode(uint8_t val);

  void TickSoundLength();
};