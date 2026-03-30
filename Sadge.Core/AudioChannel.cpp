#include "AudioChannel.h"

AudioChannel::AudioChannel(uint8_t& dataBus,
                           uint16_t& addrBus) :
  mDataBus(dataBus),
  mAddressBus(addrBus)
{
}

bool AudioChannel::Enabled() const
{
  return mEnabled;
}

double AudioChannel::ChOut() const
{
  return mChOut;
}

bool AudioChannel::DacEnabled() const
{
  return mDacEnabled;
}

void AudioChannel::HandleNRX1Write()
{
  NRX1 = mDataBus;

  mInitLenTimer = NRX1 & GetNRX1BitMask(NRX1BitMask::INIT_LEN_TIM);
}

void AudioChannel::ZombieMode(uint8_t val)
{
  uint8_t oldDir = NRX2 & GetNRX2BitMask(NRX2BitMask::ENV_DIR);
  uint8_t newDir = val & GetNRX2BitMask(NRX2BitMask::ENV_DIR);

  uint8_t oldPace = NRX2 & GetNRX2BitMask(NRX2BitMask::SWEEP_PACE);

  if (oldPace == 0 && !mEnvelopeSaturated)
    mVolume += 1;
  else if (!oldDir)
    mVolume += 2;

  if (oldDir != newDir)
    mVolume = 16 - mVolume;

  mVolume &= 0xF;
}

void AudioChannel::HandleNRX2Write()
{
  if (mEnabled)
    ZombieMode(mDataBus);

  NRX2 = mDataBus;

  mDacEnabled = NRX2 & (GetNRX2BitMask(NRX2BitMask::INIT_VOL) | GetNRX2BitMask(NRX2BitMask::ENV_DIR));
  mSetVol = (NRX2 & GetNRX2BitMask(NRX2BitMask::INIT_VOL)) >> 4;

  if (!DacEnabled())
    Disable();
}

void AudioChannel::HandleNRX4Write()
{
  NRX4 = mDataBus;

  mPeriodCounter = ((NRX4 & GetNRX4BitMask(NRX4BitMask::PERIOD_UPPER)) << 8) | NRX3;
  mLengthEnable = NRX4 & GetNRX4BitMask(NRX4BitMask::LENGTH_ENABLE);

  if (NRX4 & GetNRX4BitMask(NRX4BitMask::TRIGGER))
    Trigger();
}

double AudioChannel::Dac(uint8_t level, double dcOffset)
{
  return ((level - dcOffset) / 7.5);
}

void AudioChannel::Disable()
{
  mEnabled = false;
}

void AudioChannel::Trigger()
{
  if (!DacEnabled())
    return;

  mEnabled = true;

  mEnvelopeSaturated = false;
  mVolSweepPaceTick = 0;
  mSoundLengthTick = mInitLenTimer;
  mVolume = mSetVol;

  mLevel = 0;
  mDcOffset = mVolume / 2;
  mChOut = Dac(mLevel, mDcOffset);
}

void AudioChannel::TickVolSweep()
{
  bool direction = NRX2 & GetNRX2BitMask(NRX2BitMask::ENV_DIR);

  if (direction && mVolume < 0xF)
    mVolume += 1;
  else if (!direction && mVolume > 0)
    mVolume -= 1;
  else
    mEnvelopeSaturated = true;
}

void AudioChannel::TickVolSweepPace()
{
  mEnvelopeTick += 1;

  if (mEnvelopeTick != MAX_ENV_COUNT)
    return;

  mEnvelopeTick = 0;
  mVolSweepPaceTick += 1;

  if (mVolSweepPaceTick != (NRX2 & GetNRX2BitMask(NRX2BitMask::SWEEP_PACE)))
    return;

  mVolSweepPaceTick = 0;
  TickVolSweep();
}

void AudioChannel::TickSoundLength()
{
  if (!mLengthEnable)
    return;

  mLengthTimerTick += 1;

  if (mLengthTimerTick != MAX_LENGTH_COUNT)
    return;

  mLengthTimerTick = 0;
  mSoundLengthTick += 1;

  if (mSoundLengthTick != MaxLengthTick())
    return;

  mSoundLengthTick = 0;
  Disable();
}

void AudioChannel::DivTick()
{
  if (!mEnabled)
    return;

  TickSoundLength();
}

void AudioChannel::Reset()
{
  NRX1={0};
  NRX2={0};
  NRX3={0};
  NRX4={0};

  mEnabled = {false};

  mEnvelopeTick = {0};
  mEnvelopeSaturated = {false};

  mVolSweepPaceTick = {0};
  mLengthTimerTick = {0};
  mSoundLengthTick = {0};
  mPeriodCounterTick = {0};

  mPeriodCounter = {0};
  mLengthEnable = {false};

  mInitLenTimer = {0};
  mVolume = {0};
  mSetVol = {0};
  mLevel = {0};

  mChOut = {};
  mDcOffset = {};
}

int AudioChannel::MaxLengthTick() const
{
  return 64;
}

