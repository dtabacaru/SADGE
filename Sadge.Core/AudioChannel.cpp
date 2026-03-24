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

uint8_t AudioChannel::GetInitLengthTimer()
{
  return NRX1 & GetNRX1BitMask(NRX1BitMask::INIT_LEN_TIM);
}

uint8_t AudioChannel::GetVolume()
{
  return (NRX2 & GetNRX2BitMask(NRX2BitMask::INIT_VOL)) >> 4;
}

bool AudioChannel::GetLengthEnable()
{
  return NRX4 & GetNRX4BitMask(NRX4BitMask::LENGTH_ENABLE);
}

uint16_t AudioChannel::GetPeriodCounter()
{
  return ((NRX4 & GetNRX4BitMask(NRX4BitMask::PERIOD_UPPER)) << 8) | NRX3;
}

bool AudioChannel::DacEnabled() const
{
  return NRX2 & (GetNRX2BitMask(NRX2BitMask::INIT_VOL) | GetNRX2BitMask(NRX2BitMask::ENV_DIR));
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

  if (!DacEnabled())
    Disable();
}

double AudioChannel::Dac(uint8_t level, double dcOffset)
{
  return ((level - dcOffset) / 7.5);
}

void AudioChannel::HandleNRX4Write()
{
  NRX4 = mDataBus;

  if (NRX4 & GetNRX4BitMask(NRX4BitMask::TRIGGER))
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
  mChOut = {};
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
  mChOut = Dac(0);
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
  if (!GetLengthEnable())
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
  mEnabled={};
  mVolume={};
  mVolSweepPaceTick={};
  mChOut = {};
  mLengthTimerTick = {};
  mSoundLengthTick = {};

  NRX1={};
  NRX2={};
  NRX3={};
  NRX4={};
  mEnvelopeTick={};
  mEnvelopeSaturated={};
}

int AudioChannel::MaxLengthTick() const
{
  return 64;
}

