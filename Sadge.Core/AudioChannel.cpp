#include "AudioChannel.h"

AudioChannel::AudioChannel(uint8_t& dataBus,
                           uint16_t& addrBus) :
  mDataBus(dataBus),
  mAddressBus(addrBus)
{
}

  uint8_t AudioChannel::GetInitLengthTimer()
{
  return mNRX1 & GetNRX1BitMask(NRX1BitMask::INIT_LEN_TIM);
}

uint8_t AudioChannel::GetVolume()
{
  return (mNRX2 & GetNRX2BitMask(NRX2BitMask::INIT_VOL)) >> 4;
}

bool AudioChannel::GetLengthEnable()
{
  return mNRX4 & GetNRX4BitMask(NRX4BitMask::LENGTH_ENABLE);
}

uint16_t AudioChannel::GetPeriodCounter()
{
  return ((mNRX4 & GetNRX4BitMask(NRX4BitMask::PERIOD_UPPER)) << 8) | mNRX3;
}

bool AudioChannel::DacEnabled() const
{
  return mNRX2 & (GetNRX2BitMask(NRX2BitMask::INIT_VOL) | GetNRX2BitMask(NRX2BitMask::ENV_DIR));
}

void AudioChannel::ZombieMode(uint8_t val)
{
  uint8_t oldDir = mNRX2 & GetNRX2BitMask(NRX2BitMask::ENV_DIR);
  uint8_t newDir = val & GetNRX2BitMask(NRX2BitMask::ENV_DIR);

  uint8_t oldPace = mNRX2 & GetNRX2BitMask(NRX2BitMask::SWEEP_PACE);

  if (oldPace == 0 && !mEnvelopeSaturated)
    mVolume += 1;
  else if (!oldDir)
    mVolume += 2;

  if (oldDir != newDir)
    mVolume = 16 - mVolume;

  mVolume &= 0xF;
}

void AudioChannel::HandleNRX2Write(uint8_t val)
{
  if (mEnabled)
    ZombieMode(val);

  mNRX2 = val;

  if (!DacEnabled())
    Disable();
}

double AudioChannel::Dac(uint8_t level, double dc_offset)
{
  return ((level - dc_offset) / 7.5);
}

void AudioChannel::HandleNRX4Write(uint8_t val)
{
  bool was_length_enabled = GetLengthEnable();

  mNRX4 = val;

  bool tick_length = !was_length_enabled && GetLengthEnable() && mLengthTimerTick == 1 && mSoundLengthTick != MaxLengthTick();

  if (tick_length)
    TickSoundLength();

  if (mNRX4 & GetNRX4BitMask(NRX4BitMask::TRIGGER))
    Trigger();
}

void AudioChannel::ResetLengthTimer()
{
  mSoundLengthTick = GetInitLengthTimer();
}

void AudioChannel::Disable()
{
  mEnabled = {};
  mVolume = {};
  mCHOut = {};
  mLengthTimerTick = {};
}

void AudioChannel::Trigger()
{
  if (!DacEnabled())
    return;

  mEnvelopeSaturated = false;
  mEnabled = true;
  mVolSweepPaceTick = 0;
  mVolume = GetVolume();
  double dc_offset = mVolume / 2;
  mCHOut = Dac(0, dc_offset);
}

void AudioChannel::TickVolSweep()
{
  bool direction = mNRX2 & GetNRX2BitMask(NRX2BitMask::ENV_DIR);
  if (direction && mVolume < 0xF)
    mVolume += 1;
  else if (!direction && mVolume > 0)
    mVolume -= 1;
  else
    mEnvelopeSaturated = true;
}

void AudioChannel::TickSoundLength()
{
  if (GetLengthEnable())
  {
    mLengthTimerTick += 1;

    if (mLengthTimerTick == 2)
    {
      mSoundLengthTick += 1;

      if (mSoundLengthTick == MaxLengthTick())
      {
        Disable();
        mSoundLengthTick = 0;
      }

      mLengthTimerTick = 0;
    }
  }
}

void AudioChannel::DivTick()
{
  if (!mEnabled)
    return;

  TickSoundLength();
}

void AudioChannel::Reset()
{
  mEnabled={};
  mVolume={};
  mVolSweepPaceTick={};
  mCHOut = {};
  mLengthTimerTick = {};
  mSoundLengthTick = {};

  mNRX1={};
  mNRX2={};
  mNRX3={};
  mNRX4={};
  mEnvelopeTick={};
  mEnvelopeSaturated={};
}

int AudioChannel::MaxLengthTick() const
{
  return 64;
}
