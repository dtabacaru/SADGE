#include "AudioController.h"

#include <fstream>
std::ofstream pcm_out("pcm_out.wav", std::ios_base::binary);

void AudioController::Init()
{

}

void AudioController::ClearSamples()
{
  m_left_samples_buffer.clear();
  m_right_samples_buffer.clear();
  m_cycle_count = 0;
}

AudioController::AudioController()
{
  m_left_samples_buffer.reserve(NUM_CYCLES_TO_BUFFER);
  m_right_samples_buffer.reserve(NUM_CYCLES_TO_BUFFER);
}

AudioController::~AudioController() 
{

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
    return m_ch3.wave_ram[address];
  }

  switch (audio_address)
  {
    case AudioController::AudioAddress::NR10:
      return m_ch1.nr10 | GetUnusedBits(UnusedReadBits::NR10);
    case AudioController::AudioAddress::NR11:
      return m_ch1.nrx1 | GetUnusedBits(UnusedReadBits::NRX1);
    case AudioController::AudioAddress::NR12:
      return m_ch1.nrx2;
    //case AudioController::AudioAddress::NR13: // Write only
    //  return m_ch1.nrx3;
    case AudioController::AudioAddress::NR14:
      return m_ch1.nrx4 | GetUnusedBits(UnusedReadBits::NRX4);
    case AudioController::AudioAddress::NR21:
      return m_ch2.nrx1 | GetUnusedBits(UnusedReadBits::NRX1);
    case AudioController::AudioAddress::NR22:
      return m_ch2.nrx2;
    //case AudioController::AudioAddress::NR23: // Write only
    //  return m_ch2.nrx3;
    case AudioController::AudioAddress::NR24:
      return m_ch2.nrx4 | GetUnusedBits(UnusedReadBits::NRX4);
    case AudioController::AudioAddress::NR30:
      return m_ch3.nr30 | GetUnusedBits(UnusedReadBits::NR30);
    //case AudioController::AudioAddress::NR31: // Write only
    //  return m_ch3.nrx1;
    case AudioController::AudioAddress::NR32:
      return m_ch3.nrx2 | GetUnusedBits(UnusedReadBits::NR32);
    //case AudioController::AudioAddress::NR33: // Write only
    //  return m_ch3.nrx3;
    case AudioController::AudioAddress::NR34:
      return m_ch3.nrx4 | GetUnusedBits(UnusedReadBits::NRX4);
    //case AudioController::AudioAddress::NR41: // Write only
    //  return m_ch4.nrx1;
    case AudioController::AudioAddress::NR42:
      return m_ch4.nrx2;
    case AudioController::AudioAddress::NR43:
      return m_ch4.nrx3;
    case AudioController::AudioAddress::NR44:
      return m_ch4.nrx4 | GetUnusedBits(UnusedReadBits::NRX4);
    case AudioController::AudioAddress::NR50:
      return m_nr50;
    case AudioController::AudioAddress::NR51:
      return m_nr51;
    case AudioController::AudioAddress::NR52:
      return m_nr52 | GetUnusedBits(UnusedReadBits::NR52) | GetChOnBits();
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
    m_ch3.wave_ram[address] = val;
    return;
  }

  if (!m_audio_enabled)
    return;

  switch (audio_address)
  {
    case AudioController::AudioAddress::NR10:
      m_ch1.nr10 = val;
      break;
    case AudioController::AudioAddress::NR11:
      m_ch1.nrx1 = val;
      m_ch1.ResetLengthTimer();
      break;
    case AudioController::AudioAddress::NR12:
      m_ch1.HandleNrx2Write(val);
      break;
    case AudioController::AudioAddress::NR13:
      m_ch1.nrx3 = val;
      break;
    case AudioController::AudioAddress::NR14:
      m_ch1.HandleNrx4Write(val);
      break;
    case AudioController::AudioAddress::NR21:
      m_ch2.nrx1 = val;
      m_ch2.ResetLengthTimer();
      break;
    case AudioController::AudioAddress::NR22:
      m_ch2.HandleNrx2Write(val);
      break;
    case AudioController::AudioAddress::NR23:
      m_ch2.nrx3 = val;
      break;
    case AudioController::AudioAddress::NR24:
      m_ch2.HandleNrx4Write(val);
      break;
    case AudioController::AudioAddress::NR30:
      m_ch3.HandleNr30Write(val);
      break;
    case AudioController::AudioAddress::NR31:
      m_ch3.nrx1 = val;
      m_ch3.ResetLengthTimer();
      break;
    case AudioController::AudioAddress::NR32:
      m_ch3.nrx2 = val;
      break;
    case AudioController::AudioAddress::NR33:
      m_ch3.nrx3 = val;
      break;
    case AudioController::AudioAddress::NR34:
      m_ch3.HandleNrx4Write(val);
      break;
    case AudioController::AudioAddress::NR41:
      m_ch4.nrx1 = val;
      m_ch4.ResetLengthTimer();
      break;
    case AudioController::AudioAddress::NR42:
      m_ch4.HandleNrx2Write(val);
      break;
    case AudioController::AudioAddress::NR43:
      m_ch4.nrx3 = val;
      break;
    case AudioController::AudioAddress::NR44:
      m_ch4.HandleNrx4Write(val);
      break;
    case AudioController::AudioAddress::NR50:
      m_nr50 = val;
      break;
    case AudioController::AudioAddress::NR51:
      m_nr51 = val;
      break;
    default:
      (void)val;
      break;
  }
}

