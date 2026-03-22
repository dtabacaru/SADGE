#include "PulseSweepChannel.h"

PulseSweepChannel::PulseSweepChannel(uint8_t& dataBus,
                                     uint16_t& addrBus) :
  PulseChannel(dataBus, addrBus)
{
}

uint8_t PulseSweepChannel::GetFreqSweepPace()
{
  return (NR10 & GetNRX0BitMask(Nr10BitMask::PACE)) >> 4;
}

uint8_t PulseSweepChannel::GetFreqStep()
{
  return NR10 & GetNRX0BitMask(Nr10BitMask::STEP);
}

void PulseSweepChannel::TickFreqSweep()
{
  mPeriodCounter = GetPeriodCounter();
  bool direction = NR10 & GetNRX0BitMask(Nr10BitMask::DIRECTION);
  uint8_t step = GetFreqStep();

  if (step == 0)
    return;

  mPeriodCounter = direction ? mPeriodCounter - (mPeriodCounter >> step)
                              : mPeriodCounter + (mPeriodCounter >> step);

  if (mPeriodCounter > MAX_PERIOD_COUNT)
    Disable();

  NRX3 = mPeriodCounter & 0xFF;
  NRX4 &= ~GetNRX4BitMask(NRX4BitMask::PERIOD_UPPER);
  NRX4 |= (mPeriodCounter >> 8) & GetNRX4BitMask(NRX4BitMask::PERIOD_UPPER);
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

  if (GetFreqSweepPace() == 0 || GetFreqStep() == 0)
    return;

  mFreqTick += 1;

  if (mFreqTick != MAX_FREQ_COUNT)
    return;
  
  mFreqTick = 0;
  mFreqSweepPaceTick += 1;

  if (mFreqSweepPaceTick != GetFreqSweepPace())
    return;

  mFreqSweepPaceTick = 0;
  TickFreqSweep();
}

void PulseSweepChannel::Reset()
{
  AudioChannel::Reset();
  NR10 = {};
  mFreqTick = {};
  mFreqSweepPaceTick = {};
}