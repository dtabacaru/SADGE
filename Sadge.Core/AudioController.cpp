#include "AudioController.h"

#include "Utils.h"

//#include <fstream>
//std::ofstream pcmOut("apu_out.pcm", std::ios_base::binary);

AudioController::AudioController(uint8_t& dataBus,
                                 uint16_t& addrBus) :
  mDataBus(dataBus),
  mAddressBus(addrBus),
  mCh1(dataBus, addrBus),
  mCh2(dataBus, addrBus),
  mCh3(dataBus, addrBus),
  mCh4(dataBus, addrBus)
{

}

void AudioController::SetAudioCallback(AudioCallback callback)
{
  mCb = callback;
}

bool AudioController::DacsEnabled() const
{
  return mCh1.DacEnabled() ||
    mCh2.DacEnabled() ||
    mCh3.DacEnabled() ||
    mCh4.DacEnabled();
}

uint8_t AudioController::GetChOnBits() const
{
  return (static_cast<int>(mCh4.Enabled()) << 3) |
    (static_cast<int>(mCh3.Enabled()) << 2) |
    (static_cast<int>(mCh2.Enabled()) << 1) |
    (static_cast<int>(mCh1.Enabled()) << 0);
}

void AudioController::ClearSamples()
{
  mWCycleCount = 0;
}

void AudioController::Read() const
{
  if (mAddressBus >= static_cast<uint16_t>(WaveAddress::START) && mAddressBus <= static_cast<uint16_t>(WaveAddress::END))
  {
    mDataBus = mCh3.WaveRam[mAddressBus - GetWaveAddress(WaveAddress::START)];
    return;
  }

  Address addr = static_cast<Address>(mAddressBus);

  switch (addr)
  {
    case AudioController::Address::NR10:
      mDataBus = mCh1.NR10 | GetUnusedBits(UnusedReadBits::NR10);
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
      mDataBus = mCh3.NR30 | GetUnusedBits(UnusedReadBits::NR30);
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
    mCh3.WaveRam[mAddressBus - GetWaveAddress(WaveAddress::START)] = mDataBus;
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
      mCh1.HandleNR10Write();
      break;
    case AudioController::Address::NR11:
      mCh1.HandleNRX1Write();
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
      mCh2.HandleNRX1Write();
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
      mCh3.HandleNRX1Write();
      break;
    case AudioController::Address::NR32:
      mCh3.HandleNRX2Write();
      break;
    case AudioController::Address::NR33:
      mCh3.NRX3 = mDataBus;
      break;
    case AudioController::Address::NR34:
      mCh3.HandleNRX4Write();
      break;
    case AudioController::Address::NR41:
      mCh4.HandleNRX1Write();
      break;
    case AudioController::Address::NR42:
      mCh4.HandleNRX2Write();
      break;
    case AudioController::Address::NR43:
      mCh4.HandleNr43Write();
      break;
    case AudioController::Address::NR44:
      mCh4.HandleNRX4Write();
      break;
    case AudioController::Address::NR50:
      HandleNr50Write();
      break;
    case AudioController::Address::NR51:
      HandleNr51Write();
      break;
  }
}

void AudioController::HandleNr52Write()
{
  mNR52 = mDataBus & NR52_MASK;
  mEnabled = mNR52 & GetNr52BitMask(Nr52BitMask::AUDIO_ON);

  if (!mEnabled)
    Reset();
}

void AudioController::HandleNr51Write()
{
  mNR51 = mDataBus;

  mCh1L = (mNR51 & GetNr51BitMask(Nr51BitMask::CH1_L));
  mCh2L = (mNR51 & GetNr51BitMask(Nr51BitMask::CH2_L));
  mCh3L = (mNR51 & GetNr51BitMask(Nr51BitMask::CH3_L));
  mCh4L = (mNR51 & GetNr51BitMask(Nr51BitMask::CH4_L));
  mCh1R = (mNR51 & GetNr51BitMask(Nr51BitMask::CH1_R));
  mCh2R = (mNR51 & GetNr51BitMask(Nr51BitMask::CH2_R));
  mCh3R = (mNR51 & GetNr51BitMask(Nr51BitMask::CH3_R));
  mCh4R = (mNR51 & GetNr51BitMask(Nr51BitMask::CH4_R));
}

