#include "AudioController.h"

#include "Utils.h"

AudioController::AudioController(uint8_t& dataBus,
                                 uint16_t& addrBus) :
  mDataBus(dataBus),
  mAddressBus(addrBus),
  mCh1(dataBus, addrBus),
  mCh2(dataBus, addrBus),
  mCh3(dataBus, addrBus),
  mCh4(dataBus, addrBus)
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
    mDataBus = mCh3.mWaveRam[mAddressBus - GetWaveAddress(WaveAddress::START)];
    return;
  }

  Address addr = static_cast<Address>(mAddressBus);

  switch (addr)
  {
    case AudioController::Address::NR10:
      mDataBus = mCh1.mNR10 | GetUnusedBits(UnusedReadBits::NR10);
      break;
    case AudioController::Address::NR11:
      mDataBus = mCh1.NRX1 | GetUnusedBits(UnusedReadBits::NRX1);
      break;
    case AudioController::Address::NR12:
      mDataBus = mCh1.NRX2;
      break;
      //case AudioController::Address::NR13: // Write only
        //mDataBus = mCH1.NRX3;
        //break;
    case AudioController::Address::NR14:
      mDataBus = mCh1.NRX4 | GetUnusedBits(UnusedReadBits::NRX4);
      break;
    case AudioController::Address::NR21:
      mDataBus = mCh2.NRX1 | GetUnusedBits(UnusedReadBits::NRX1);
      break;
    case AudioController::Address::NR22:
      mDataBus = mCh2.NRX2;
      break;
      //case AudioController::Address::NR23: // Write only
        //mDataBus = mCH2.NRX3;
        //break;
    case AudioController::Address::NR24:
      mDataBus = mCh2.NRX4 | GetUnusedBits(UnusedReadBits::NRX4);
      break;
    case AudioController::Address::NR30:
      mDataBus = mCh3.mNR30 | GetUnusedBits(UnusedReadBits::NR30);
      break;
      //case AudioController::Address::NR31: // Write only
        //mDataBus = mCH3.NRX1;
        //break;
    case AudioController::Address::NR32:
      mDataBus = mCh3.NRX2 | GetUnusedBits(UnusedReadBits::NR32);
      break;
      //case AudioController::Address::NR33: // Write only
        //mDataBus = mCH3.NRX3;
        //break;
    case AudioController::Address::NR34:
      mDataBus = mCh3.NRX4 | GetUnusedBits(UnusedReadBits::NRX4);
      break;
      //case AudioController::Address::NR41: // Write only
        //mDataBus = mCH4.NRX1;
        //break;
    case AudioController::Address::NR42:
      mDataBus = mCh4.NRX2;
      break;
    case AudioController::Address::NR43:
      mDataBus = mCh4.NRX3;
      break;
    case AudioController::Address::NR44:
      mDataBus = mCh4.NRX4 | GetUnusedBits(UnusedReadBits::NRX4);
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
    mCh3.mWaveRam[mAddressBus - GetWaveAddress(WaveAddress::START)] = mDataBus;
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
      mCh1.mNR10 = mDataBus;
      break;
    case AudioController::Address::NR11:
      mCh1.NRX1 = mDataBus;
      break;
    case AudioController::Address::NR12:
      mCh1.HandleNRX2Write();
      break;
    case AudioController::Address::NR13:
      mCh1.NRX3 = mDataBus;
      break;
    case AudioController::Address::NR14:
      mCh1.HandleNRX4Write();
      break;
    case AudioController::Address::NR21:
      mCh2.NRX1 = mDataBus;
      break;
    case AudioController::Address::NR22:
      mCh2.HandleNRX2Write();
      break;
    case AudioController::Address::NR23:
      mCh2.NRX3 = mDataBus;
      break;
    case AudioController::Address::NR24:
      mCh2.HandleNRX4Write();
      break;
    case AudioController::Address::NR30:
      mCh3.HandleNR30Write();
      break;
    case AudioController::Address::NR31:
      mCh3.NRX1 = mDataBus;
      break;
    case AudioController::Address::NR32:
      mCh3.NRX2 = mDataBus;
      break;
    case AudioController::Address::NR33:
      mCh3.NRX3 = mDataBus;
      break;
    case AudioController::Address::NR34:
      mCh3.HandleNRX4Write();
      break;
    case AudioController::Address::NR41:
      mCh4.NRX1 = mDataBus;
      break;
    case AudioController::Address::NR42:
      mCh4.HandleNRX2Write();
      break;
    case AudioController::Address::NR43:
      mCh4.NRX3 = mDataBus;
      break;
    case AudioController::Address::NR44:
      mCh4.HandleNRX4Write();
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
  mCh1.DivTick();
  mCh2.DivTick();
  mCh3.DivTick();
  mCh4.DivTick();
}

void AudioController::ApuTick()
{
  mCh1.ApuTick();
  mCh2.ApuTick();
  mCh3.ApuTick();
  mCh4.ApuTick();
}

Sample AudioController::Mixer()
{
  constexpr static auto L_VOL_START_BIT = 4;
  constexpr static auto R_VOL_START_BIT = 0;

  constexpr static double VOL_DIV = 8.0;

  Sample out;

  if (mNR51 & GetNr51BitMask(Nr51BitMask::CH1_L)) out.left += mCh1.ChOut();
  if (mNR51 & GetNr51BitMask(Nr51BitMask::CH2_L)) out.left += mCh2.ChOut();
  if (mNR51 & GetNr51BitMask(Nr51BitMask::CH3_L)) out.left += mCh3.ChOut();
  if (mNR51 & GetNr51BitMask(Nr51BitMask::CH4_L)) out.left += mCh4.ChOut();

  if (mNR51 & GetNr51BitMask(Nr51BitMask::CH1_R)) out.right += mCh1.ChOut();
  if (mNR51 & GetNr51BitMask(Nr51BitMask::CH2_R)) out.right += mCh2.ChOut();
  if (mNR51 & GetNr51BitMask(Nr51BitMask::CH3_R)) out.right += mCh3.ChOut();
  if (mNR51 & GetNr51BitMask(Nr51BitMask::CH4_R)) out.right += mCh4.ChOut();

  uint8_t leftVol  = ((mNR50 & GetNr50BitMask(Nr50BitMask::L_VOL)) >> L_VOL_START_BIT) & VOL_MASK;
  uint8_t rightVol = ((mNR50 & GetNr50BitMask(Nr50BitMask::R_VOL)) >> R_VOL_START_BIT) & VOL_MASK;
  double leftVolScale  = (leftVol + 1) / VOL_DIV;
  double rightVolScale = (rightVol + 1) / VOL_DIV;

  double chanAvgScale = 1.0 / NUM_CHANNELS;
  double invScale = -1.0;

  double leftScale  = leftVolScale * chanAvgScale * invScale;
  double rightScale = rightVolScale * chanAvgScale * invScale;

  out.left  *= leftScale;
  out.right *= rightScale;

  return out;
}

void AudioController::Reset()
{
  ClearSamples();

  mNR50 = {};
  mNR51 = {};
  mNR52 = {};

  mCh1.Reset();
  mCh2.Reset();
  mCh3.Reset();
  mCh4.Reset();
}

void AudioController::HandleNr52Write()
{
  mNR52 = mDataBus & NR52_MASK;
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
  return (static_cast<int>(mCh4.Enabled()) << 3) |
    (static_cast<int>(mCh3.Enabled()) << 2) |
    (static_cast<int>(mCh2.Enabled()) << 1) |
    (static_cast<int>(mCh1.Enabled()) << 0);
}
