#include "WaveChannel.h"

WaveChannel::WaveChannel(uint8_t& dataBus, uint16_t& addressBus) :
  AudioChannel(dataBus, addressBus),
  WaveRam(WAVE_RAM_INIT) // TODO randomize this, but this is one sample from my hardware
{
}

bool WaveChannel::DacEnabled() const
{
  return NR30 & static_cast<uint8_t>(Nr30BitMask::DAC_ENABLE);
}

uint8_t WaveChannel::GetInitLengthTimer()
{
  return NRX1;
}

uint8_t WaveChannel::GetVolume()
{
  return (NRX2 & static_cast<uint8_t>(Nr32BitMask::INIT_VOL)) >> 5;
}

void WaveChannel::OutputCurrentSample()
{
  uint8_t shift = GetVolume() == 0 ? 4 : GetVolume() - 1;
  uint8_t level = mCurrentSample >> shift;
  int scale = shift == 4 ? 1 : 1 << shift;
  mChOut = Dac(level, mDcOffset / scale);
}

void WaveChannel::UpdateChOut()
{
  mIdx += 1;
  mIdx %= 32;

  int addr = mIdx / 2;

  mCurrentSample = mIdx % 2 == 0 ? WaveRam[addr] >> 4
                                 : WaveRam[addr] & 0xF;

  OutputCurrentSample();
}

void WaveChannel::ApuTick()
{
  mPeriodCounter += 2;

  if (mPeriodCounter < MAX_PERIOD_COUNT)
    return;

  mPeriodCounter = mPeriodCounter > MAX_PERIOD_COUNT ? GetPeriodCounter() - 1 : GetPeriodCounter();
  UpdateChOut();
}

int WaveChannel::MaxLengthTick() const 
{ 
  return 256; 
}

void WaveChannel::HandleNR30Write()
{
  NR30 = mDataBus;

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
  mPeriodCounter = {};
  mIdx = {};
  mDcOffset = {};
}
