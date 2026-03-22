#include "PulseChannel.h"

PulseChannel::PulseChannel(uint8_t& dataBus,
                           uint16_t& addrBus) :
  AudioChannel(mDataBus, mAddressBus)
{
}

uint8_t PulseChannel::GetWaveDuty()
{
  return (mNRX1 & GetNRX1BitMask(NRX1BitMask::WAVE_DUTY)) >> 6;
}

void PulseChannel::ApuTick()
{
  if (!mEnabled)
    return;

  mPeriodCounter += 1;

  if (mPeriodCounter == MAX_PERIOD_COUNT)
  {
    UpdateChOut();
    mPeriodCounter = GetPeriodCounter();
  }
}

void PulseChannel::UpdateChOut()
{
  mIdx += 1;
  mIdx %= 8;

  uint8_t level = DUTY[GetWaveDuty()][mIdx] ? mVolume : 0;

  double dcOffset = mVolume / 2;
  mCHOut = Dac(level, dcOffset);
}

void PulseChannel::DivTick()
{
  AudioChannel::DivTick();

  mEnvelopeTick += 1;

  if (mEnvelopeTick == 8)
  {
    mVolSweepPaceTick += 1;

    if (mVolSweepPaceTick == (mNRX2 & GetNRX2BitMask(NRX2BitMask::SWEEP_PACE)))
    {
      TickVolSweep();
      mVolSweepPaceTick = 0;
    }

    mEnvelopeTick = 0;
  }
}

void PulseChannel::Trigger()
{
  AudioChannel::Trigger();
  mPeriodCounter = GetPeriodCounter();
}

void PulseChannel::Reset()
{
  AudioChannel::Reset();
  mPeriodCounter = {};
  mIdx = {};
}
