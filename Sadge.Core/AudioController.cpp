#include "AudioController.h"

#include "Utils.h"

//#include <fstream>
//std::ofstream pcm_out("apu_out.pcm", std::ios_base::binary);

AudioController::AudioController()
{
  mLBuf.reserve(NUM_CYCLES_TO_BUFFER);
  mRBuf.reserve(NUM_CYCLES_TO_BUFFER);
}

constexpr uint8_t AudioController::GetNr52BitMask(Nr52BitMask bit_mask)
{
  return static_cast<uint8_t>(bit_mask);
}

void AudioController::SetAudioCallback(AudioCallback audio_callback)
{
  m_audio_callback = audio_callback;
}

void AudioController::ApuDivTick()
{
  if (mCH1.enabled)
    mCH1.ApuDivTick();
  if (mCH2.enabled)
    mCH2.ApuDivTick();
  if (mCH3.enabled)
    mCH3.ApuDivTick();
  if (mCH4.enabled)
    mCH4.ApuDivTick();
}

void AudioController::UpdateApu()
{
  if (mCH1.enabled)
    mCH1.Update();
  if (mCH2.enabled)
    mCH2.Update();
  if (mCH3.enabled)
    mCH3.Update();
  if (mCH4.enabled)
    mCH4.Update();
}

void AudioController::Mixer(double& left, double& right)
{
  if (mNR51 & (1 << 4))
    left += mCH1.ch_out;
  if (mNR51 & (1 << 5))
    left += mCH2.ch_out;
  if (mNR51 & (1 << 6))
    left += mCH3.ch_out;
  if (mNR51 & (1 << 7))
    left += mCH4.ch_out;

  if (mNR51 & (1 << 0))
    right += mCH1.ch_out;
  if (mNR51 & (1 << 1))
    right += mCH2.ch_out;
  if (mNR51 & (1 << 2))
    right += mCH3.ch_out;
  if (mNR51 & (1 << 3))
    right += mCH4.ch_out;

  uint8_t left_vol = (mNR50 >> 4) & 0x7;
  uint8_t right_vol = (mNR50 >> 0) & 0x7;

  double left_scale = -1.0 * ((left_vol + 1) / 8.0) / NUM_CHANNELS;
  double right_scale = -1.0 * ((right_vol + 1) / 8.0) / NUM_CHANNELS;

  left *= left_scale;
  right *= right_scale;
}

void AudioController::ClearSamples()
{
  mLBuf.clear();
  mRBuf.clear();
  mCycleCount = 0;
}

enum class UnusedReadBits : uint8_t
{
  NR10 = 0b10000000,
  NRX1 = 0b00111111,
  NRX4 = 0b10111111,
  NR30 = 0b01111111,
  NR32 = 0b10011111,
  NR52 = 0b01110000
};

static constexpr inline uint8_t GetUnusedBits(UnusedReadBits bits)
{
  return static_cast<uint8_t>(bits);
}

uint8_t AudioController::HandleRead(uint16_t address) const
{
  AudioAddress audio_address = static_cast<AudioAddress>(address);

  if (address >= static_cast<uint16_t>(AudioWaveAddress::START) && address <= static_cast<uint16_t>(AudioWaveAddress::END))
  {
    address -= static_cast<uint16_t>(AudioWaveAddress::START);
    return mCH3.wave_ram[address];
  }

  switch (audio_address)
  {
    case AudioController::AudioAddress::NR10:
      return mCH1.nr10 | GetUnusedBits(UnusedReadBits::NR10);
    case AudioController::AudioAddress::NR11:
      return mCH1.nrx1 | GetUnusedBits(UnusedReadBits::NRX1);
    case AudioController::AudioAddress::NR12:
      return mCH1.nrx2;
    //case AudioController::AudioAddress::NR13: // Write only
    //  return m_ch1.nrx3;
    case AudioController::AudioAddress::NR14:
      return mCH1.nrx4 | GetUnusedBits(UnusedReadBits::NRX4);
    case AudioController::AudioAddress::NR21:
      return mCH2.nrx1 | GetUnusedBits(UnusedReadBits::NRX1);
    case AudioController::AudioAddress::NR22:
      return mCH2.nrx2;
    //case AudioController::AudioAddress::NR23: // Write only
    //  return m_ch2.nrx3;
    case AudioController::AudioAddress::NR24:
      return mCH2.nrx4 | GetUnusedBits(UnusedReadBits::NRX4);
    case AudioController::AudioAddress::NR30:
      return mCH3.nr30 | GetUnusedBits(UnusedReadBits::NR30);
    //case AudioController::AudioAddress::NR31: // Write only
    //  return m_ch3.nrx1;
    case AudioController::AudioAddress::NR32:
      return mCH3.nrx2 | GetUnusedBits(UnusedReadBits::NR32);
    //case AudioController::AudioAddress::NR33: // Write only
    //  return m_ch3.nrx3;
    case AudioController::AudioAddress::NR34:
      return mCH3.nrx4 | GetUnusedBits(UnusedReadBits::NRX4);
    //case AudioController::AudioAddress::NR41: // Write only
    //  return m_ch4.nrx1;
    case AudioController::AudioAddress::NR42:
      return mCH4.nrx2;
    case AudioController::AudioAddress::NR43:
      return mCH4.nrx3;
    case AudioController::AudioAddress::NR44:
      return mCH4.nrx4 | GetUnusedBits(UnusedReadBits::NRX4);
    case AudioController::AudioAddress::NR50:
      return mNR50;
    case AudioController::AudioAddress::NR51:
      return mNR51;
    case AudioController::AudioAddress::NR52:
      return mNR52 | GetUnusedBits(UnusedReadBits::NR52) | GetChOnBits();
    default:
      return DEFAULT_READ;
  }
}

