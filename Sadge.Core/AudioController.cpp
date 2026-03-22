#include "AudioController.h"

#include "Utils.h"

AudioController::AudioController(uint8_t& dataBus,
                                 uint16_t& addrBus) :
  mDataBus(dataBus),
  mAddressBus(addrBus),
  mCH1(dataBus, addrBus),
  mCH2(dataBus, addrBus),
  mCH3(dataBus, addrBus),
  mCH4(dataBus, addrBus)
{
  mSampleBuf.reserve(NUM_CYCLES_TO_BUFFER);
}

void AudioController::SetAudioCallback(AudioCallback callback)
{
  mCallback = callback;
}

void AudioController::ClearSamples()
{
  mSampleBuf.clear();
  mCycleCount = 0;
}

void AudioController::Read() const
{
  if (mAddressBus >= static_cast<uint16_t>(WaveAddress::START) && mAddressBus <= static_cast<uint16_t>(WaveAddress::END))
  {
    mDataBus = mCH3.mWaveRam[mAddressBus - GetWaveAddress(WaveAddress::START)];
    return;
  }

  Address addr = static_cast<Address>(mAddressBus);

  switch (addr)
  {
    case AudioController::Address::NR10:
      mDataBus = mCH1.mNR10 | GetUnusedBits(UnusedReadBits::NR10);
      break;
    case AudioController::Address::NR11:
      mDataBus = mCH1.mNRX1 | GetUnusedBits(UnusedReadBits::NRX1);
      break;
    case AudioController::Address::NR12:
      mDataBus = mCH1.mNRX2;
      break;
      //case AudioController::Address::NR13: // Write only
        //mDataBus = mCH1.NRX3;
        //break;
    case AudioController::Address::NR14:
      mDataBus = mCH1.mNRX4 | GetUnusedBits(UnusedReadBits::NRX4);
      break;
    case AudioController::Address::NR21:
      mDataBus = mCH2.mNRX1 | GetUnusedBits(UnusedReadBits::NRX1);
      break;
    case AudioController::Address::NR22:
      mDataBus = mCH2.mNRX2;
      break;
      //case AudioController::Address::NR23: // Write only
        //mDataBus = mCH2.NRX3;
        //break;
    case AudioController::Address::NR24:
      mDataBus = mCH2.mNRX4 | GetUnusedBits(UnusedReadBits::NRX4);
      break;
    case AudioController::Address::NR30:
      mDataBus = mCH3.mNR30 | GetUnusedBits(UnusedReadBits::NR30);
      break;
      //case AudioController::Address::NR31: // Write only
        //mDataBus = mCH3.NRX1;
        //break;
    case AudioController::Address::NR32:
      mDataBus = mCH3.mNRX2 | GetUnusedBits(UnusedReadBits::NR32);
      break;
      //case AudioController::Address::NR33: // Write only
        //mDataBus = mCH3.NRX3;
        //break;
    case AudioController::Address::NR34:
      mDataBus = mCH3.mNRX4 | GetUnusedBits(UnusedReadBits::NRX4);
      break;
      //case AudioController::Address::NR41: // Write only
        //mDataBus = mCH4.NRX1;
        //break;
    case AudioController::Address::NR42:
      mDataBus = mCH4.mNRX2;
      break;
    case AudioController::Address::NR43:
      mDataBus = mCH4.mNRX3;
      break;
    case AudioController::Address::NR44:
      mDataBus = mCH4.mNRX4 | GetUnusedBits(UnusedReadBits::NRX4);
      break;
    case AudioController::Address::NR50:
      mDataBus = mNR50;
      break;
    case AudioController::Address::NR51:
      mDataBus = mNR51;
      break;
    case AudioController::Address::NR52:
      mDataBus = mNR52 | GetUnusedBits(UnusedReadBits::NR52) | GetChOnBits();
      break;
    default:
      mDataBus = DEFAULT_READ;
      break;
  }
}

void AudioController::Write()
{
  if (mAddressBus >= static_cast<uint16_t>(WaveAddress::START) && mAddressBus <= static_cast<uint16_t>(WaveAddress::END))
  {
    mCH3.mWaveRam[mAddressBus - GetWaveAddress(WaveAddress::START)] = mDataBus;
    return;
  }

  Address addr = static_cast<Address>(mAddressBus);

  if (addr == AudioController::Address::NR52)
  {
    HandleNr52Write();
    return;
  }

  if (!mEnabled)
    return;

  switch (addr)
  {
    case AudioController::Address::NR10:
      mCH1.mNR10 = mDataBus;
      break;
    case AudioController::Address::NR11:
      mCH1.mNRX1 = mDataBus;
      mCH1.ResetLengthTimer();
      break;
    case AudioController::Address::NR12:
      mCH1.HandleNRX2Write(mDataBus);
      break;
    case AudioController::Address::NR13:
      mCH1.mNRX3 = mDataBus;
      break;
    case AudioController::Address::NR14:
      mCH1.HandleNRX4Write(mDataBus);
      break;
    case AudioController::Address::NR21:
      mCH2.mNRX1 = mDataBus;
      mCH2.ResetLengthTimer();
      break;
    case AudioController::Address::NR22:
      mCH2.HandleNRX2Write(mDataBus);
      break;
    case AudioController::Address::NR23:
      mCH2.mNRX3 = mDataBus;
      break;
    case AudioController::Address::NR24:
      mCH2.HandleNRX4Write(mDataBus);
      break;
    case AudioController::Address::NR30:
      mCH3.HandleNR30Write(mDataBus);
      break;
    case AudioController::Address::NR31:
      mCH3.mNRX1 = mDataBus;
      mCH3.ResetLengthTimer();
      break;
    case AudioController::Address::NR32:
      mCH3.mNRX2 = mDataBus;
      break;
    case AudioController::Address::NR33:
      mCH3.mNRX3 = mDataBus;
      break;
    case AudioController::Address::NR34:
      mCH3.HandleNRX4Write(mDataBus);
      break;
    case AudioController::Address::NR41:
      mCH4.mNRX1 = mDataBus;
      mCH4.ResetLengthTimer();
      break;
    case AudioController::Address::NR42:
      mCH4.HandleNRX2Write(mDataBus);
      break;
    case AudioController::Address::NR43:
      mCH4.mNRX3 = mDataBus;
      break;
    case AudioController::Address::NR44:
      mCH4.HandleNRX4Write(mDataBus);
      break;
    case AudioController::Address::NR50:
      mNR50 = mDataBus;
      break;
    case AudioController::Address::NR51:
      mNR51 = mDataBus;
      break;
  }
}

