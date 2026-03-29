#include "PulseChannel.h"

PulseChannel::PulseChannel(uint8_t& dataBus,
                           uint16_t& addrBus) :
  AudioChannel(dataBus, addrBus)
{
}

void PulseChannel::HandleNRX1Write()
{
  AudioChannel::HandleNRX1Write();
  mWaveDuty = (NRX1 & GetNRX1BitMask(NRX1BitMask::WAVE_DUTY)) >> 6;
}

void PulseChannel::ApuTick()
{
  if (!mEnabled)
    return;

  mPeriodCounterTick += 1;

  if (mPeriodCounterTick != MAX_PERIOD_COUNT)
    return;

  mPeriodCounterTick = mPeriodCounter;
  UpdateChOut();
}

void PulseChannel::UpdateChOut()
{
  mIdx += 1;
  mIdx %= 8;

  mLevel = DUTY[mWaveDuty][mIdx] ? mVolume : 0;
  mDcOffset = mVolume / 2;
  mChOut = Dac(mLevel, mDcOffset);
}

void PulseChannel::DivTick()
{
  AudioChannel::DivTick();

  TickVolSweepPace();
}

void PulseChannel::Trigger()
{
  AudioChannel::Trigger();
  mPeriodCounterTick = mPeriodCounter;
}

void PulseChannel::Reset()
{
  AudioChannel::Reset();
  mPeriodCounterTick = {};
  mIdx = {};
}
