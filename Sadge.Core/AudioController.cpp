#include "AudioController.h"

#include "raylib.h"

#include <mutex>

std::vector<short> audio_buffer;
std::mutex audio_buffer_lock;
AudioStream audio_stream;

void AudioInputCallback(void* buffer, unsigned int frames)
{
  std::lock_guard<std::mutex> lock(audio_buffer_lock);

  short* d = (short*)buffer;

  auto num_frames = audio_buffer.size() > frames ? frames : audio_buffer.size();
  for (unsigned int i = 0; i < num_frames; i++)
    d[i] = audio_buffer[i];

  if(num_frames > 0)
    audio_buffer.erase(audio_buffer.begin(), audio_buffer.begin() + num_frames);
}

void AudioController::Init()
{
  SetTraceLogLevel(LOG_NONE);
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(375);
  audio_stream = LoadAudioStream(AUDIO_FREQUENCY, AUDIO_BITS, AUDIO_CHANNELS);
  SetAudioStreamCallback(audio_stream, AudioInputCallback);
}

void AudioController::Reset()
{
  m_samples_buffer.clear();
  audio_buffer.clear();
  uint64_t m_cycle_count = 0;
}

AudioController::AudioController()
{

}

AudioController::~AudioController() 
{
  StopAudioStream(audio_stream);
  CloseAudioDevice();
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
      return m_ch1.nr10;
    case AudioController::AudioAddress::NR11:
      return m_ch1.nrx1;
    case AudioController::AudioAddress::NR12:
      return m_ch1.nrx2;
    case AudioController::AudioAddress::NR13:
      return m_ch1.nrx3;
    case AudioController::AudioAddress::NR14:
      return m_ch1.nrx4;
    case AudioController::AudioAddress::NR21:
      return m_ch2.nrx1;
    case AudioController::AudioAddress::NR22:
      return m_ch2.nrx2;
    case AudioController::AudioAddress::NR23:
      return m_ch2.nrx3;
    case AudioController::AudioAddress::NR24:
      return m_ch2.nrx4;
    case AudioController::AudioAddress::NR30:
      return m_ch3.nr30;
    case AudioController::AudioAddress::NR31:
      return m_ch3.nrx1;
    case AudioController::AudioAddress::NR32:
      return m_ch3.nrx2;
    case AudioController::AudioAddress::NR33:
      return m_ch3.nrx3;
    case AudioController::AudioAddress::NR34:
      return m_ch3.nrx4;
    case AudioController::AudioAddress::NR41:
      return m_ch4.nrx1;
    case AudioController::AudioAddress::NR42:
      return m_ch4.nrx2;
    case AudioController::AudioAddress::NR43:
      return m_ch4.nrx3;
    case AudioController::AudioAddress::NR44:
      return m_ch4.nrx4;
    case AudioController::AudioAddress::NR50:
      return m_nr50;
    case AudioController::AudioAddress::NR51:
      return m_nr51;
    case AudioController::AudioAddress::NR52:
      return m_nr52; // TODO: handle CHxON? bits
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

  if (AudioOn())
  {
    if (address >= static_cast<uint16_t>(AudioWaveAddress::START) && address <= static_cast<uint16_t>(AudioWaveAddress::END))
    {
      address -= static_cast<uint16_t>(AudioWaveAddress::START);
      m_ch3.wave_ram[address] = val;
      return;
    }

    switch (audio_address)
    {
      case AudioController::AudioAddress::NR10:
        m_ch1.nr10 = val;
        break;
      case AudioController::AudioAddress::NR11:
        m_ch1.nrx1 = val;
        break;
      case AudioController::AudioAddress::NR12:
        m_ch1.nrx2 = val;
        break;
      case AudioController::AudioAddress::NR13:
        m_ch1.nrx3 = val;
        break;
      case AudioController::AudioAddress::NR14:
        m_ch1.HandleNrx4Write(val);
        break;
      case AudioController::AudioAddress::NR21:
        m_ch2.nrx1 = val;
        break;
      case AudioController::AudioAddress::NR22:
        m_ch2.nrx2 = val;
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
        break;
      case AudioController::AudioAddress::NR42:
        m_ch4.nrx2 = val;
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
  else
    (void)val;
}

void AudioController::HandleNr52Write(uint8_t val)
{
  m_nr52 = val;

  if (AudioOn())
  {
    m_ch3.wave_form_index = 0;
  }
  else
  {
    StopAudioStream(audio_stream);
    audio_buffer.clear();
    m_samples_buffer.clear();
    m_cycle_count = 0;
  }
}

void AudioController::CheckPanning()
{
  if (!(m_nr51 & (1 << 4)) && !(m_nr51 & (1 << 0)))
  {
    m_ch1.ch_out = 0;
  }
  else if (!(m_nr51 & (1 << 5)) && !(m_nr51 & (1 << 1)))
  {
    m_ch2.ch_out = 0;
  }
  else if (!(m_nr51 & (1 << 6)) && !(m_nr51 & (1 << 2)))
  {
    m_ch3.ch_out = 0;
  }
  else if (!(m_nr51 & (1 << 7)) && !(m_nr51 & (1 << 3)))
  {
    m_ch4.ch_out = 0;
  }
}

void AudioController::FillAudioBuffer()
{
  std::lock_guard<std::mutex> lock(audio_buffer_lock);

  uint64_t count = static_cast<uint64_t>((m_samples_buffer.size() / M_RATE) / AUDIO_DT);

  auto closest_sample = m_samples_buffer.begin();
  double audio_cycle = static_cast<double>(m_samples_buffer.front().cycle);
  for (int i = 0; i < count; i += 1)
  {
    while (closest_sample->cycle < audio_cycle)
      closest_sample += 1;

    audio_buffer.push_back(static_cast<short>(closest_sample->level * SHRT_MAX));
    audio_cycle += AUDIO_DT * T_RATE;
  }

  m_samples_buffer.clear();
}

void AudioController::Update(bool frame_ready)
{
  if (AudioOn())
  {
    m_cycle_count += 4;

    m_ch1.Update();
    m_ch2.Update();
    m_ch3.Update();
    m_ch4.Update();

    double out = Mixer();
    m_samples_buffer.push_back({m_cycle_count, out});

    if (m_cycle_count == 32768)
    {
      FillAudioBuffer();

      if(!IsAudioStreamPlaying(audio_stream))
        PlayAudioStream(audio_stream);

      m_cycle_count = 0;
    }
  }
}
