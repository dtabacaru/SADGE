#include "Cpu.h"

#include <fstream>

Cpu::Cpu() : mAudioCtrl(mDataBus, mAddressBus),
  mInterruptCtrl(mDataBus, mAddressBus),
  mJoypadCtrl(mInterruptCtrl, mDataBus, mAddressBus),
  mLcdCtrl(mInterruptCtrl),
  mSerialCtrl(mInterruptCtrl, mDataBus, mAddressBus),
  mTimerCtrl(mInterruptCtrl, mDataBus, mAddressBus)
{
}

Cpu::~Cpu()
{
  WriteEram();
}

void Cpu::WriteEram()
{
  if (m_battery_flag)
  {
    std::ofstream sram = std::ofstream(m_save_file_path, std::ios_base::binary);

    for (int rom_bank_num = 0; rom_bank_num < m_num_ram_banks; rom_bank_num += 1)
      sram.write(reinterpret_cast<const char*>(mRamBanks[rom_bank_num].data()), ERAM_SIZE);
  }
}

void Cpu::RunUntil(uint64_t count)
{
  Init();

  while (mTotalCycles < count)
    Main();
}

void Cpu::Run()
{
  Init();

  while (!mStopped)
    Main();
}

void Cpu::Stop()
{
  mStopped = true;
}

void Cpu::WriteIo()
{
  if (mAddressBus == JoypadController::GetAddress(JoypadController::Address::STATE))
  {
    mJoypadCtrl.Write();
  }
  else if (mAddressBus >= SerialController::GetAddress(SerialController::Address::START) &&
           mAddressBus <= SerialController::GetAddress(SerialController::Address::END))
  {
    mSerialCtrl.Write();
  }
  else if (mAddressBus >= TimerController::GetAddress(TimerController::Address::START) &&
           mAddressBus <= TimerController::GetAddress(TimerController::Address::END))
  {
    mTimerCtrl.Write();
  }
  else if (mAddressBus == InterruptController::GetAddress(InterruptController::InterruptAddress::FLAG))
  {
    mInterruptCtrl.Write();
  }
  else if (mAddressBus >= static_cast<uint16_t>(AudioController::Address::START) &&
           mAddressBus <= static_cast<uint16_t>(AudioController::Address::END))
  {
    mAudioCtrl.Write();
  }
  else if (mAddressBus >= AudioController::GetWaveAddress(AudioController::WaveAddress::START) &&
           mAddressBus <= AudioController::GetWaveAddress(AudioController::WaveAddress::END))
  {
    mAudioCtrl.Write();
  }
  else if (mAddressBus >= static_cast<uint16_t>(LcdController::Address::START) &&
           mAddressBus <= static_cast<uint16_t>(LcdController::Address::END))
  {
    mLcdCtrl.HandleWrite(mAddressBus, mDataBus);
  }
  else if (mAddressBus == static_cast<uint16_t>(ExecutionAddress::BOOT_COMPLETE))
  {
    if (mDataBus && !mBooted)
    { 
      std::copy(m_rom.begin(), m_rom.begin() + BOOT_ROM.size(), mRomBanks[0].begin());
      mBooted = true;
    }
  }
}

void Cpu::ReadIo()
{
  if (mAddressBus == JoypadController::GetAddress(JoypadController::Address::STATE))
  {
    mJoypadCtrl.Read();
  }
  else if (mAddressBus >= SerialController::GetAddress(SerialController::Address::START) &&
           mAddressBus <= SerialController::GetAddress(SerialController::Address::END))
  {
    mSerialCtrl.Read();
  }
  else if (mAddressBus >= TimerController::GetAddress(TimerController::Address::START) &&
           mAddressBus <= TimerController::GetAddress(TimerController::Address::END))
  {
    mTimerCtrl.Read();
  }
  else if (mAddressBus == InterruptController::GetAddress(InterruptController::InterruptAddress::FLAG))
  {
    mInterruptCtrl.Read();
  }
  else if (mAddressBus >= AudioController::GetAddress(AudioController::Address::START) &&
           mAddressBus <= AudioController::GetAddress(AudioController::Address::END))
  {
    mAudioCtrl.Read();
  }
  else if (mAddressBus >= AudioController::GetWaveAddress(AudioController::WaveAddress::START) &&
           mAddressBus <= AudioController::GetWaveAddress(AudioController::WaveAddress::END))
  {
    mAudioCtrl.Read();
  }
  else if (mAddressBus >= static_cast<uint16_t>(LcdController::Address::START) &&
           mAddressBus <= static_cast<uint16_t>(LcdController::Address::END))
  {
    mDataBus = mLcdCtrl.HandleRead(mAddressBus);
  }
  else
  {
    mDataBus = DEFAULT_READ;
  }
}

