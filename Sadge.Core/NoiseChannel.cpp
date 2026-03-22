#include "NoiseChannel.h"

NoiseChannel::NoiseChannel(uint8_t& dataBus,
                           uint16_t& addrBus) :
  AudioChannel(dataBus, addrBus)
{

}

bool NoiseChannel::GetLfsrWidth()
{
  return mNRX3 & GetNr43BitMask(Nr43BitMask::LFSR_WIDTH);
}

uint8_t NoiseChannel::GetClockDivider()
{
  return mNRX3 & GetNr43BitMask(Nr43BitMask::CLOCK_DIV);
}

uint8_t NoiseChannel::GetClockShift()
{
  return (mNRX3 & GetNr43BitMask(Nr43BitMask::CLOCK_SHIFT)) >> 4;
}

void NoiseChannel::ApuTick()
{
  mLfsrTick += 1;

  float div = GetClockDivider();
  if (div == 0)
    div = 0.5;

  uint32_t lfsrPeriodCount = static_cast<uint32_t>((1 << 20) / ((1 << 18) / (div * (1 << GetClockShift()))));

  if (mLfsrTick >= lfsrPeriodCount)
  {
    UpdateChOut();

    mLfsrTick = 0;
  }
}

void NoiseChannel::UpdateChOut()
{
  uint16_t result = ((mLfsr & 0x1) ^ ((mLfsr & 0x2) >> 1)) & 0x1;

  mLfsr &= ~0x8000;
  mLfsr |= result << 15;
  if (GetLfsrWidth())
  {
    mLfsr &= ~0x80;
    mLfsr |= result << 7;
  }
  mLfsr >>= 1;

  uint16_t bit0 = mLfsr & 0x1;
  uint8_t level = bit0 ? mVolume : 0;
  double dcOffset = mVolume / 2;
  mCHOut = Dac(level, dcOffset);
}

void NoiseChannel::DivTick()
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

void NoiseChannel::Trigger()
{
  AudioChannel::Trigger();
  mLfsr = 0xFFFF;
}

void NoiseChannel::Disable()
{
  AudioChannel::Disable();
  mLfsrTick = 0;
}

void NoiseChannel::Reset()
{
  AudioChannel::Reset();
  mLfsrTick = {};
  mLfsr = {0xFFFF};
}