void AudioController::HandleNr50Write()
{
  constexpr auto L_VOL_START_BIT = 4;
  constexpr auto R_VOL_START_BIT = 0;

  constexpr double VOL_DIV = 8.0;

  mNR50 = mDataBus;

  uint8_t leftVol  = ((mNR50 & GetNr50BitMask(Nr50BitMask::L_VOL)) >> L_VOL_START_BIT) & VOL_MASK;
  uint8_t rightVol = ((mNR50 & GetNr50BitMask(Nr50BitMask::R_VOL)) >> R_VOL_START_BIT) & VOL_MASK;

  mLeftVolScale  = (leftVol + 1) / VOL_DIV;
  mRightVolScale = (rightVol + 1) / VOL_DIV;
}

void AudioController::UpdateClk(uint16_t clk)
{
  if (mClk != clk)
  {
    if (Utils::FallingEdgeDetect(mClk, clk, DIV_APU_BIT_MASK))
      DivTick();

    mClk = clk;
  }
}

void AudioController::Tick()
{
  if (!mEnabled)
    return;

  Sample sample = Mixer();
  mSampleBuf[mWCycleCount] = sample;

  mWCycleCount += 1;

  if (mWCycleCount != NUM_SAMPLE)
    return;

  SubSample();
  mWCycleCount = 0;
}

void AudioController::TickCh1()
{
  if (mEnabled)
    mCh1.ApuTick();
}

void AudioController::TickCh2()
{
  if (mEnabled)
    mCh2.ApuTick();
}

void AudioController::TickCh3()
{
  if (mEnabled)
    mCh3.ApuTick();
}

void AudioController::TickCh4()
{
  if (mEnabled)
    mCh4.ApuTick();
}

void AudioController::DivTick()
{
  mCh1.DivTick();
  mCh2.DivTick();
  mCh3.DivTick();
  mCh4.DivTick();
}

void AudioController::BandPass(Sample& in)
{
  if (DacsEnabled())
  {
    mLHPCap = in.left - (in.left - mLHPCap)    * HP_CAP_CHARGE_CONSTANT;
    in.left = mLLPCap + LP_CAP_CHARGE_CONSTANT * ((in.left - mLHPCap) - mLLPCap);
    mLLPCap = in.left;

    mRHPCap = in.right - (in.right - mRHPCap)   * HP_CAP_CHARGE_CONSTANT;
    in.right = mRLPCap + LP_CAP_CHARGE_CONSTANT * ((in.right - mRHPCap) - mRLPCap);
    mRLPCap = in.right;
  }
  else
  {
    mLHPCap = 0;
    mRHPCap = 0;
    mLLPCap = 0;
    mRLPCap = 0;
    in.left = 0;
    in.right = 0;
  }
}

AudioController::Sample AudioController::Mixer()
{
  constexpr double CHAN_AVG_SCALE = 1.0 / NUM_CHANNELS;
  constexpr double INV_SCALE = -1.0;

  Sample out;

  if (mCh1L) out.left  += mCh1.ChOut();
  if (mCh2L) out.left  += mCh2.ChOut();
  if (mCh3L) out.left  += mCh3.ChOut();
  if (mCh4L) out.left  += mCh4.ChOut();
  if (mCh1R) out.right += mCh1.ChOut();
  if (mCh2R) out.right += mCh2.ChOut();
  if (mCh3R) out.right += mCh3.ChOut();
  if (mCh4R) out.right += mCh4.ChOut();

  double leftScale  = mLeftVolScale  * CHAN_AVG_SCALE * INV_SCALE;
  double rightScale = mRightVolScale * CHAN_AVG_SCALE * INV_SCALE;

  out.left  *= leftScale;
  out.right *= rightScale;

  BandPass(out);

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

void AudioController::SubSample()
{
  double sampleIdxStep = W_RATE / A_RATE;

  double sampleIdxF = 0;
  int    sampleIdx = 0;
  for (int subsampleIdx = 0; subsampleIdx < NUM_SUB_SAMPLE; subsampleIdx += 2)
  {
    mSubsampleBuf[subsampleIdx + 0] = static_cast<short>(mSampleBuf[sampleIdx].left  * SHRT_MAX);
    mSubsampleBuf[subsampleIdx + 1] = static_cast<short>(mSampleBuf[sampleIdx].right * SHRT_MAX);

    sampleIdxF += sampleIdxStep;
    sampleIdx = static_cast<int>(round(sampleIdxF));
  }

  //pcmOut.write(reinterpret_cast<char*>(mSubsampleBuf.data()), mSubsampleBuf.size() * sizeof(short));

  if (mCb)
    mCb(mSubsampleBuf);
}