void Cpu::Write()
{
  if (mAddressBus == 0xFFFF) // (FFFF-FFFF) Interrupts Enable Register (IE)
  {
    mInterruptCtrl.Write();
  }
  else if (mAddressBus >= 0xFF80) // (FF80-FFFE) High RAM (HRAM)
  {
    m_hram[mAddressBus - 0xFF80] = mDataBus;
  }
  else if (mAddressBus >= 0xFF00) // (FF00-FF7F) I/O Registers
  {
    WriteIo();
  }
  else if (mAddressBus >= 0xFEA0) // (FEA0-FEFF) Not Usable
  {
    // Ayyyy
  }
  else if (mAddressBus >= 0xFE00) // (FE00-FE9F) Sprite attribute table (OAM)
  {
    mLcdCtrl.HandleWrite(mAddressBus, mDataBus);
  }
  else if (mAddressBus >= 0xE000) // (E000-FDFF) Mirror of C000~DDFF (ECHO RAM) Typically not used
  {
    m_wram[mAddressBus - 0xE000] = mDataBus;
  }
  else if (mAddressBus >= 0xC000) // (C000-DFFF) 8KB Work RAM (WRAM)
  {
    m_wram[mAddressBus - 0xC000] = mDataBus;
  }
  else if (mAddressBus >= 0xA000) // (A000-BFFF) 8KB External RAM In cartridge, switchable bank if any
  {
    if (mEramEn)
    {

      mRamBanks[mRamBank][mAddressBus - 0xA000] = mDataBus;
    }
  }
  else if (mAddressBus >= 0x8000) // (8000-9FFF) 8KB Video RAM (VRAM) bank 0
  {
    mLcdCtrl.HandleWrite(mAddressBus, mDataBus);
  }
  else if (mAddressBus >= 0x6000) // (6000-7FFF) MBC Mode
  {
    switch (m_rom_header.GetCartridgeType())
    {
      case CartridgeType::MBC1:
        // Fall through
      case CartridgeType::MBC1_RAM:
        // Fall through
      case CartridgeType::MBC1_RAM_BATTERY:
        Mbc1_7FFF(mDataBus);
        break;
      case CartridgeType::MBC3:
        // Fall through
      case CartridgeType::MBC3_RAM:
        // Fall through
      case CartridgeType::MBC3_RAM_BATTERY:
        // Fall through
      case CartridgeType::MBC3_TIMER_RAM_BATTERY:
        // Fall through
      case CartridgeType::MBC5:
        // Fall through
      case CartridgeType::MBC5_RAM:
        // Fall through
      case CartridgeType::MBC5_RAM_BATTERY:
        Mbc3_7FFF(mDataBus);
        break;
      default:
        break;
    }
    
  }
  else if (mAddressBus >= 0x4000) // (4000-5FFF) RAM bank or Upper 2 bits of ROM bank
  {
    switch (m_rom_header.GetCartridgeType())
    {
      case CartridgeType::MBC1:
        // Fall through
      case CartridgeType::MBC1_RAM:
        // Fall through
      case CartridgeType::MBC1_RAM_BATTERY:
        Mbc1_5FFF(mDataBus);
        break;
      case CartridgeType::MBC3:
        // Fall through
      case CartridgeType::MBC3_RAM:
        // Fall through
      case CartridgeType::MBC3_RAM_BATTERY:
        // Fall through
      case CartridgeType::MBC3_TIMER_RAM_BATTERY:
        // Fall through
      case CartridgeType::MBC5:
        // Fall through
      case CartridgeType::MBC5_RAM:
        // Fall through
      case CartridgeType::MBC5_RAM_BATTERY:
        Mbc3_5FFF(mDataBus);
        break;
      default:
        break;
    }
  }
  else if (mAddressBus >= 0x2000) // (2000-3FFF) Lower 5 bits of ROM bank
  {
    switch (m_rom_header.GetCartridgeType())
    {
      case CartridgeType::MBC1:
        // Fall through
      case CartridgeType::MBC1_RAM:
        // Fall through
      case CartridgeType::MBC1_RAM_BATTERY:
        Mbc1_3FFF(mDataBus);
        break;
      case CartridgeType::MBC3:
        // Fall through
      case CartridgeType::MBC3_RAM:
        // Fall through
      case CartridgeType::MBC3_RAM_BATTERY:
        // Fall through
      case CartridgeType::MBC3_TIMER_RAM_BATTERY:
        // Fall through
      case CartridgeType::MBC5:
        // Fall through
      case CartridgeType::MBC5_RAM:
        // Fall through
      case CartridgeType::MBC5_RAM_BATTERY:
        Mbc3_3FFF(mDataBus);
        break;
      default:
        break;
    }
  }
  else //if (address > 0x0000) // (0000-1FFF) External RAM enable
  {
    switch (m_rom_header.GetCartridgeType())
    {
      case CartridgeType::MBC1:
        // Fall through
      case CartridgeType::MBC1_RAM:
        // Fall through
      case CartridgeType::MBC1_RAM_BATTERY:
        Mbc1_1FFF(mDataBus);
        break;
      case CartridgeType::MBC3:
        // Fall through
      case CartridgeType::MBC3_RAM:
        // Fall through
      case CartridgeType::MBC3_RAM_BATTERY:
        // Fall through
      case CartridgeType::MBC3_TIMER_RAM_BATTERY:
        // Fall through
      case CartridgeType::MBC5:
        // Fall through
      case CartridgeType::MBC5_RAM:
        // Fall through
      case CartridgeType::MBC5_RAM_BATTERY:
        Mbc3_1FFF(mDataBus);
        break;
      default:
        break;
    }
  }
}

