#include "PulseSweepChannel.h"

PulseSweepChannel::PulseSweepChannel(uint8_t& dataBus,
                                     uint16_t& addrBus) :
  PulseChannel(dataBus, addrBus)
{
}

void PulseSweepChannel::HandleNR10Write()
{
  NR10 = mDataBus;

  mDir = NR10 & GetNRX0BitMask(Nr10BitMask::DIRECTION);
  mFreqPace = (NR10 & GetNRX0BitMask(Nr10BitMask::PACE)) >> 4;
  mFreqStep = NR10 & GetNRX0BitMask(Nr10BitMask::STEP);
}

void PulseSweepChannel::TickFreqSweep()
{
  if (mFreqStep == 0)
    return;

  mPeriodCounter = mDir ? mPeriodCounter - (mPeriodCounter >> mFreqStep)
                        : mPeriodCounter + (mPeriodCounter >> mFreqStep);

  if (mPeriodCounter >= MAX_PERIOD_COUNT)
    Disable();

  NRX3 = mPeriodCounter & 0xFF;
  NRX4 &= ~GetNRX4BitMask(NRX4BitMask::PERIOD_UPPER);
  NRX4 |= (mPeriodCounter >> 8) & GetNRX4BitMask(NRX4BitMask::PERIOD_UPPER);

  mPeriodCounterTick = mPeriodCounter;
}

void PulseSweepChannel::Trigger()
{
  PulseChannel::Trigger();
  mFreqTick = 0;
  mFreqSweepPaceTick = 0;
  TickFreqSweep();
}

void PulseSweepChannel::DivTick()
{
  PulseChannel::DivTick();

  if (mFreqPace == 0 || mFreqStep == 0)
    return;

  mFreqTick += 1;

  if (mFreqTick != MAX_FREQ_COUNT)
    return;
  
  mFreqTick = 0;
  mFreqSweepPaceTick += 1;

  if (mFreqSweepPaceTick != mFreqPace)
    return;

  mFreqSweepPaceTick = 0;
  TickFreqSweep();
}

void PulseSweepChannel::Reset()
{
  PulseChannel::Reset();
  NR10 = {0};
  mFreqTick = {0};
  mFreqSweepPaceTick = {0};
  mDir = {false};
  mFreqPace = {0};
  mFreqStep = {0};
}