void AudioController::Reset()
{
  m_left_samples_buffer.clear();
  m_right_samples_buffer.clear();
  m_cycle_count = {};
  m_ch3.wave_form_index = {};
  m_nr50 = {};
  m_nr51 = {};
  m_nr52 = {};

  m_ch1.Reset();
  m_ch2.Reset();
  m_ch3.Reset();
  m_ch4.Reset();
}

void AudioController::HandleNr52Write(uint8_t val)
{
  m_nr52 = val & 0b10000000;
  m_audio_enabled = m_nr52 & GetNr52BitMask(Nr52BitMask::AUDIO_ON);

  if (!m_audio_enabled)
    Reset();
}

void AudioController::SubSample()
{
  uint64_t count  = static_cast<uint64_t>(m_left_samples_buffer.size() * AUDIO_FREQUENCY / M_RATE);
  double sample_idx_step = M_RATE / AUDIO_FREQUENCY;
  m_subsample_buffer.resize(count * 2);

  double samples_idx_f = 0;
  int    samples_idx = 0;
  for (int i = 0; i < count*2; i += 2)
  {
    m_subsample_buffer[i + 0] = static_cast<short>(m_left_samples_buffer[samples_idx]  * SHRT_MAX);
    m_subsample_buffer[i + 1] = static_cast<short>(m_right_samples_buffer[samples_idx] * SHRT_MAX);

    samples_idx_f += sample_idx_step;
    samples_idx = static_cast<int>(round(samples_idx_f));
  }

  pcm_out.write(reinterpret_cast<char*>(m_subsample_buffer.data()), m_subsample_buffer.size() * sizeof(short));

  if(m_audio_callback)
    m_audio_callback(m_subsample_buffer);

  m_subsample_buffer.clear();
  m_left_samples_buffer.clear();
  m_right_samples_buffer.clear();
}

void AudioController::Update()
{
  if (m_audio_enabled)
  {
    UpdateApu();

    double left{}, right{};
    Mixer(left, right);
    m_left_samples_buffer.push_back(left);
    m_right_samples_buffer.push_back(right);

    m_cycle_count += 4;

    if (m_cycle_count == NUM_CYCLES_TO_BUFFER)
    {
      SubSample();
      m_cycle_count = 0;
    }
  }
}
