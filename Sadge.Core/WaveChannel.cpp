#include "WaveChannel.h"

WaveChannel::WaveChannel(uint8_t& dataBus, uint16_t& addressBus) :
  AudioChannel(dataBus, addressBus),
  WaveRam(WAVE_RAM_INIT) // TODO randomize this, but this is one sample from my hardware
{
}

bool WaveChannel::DacEnabled() const
{
  return mDacEnabled;
}

void WaveChannel::HandleNR30Write()
{
  NR30 = mDataBus;

  mDacEnabled = NR30 & static_cast<uint8_t>(Nr30BitMask::DAC_ENABLE);

  if (!DacEnabled())
    Disable();
}

void WaveChannel::HandleNRX1Write()
{
  NRX1 = mDataBus;

  mInitLenTimer = NRX1;
}

void WaveChannel::HandleNRX2Write()
{
  NRX2 = mDataBus;
  mSetVol = (NRX2 & static_cast<uint8_t>(Nr32BitMask::INIT_VOL)) >> 5;
  mShift = mSetVol == 0 ? 4 : mSetVol - 1;
  mDcScale = mShift == 4 ? 1 : 1 << mShift;
}

void WaveChannel::UpdateChOut()
{
  mIdx += 1;
  mIdx %= 32;

  int addr = mIdx / 2;

  mCurrentSample = mIdx % 2 == 0 ? WaveRam[addr] >> 4
                                 : WaveRam[addr] & 0xF;

  mLevel = mCurrentSample >> mShift;
  mChOut = Dac(mLevel, mDcOffset / mDcScale);
}

void WaveChannel::ApuTick()
{
  mPeriodCounterTick += 1;

  if (mPeriodCounterTick != MAX_PERIOD_COUNT)
    return;

  mPeriodCounterTick = mPeriodCounter;
  UpdateChOut();
}

int WaveChannel::MaxLengthTick() const 
{ 
  return 256; 
}

void WaveChannel::Trigger()
{
  if (!DacEnabled())
    return;

  mEnabled = true;
  mPeriodCounterTick = mPeriodCounter;

  double sum = 0;
  for (int i = 0; i < WAVE_RAM_SIZE; i += 1)
  {
    sum += WaveRam[i] >> 4;
    sum += WaveRam[i] & 0xF;
  }
  mDcOffset = sum / (WAVE_RAM_SIZE * 2);

  mIdx = 0;
}

void WaveChannel::Reset()
{
  AudioChannel::Reset();

  NR30 = {};
  mCurrentSample = {};
  mPeriodCounterTick = {};
  mIdx = {};
  mDcOffset = {};
}
