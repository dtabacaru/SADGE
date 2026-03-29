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

  double div = NRX3 & GetNr43BitMask(Nr43BitMask::CLOCK_DIV);
  if (div == 0)
    div = 0.5;

  uint16_t clkShift = 1 << ((NRX3 & GetNr43BitMask(Nr43BitMask::CLOCK_SHIFT)) >> 4);
  mPeriodCounterTick = static_cast<int>(div * clkShift * 4);
  mLfsrWidth = NRX3 & GetNr43BitMask(Nr43BitMask::LFSR_WIDTH);
}

void NoiseChannel::ApuTick()
{
  mLfsrTick += 1;

  if (mLfsrTick < mPeriodCounterTick)
    return;

  mLfsrTick = 0;

  UpdateChOut();
}

void NoiseChannel::UpdateChOut()
{
  uint16_t xnorResult = ((mLfsr - 1) >> 1) & 0x1;

  mLfsr &= ~0x8000;
  mLfsr |= xnorResult << 15;
  if (mLfsrWidth)
  {
    mLfsr &= ~0x80;
    mLfsr |= xnorResult << 7;
  }
  mLfsr >>= 1;

  mLevel = (mLfsr & 0x1) ? mVolume : 0;
  mDcOffset = mVolume / 2;
  mChOut = Dac(mLevel, mDcOffset);
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

void NoiseChannel::Reset()
{
  AudioChannel::Reset();
  mLfsrTick = 0;
  mLfsrWidth = false;
  mLfsr = 0;
}