void AudioController::HandleWrite(uint16_t address, uint8_t val)
{
  AudioAddress audio_address = static_cast<AudioAddress>(address);

  if (audio_address == AudioController::AudioAddress::NR52)
  {
    HandleNr52Write(val);
    return;
  }

  if (address >= static_cast<uint16_t>(AudioWaveAddress::START) && address <= static_cast<uint16_t>(AudioWaveAddress::END))
  {
    address -= static_cast<uint16_t>(AudioWaveAddress::START);
    mCH3.wave_ram[address] = val;
    return;
  }

  if (!mEnabled)
    return;

  switch (audio_address)
  {
    case AudioController::AudioAddress::NR10:
      mCH1.nr10 = val;
      break;
    case AudioController::AudioAddress::NR11:
      mCH1.nrx1 = val;
      mCH1.ResetLengthTimer();
      break;
    case AudioController::AudioAddress::NR12:
      mCH1.HandleNrx2Write(val);
      break;
    case AudioController::AudioAddress::NR13:
      mCH1.nrx3 = val;
      break;
    case AudioController::AudioAddress::NR14:
      mCH1.HandleNrx4Write(val);
      break;
    case AudioController::AudioAddress::NR21:
      mCH2.nrx1 = val;
      mCH2.ResetLengthTimer();
      break;
    case AudioController::AudioAddress::NR22:
      mCH2.HandleNrx2Write(val);
      break;
    case AudioController::AudioAddress::NR23:
      mCH2.nrx3 = val;
      break;
    case AudioController::AudioAddress::NR24:
      mCH2.HandleNrx4Write(val);
      break;
    case AudioController::AudioAddress::NR30:
      mCH3.HandleNr30Write(val);
      break;
    case AudioController::AudioAddress::NR31:
      mCH3.nrx1 = val;
      mCH3.ResetLengthTimer();
      break;
    case AudioController::AudioAddress::NR32:
      mCH3.nrx2 = val;
      break;
    case AudioController::AudioAddress::NR33:
      mCH3.nrx3 = val;
      break;
    case AudioController::AudioAddress::NR34:
      mCH3.HandleNrx4Write(val);
      break;
    case AudioController::AudioAddress::NR41:
      mCH4.nrx1 = val;
      mCH4.ResetLengthTimer();
      break;
    case AudioController::AudioAddress::NR42:
      mCH4.HandleNrx2Write(val);
      break;
    case AudioController::AudioAddress::NR43:
      mCH4.nrx3 = val;
      break;
    case AudioController::AudioAddress::NR44:
      mCH4.HandleNrx4Write(val);
      break;
    case AudioController::AudioAddress::NR50:
      mNR50 = val;
      break;
    case AudioController::AudioAddress::NR51:
      mNR51 = val;
      break;
    default:
      (void)val;
      break;
  }
}

void AudioController::Reset()
{
  mLBuf.clear();
  mRBuf.clear();
  mCycleCount = {};
  mCH3.wave_form_index = {};
  mNR50 = {};
  mNR51 = {};
  mNR52 = {};

  mCH1.Reset();
  mCH2.Reset();
  mCH3.Reset();
  mCH4.Reset();
}

void AudioController::HandleNr52Write(uint8_t val)
{
  mNR52 = val & 0b10000000;
  mEnabled = mNR52 & GetNr52BitMask(Nr52BitMask::AUDIO_ON);

  if (!mEnabled)
    Reset();
}

uint64_t temp_count{};

void AudioController::SubSample()
{
  uint64_t count  = static_cast<uint64_t>(mLBuf.size() * AUDIO_FREQUENCY / M_RATE);
  double sample_idx_step = M_RATE / AUDIO_FREQUENCY;
  mSubsampleBuf.resize(count * 2);

  double samples_idx_f = 0;
  int    samples_idx = 0;
  for (int i = 0; i < count*2; i += 2)
  {
    mSubsampleBuf[i + 0] = static_cast<short>(mLBuf[samples_idx]  * SHRT_MAX);
    mSubsampleBuf[i + 1] = static_cast<short>(mRBuf[samples_idx] * SHRT_MAX);

    samples_idx_f += sample_idx_step;
    samples_idx = static_cast<int>(round(samples_idx_f));

    temp_count += 1;
  }

  //pcm_out.write(reinterpret_cast<char*>(m_subsample_buffer.data()), m_subsample_buffer.size() * sizeof(short));

  if(m_audio_callback)
    m_audio_callback(mSubsampleBuf);

  mSubsampleBuf.clear();
  mLBuf.clear();
  mRBuf.clear();
}

void AudioController::Update(uint16_t clk)
{
  if(Utils::FallingEdgeDetect(mClk, clk, DIV_APU_BIT_MASK))
    ApuDivTick();

  mClk = clk;

  if (mEnabled)
  {
    UpdateApu();

    double left{}, right{};
    Mixer(left, right);
    mLBuf.push_back(left);
    mRBuf.push_back(right);

    mCycleCount += 4;

    if (mCycleCount == NUM_CYCLES_TO_BUFFER)
    {
      SubSample();
      mCycleCount = 0;
    }
  }
}

uint8_t AudioController::GetChOnBits() const
{
  return (static_cast<int>(mCH4.enabled) << 3) | (static_cast<int>(mCH3.enabled) << 2) | (static_cast<int>(mCH2.enabled) << 1) | static_cast<int>(mCH1.enabled);
}