void AudioController::Tick(uint16_t clk)
{
  if (Utils::FallingEdgeDetect(mClk, clk, DIV_APU_BIT_MASK))
    DivTick();

  mClk = clk;

  if (mEnabled)
  {
    ApuTick();

    Sample sample = Mixer();
    mSampleBuf.push_back(sample);

    mCycleCount += 4;

    if (mCycleCount == NUM_CYCLES_TO_BUFFER)
    {
      SubSample();
      mCycleCount = 0;
    }
  }
}

void AudioController::DivTick()
{
  mCH1.DivTick();
  mCH2.DivTick();
  mCH3.DivTick();
  mCH4.DivTick();
}

void AudioController::ApuTick()
{
  mCH1.ApuTick();
  mCH2.ApuTick();
  mCH3.ApuTick();
  mCH4.ApuTick();
}

Sample AudioController::Mixer()
{
  Sample out;

  if (mNR51 & (1 << 4))
    out.left += mCH1.mCHOut;
  if (mNR51 & (1 << 5))
    out.left += mCH2.mCHOut;
  if (mNR51 & (1 << 6))
    out.left += mCH3.mCHOut;
  if (mNR51 & (1 << 7))
    out.left += mCH4.mCHOut;

  if (mNR51 & (1 << 0))
    out.right += mCH1.mCHOut;
  if (mNR51 & (1 << 1))
    out.right += mCH2.mCHOut;
  if (mNR51 & (1 << 2))
    out.right += mCH3.mCHOut;
  if (mNR51 & (1 << 3))
    out.right += mCH4.mCHOut;

  uint8_t left_vol  = (mNR50 >> 4) & 0x7;
  uint8_t right_vol = (mNR50 >> 0) & 0x7;

  double left_scale  = -1.0 * ((left_vol + 1) / 8.0) / NUM_CHANNELS;
  double right_scale = -1.0 * ((right_vol + 1) / 8.0) / NUM_CHANNELS;

  out.left  *= left_scale;
  out.right *= right_scale;

  return out;
}

void AudioController::Reset()
{
  ClearSamples();

  mNR50 = {};
  mNR51 = {};
  mNR52 = {};

  mCH1.Reset();
  mCH2.Reset();
  mCH3.Reset();
  mCH4.Reset();
}

void AudioController::HandleNr52Write()
{
  mNR52 = mDataBus & 0b10000000;
  mEnabled = mNR52 & GetNr52BitMask(Nr52BitMask::AUDIO_ON);

  if (!mEnabled)
    Reset();
}

void AudioController::SubSample()
{
  uint64_t numSubSample  = static_cast<uint64_t>(mSampleBuf.size() * AUDIO_FREQUENCY / M_RATE) * 2; // * 2 for stereo
  double sampleIdxStep = M_RATE / AUDIO_FREQUENCY;
  mSubsampleBuf.resize(numSubSample);

  double sampleIdxF = 0;
  int    sampleIdx = 0;
  for (int subsampleIdx = 0; subsampleIdx < numSubSample; subsampleIdx += 2)
  {
    mSubsampleBuf[subsampleIdx + 0] = static_cast<short>(mSampleBuf[sampleIdx].left * SHRT_MAX);
    mSubsampleBuf[subsampleIdx + 1] = static_cast<short>(mSampleBuf[sampleIdx].right * SHRT_MAX);

    sampleIdxF += sampleIdxStep;
    sampleIdx = static_cast<int>(round(sampleIdxF));
  }

  if (mCallback)
    mCallback(mSubsampleBuf);

  mSampleBuf.clear();
  mSubsampleBuf.clear();
}

uint8_t AudioController::GetChOnBits() const
{
  return (static_cast<int>(mCH4.mEnabled) << 3) | 
         (static_cast<int>(mCH3.mEnabled) << 2) | 
         (static_cast<int>(mCH2.mEnabled) << 1) |
         (static_cast<int>(mCH1.mEnabled) << 0);
}
