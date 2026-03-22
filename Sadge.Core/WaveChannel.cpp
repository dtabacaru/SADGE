#include "WaveChannel.h"

WaveChannel::WaveChannel(uint8_t& dataBus, uint16_t& addressBus) :
  AudioChannel(dataBus, addressBus),
  mWaveRam(WAVE_RAM_INIT) // TODO randomize this, but this is one sample from my hardware
{

}

bool WaveChannel::DacEnabled() const
{
  return mNR30 & static_cast<uint8_t>(Nr30BitMask::DAC_ENABLE);
}

uint8_t WaveChannel::GetInitLengthTimer()
{
  return mNRX1;
}

uint8_t WaveChannel::GetVolume()
{
  return (mNRX2 & static_cast<uint8_t>(Nr32BitMask::INIT_VOL)) >> 5;
}

void WaveChannel::OutputCurrentSample()
{
  uint8_t shift = GetVolume() == 0 ? 4 : GetVolume() - 1;
  uint8_t level = mCurrentSample >> shift;
  int scale = shift == 4 ? 1 : 1 << shift;
  mCHOut = Dac(level, mDcOffset / scale);
}

void WaveChannel::UpdateChOut()
{
  mIdx += 1;
  mIdx %= 32;

  int addr = mIdx / 2;

  mCurrentSample = mIdx % 2 == 0 ? mWaveRam[addr] >> 4
                                 : mWaveRam[addr] & 0xF;

  OutputCurrentSample();
}

void WaveChannel::ApuTick()
{
  mPeriodCounter += 2;

  if (mPeriodCounter >= MAX_PERIOD_COUNT)
  {
    UpdateChOut();
    mPeriodCounter = mPeriodCounter > MAX_PERIOD_COUNT ? GetPeriodCounter() - 1 : GetPeriodCounter();
  }
}

int WaveChannel::MaxLengthTick() const { return 256; }

void WaveChannel::HandleNR30Write(uint8_t val)
{
  mNR30 = val;

  if (!DacEnabled())
    Disable();
}

void WaveChannel::Trigger()
{
  if (!DacEnabled())
    return;

  mEnabled = true;
  mPeriodCounter = GetPeriodCounter();
  OutputCurrentSample();

  double sum = 0;
  for (int i = 0; i < WAVE_RAM_SIZE; i += 1)
  {
    sum += mWaveRam[i] >> 4;
    sum += mWaveRam[i] & 0xF;
  }
  mDcOffset = sum / (WAVE_RAM_SIZE * 2);

  mIdx = 0;
}

void WaveChannel::Reset()
{
  AudioChannel::Reset();

  mNR30 = {};
  mCurrentSample = {};
  mPeriodCounter = {};
  mIdx = {};
  mDcOffset = {};
}
