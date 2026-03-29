#include "NoiseChannel.h"

#include "Constants.h"

NoiseChannel::NoiseChannel(uint8_t& dataBus,
                           uint16_t& addrBus) :
  AudioChannel(dataBus, addrBus)
{
}

void NoiseChannel::HandleNr43Write()
{
  NRX3 = mDataBus;

  double div = GetClockDivider();
  if (div == 0)
    div = 0.5;

  mPeriodCounter = div * (1 << GetClockShift()) * 4;
}

bool NoiseChannel::GetLfsrWidth()
{
  return NRX3 & GetNr43BitMask(Nr43BitMask::LFSR_WIDTH);
}

uint8_t NoiseChannel::GetClockDivider()
{
  return NRX3 & GetNr43BitMask(Nr43BitMask::CLOCK_DIV);
}

uint8_t NoiseChannel::GetClockShift()
{
  return (NRX3 & GetNr43BitMask(Nr43BitMask::CLOCK_SHIFT)) >> 4;
}

void NoiseChannel::ApuTick()
{
  mLfsrTick += 1;

  if (mLfsrTick < mPeriodCounter)
    return;

  mLfsrTick = 0;

  UpdateChOut();
}

void NoiseChannel::UpdateChOut()
{
  uint16_t result = ((mLfsr-1) >> 1) & 0x1;

  mLfsr &= ~0x8000;
  mLfsr |= result << 15;
  if (GetLfsrWidth())
  {
    mLfsr &= ~0x80;
    mLfsr |= result << 7;
  }
  mLfsr >>= 1;

  uint8_t level = (mLfsr & 0x1) ? mVolume : 0;
  double dcOffset = mVolume / 2;
  mChOut = Dac(level, dcOffset);
}

void NoiseChannel::DivTick()
{
  AudioChannel::DivTick();

  TickVolSweepPace();
}

void NoiseChannel::Trigger()
{
  AudioChannel::Trigger();
  mLfsr = 0;
}

void NoiseChannel::Disable()
{
  AudioChannel::Disable();
  mLfsrTick = 0;
}

void NoiseChannel::Reset()
{
  AudioChannel::Reset();
  mLfsrTick = 0;
  mLfsr = 0;
}