void Cpu::Read() 
{
  if (mAddressBus > 0xFFFE) // (FFFF-FFFF) Interrupts Enable Register (IE)
  {
    mInterruptCtrl.Read();
  }
  else if (mAddressBus > 0xFF7F) // (FF80-FFFE) High RAM (HRAM)
  {
    mDataBus = m_hram[mAddressBus - 0xFF80];
  }
  else if (mAddressBus >= 0xFF00) // (FF00-FF7F) I/O Registers
  {
    ReadIo();
  }
  else if (mAddressBus >= 0xFEA0) // (FEA0-FEFF) Not Usable
  {
    mDataBus = DEFAULT_READ;
  }
  else if (mAddressBus >= 0xFE00) // (FE00-FE9F) Sprite attribute table (OAM)
  {
    mDataBus = mLcdCtrl.HandleRead(mAddressBus);
  }
  else if (mAddressBus >= 0xE000) // (E000-FDFF) Mirror of C000~DDFF (ECHO RAM) Typically not used
  {
    mDataBus = m_wram[mAddressBus - 0xE000];
  }
  else if (mAddressBus >= 0xC000) // (C000-DFFF) 8KB Work RAM (WRAM)
  {
    mDataBus = m_wram[mAddressBus - 0xC000];
  }
  else if (mAddressBus >= 0xA000) // (A000-BFFF) 8KB External RAM In cartridge, switchable bank if any
  {
    if (mEramEn)
      mDataBus = mRamBanks[mRamBank][mAddressBus - 0xA000];
    else
      mDataBus = DEFAULT_READ;
  }
  else if (mAddressBus >= 0x8000) // (8000-9FFF) 8KB Video RAM (VRAM) bank 0
  {
    mDataBus = mLcdCtrl.HandleRead(mAddressBus);
  }
  else if (mAddressBus >= 0x4000) // (4000-7FFF) 16KB ROM bank 01~NN From cartridge, switchable bank via MBC (if any)
  {
    mDataBus = mRomBanks[m_rom_bank][mAddressBus - 0x4000];
  }
  else //if (address > 0x0000) // (0000-3FFF) 16KB ROM bank 00 From cartridge, usually a fixed bank, BOOT_ROM mapped to 00-FF at start
  {
    mDataBus = mRomBanks[0][mAddressBus];
  }
}

void Cpu::SetState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime, uint8_t ie)
{
  ime;
  ie;

  mPC.HL = pc;
  mSP.HL = sp;
  _af.H = a;
  _bc.H = b;
  _bc.L = c;
  _de.H = d;
  _de.L = e;
  _af.L = f;
  mHL.H = h;
  mHL.L = l;
}

bool Cpu::CheckState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime)
{
  ime;

  if (pc != mPC.HL)
    return false;
  else if (sp != mSP.HL)
    return false;
  else if (a != _af.H)
    return false;
  else if (b != _bc.H)
    return false;
  else if (c != _bc.L)
    return false;
  else if (d != _de.H)
    return false;
  else if (e != _de.L)
    return false;
  else if (f != _af.L)
    return false;
  else if (h != mHL.H)
    return false;
  else if (l != mHL.L)
    return false;
  else
    return true;
}