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
  virtual void HandleNRX4Write();

  virtual void DivTick();
  virtual void ApuTick() = 0;

  uint8_t NRX1{};
  uint8_t NRX2{};
  uint8_t NRX3{};
  uint8_t NRX4{};

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
  virtual uint8_t GetVolume();

  double Dac(uint8_t level, double dcOffset = 0);

  virtual void Trigger();

  virtual void Reset();
  virtual void Disable();

  void TickVolSweep();
  void TickVolSweepPace();

  virtual void UpdateChOut() = 0;

  bool     mEnabled{};

  int      mEnvelopeTick{};
  bool     mEnvelopeSaturated{};
  
  int      mVolSweepPaceTick{};
  int      mLengthTimerTick{};
  int      mSoundLengthTick{};
  int      mPeriodCounterTick{};
  
  uint16_t mPeriodCounter{};

  uint8_t mInitLenTimer{};
  uint8_t mVolume{};
  uint8_t mSetVol{};
  uint8_t mLevel{};
  bool    mDacEnabled{};
  double  mChOut{};
  double  mDcOffset{};

  uint8_t& mDataBus;
  uint16_t& mAddressBus;

private:

  bool GetLengthEnable();
  void ZombieMode(uint8_t val);

  void TickSoundLength();
};