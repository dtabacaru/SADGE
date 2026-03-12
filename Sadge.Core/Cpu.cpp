#include "Cpu.h"

#include <fstream>

Cpu::Cpu() : mTimerCtrl(mInterruptCtrl),
  mJoypadCtrl(mInterruptCtrl),
  mLcdCtrl(mInterruptCtrl),
  mSerialCtrl(mInterruptCtrl)
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
      sram.write(reinterpret_cast<const char*>(m_ram_banks[rom_bank_num].data()), ERAM_SIZE);
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

void Cpu::WriteIo(uint16_t address, uint8_t val)
{
  if (address == static_cast<uint16_t>(JoypadController::JoypadAddress::STATE))
  {
    mJoypadCtrl.WriteSelect(val);
  }
  else if (address >= static_cast<uint16_t>(SerialController::Address::START) &&
           address <= static_cast<uint16_t>(SerialController::Address::END))
  {
    mSerialCtrl.HandleWrite(address, val);
  }
  else if (address >= static_cast<uint16_t>(TimerAddress::START) &&
           address <= static_cast<uint16_t>(TimerAddress::END))
  {
    mTimerCtrl.HandleWrite(address, val);
  }
  else if (address == static_cast<uint16_t>(InterruptController::InterruptAddress::FLAG))
  {
    mInterruptCtrl.HandleWrite(address, val);
  }
  else if (address >= static_cast<uint16_t>(AudioController::AudioAddress::START) &&
           address <= static_cast<uint16_t>(AudioController::AudioAddress::END))
  {
    mAudioCtrl.HandleWrite(address, val);
  }
  else if (address >= static_cast<uint16_t>(AudioController::AudioWaveAddress::START) &&
           address <= static_cast<uint16_t>(AudioController::AudioWaveAddress::END))
  {
    mAudioCtrl.HandleWrite(address, val);
  }
  else if (address >= static_cast<uint16_t>(LcdController::Address::START) &&
           address <= static_cast<uint16_t>(LcdController::Address::END))
  {
    mLcdCtrl.HandleWrite(address, val);
  }
  else if (address == static_cast<uint16_t>(ExecutionAddress::BOOT_COMPLETE))
  {
    if (val && !mBooted)
    { 
      mBooted = true;
      std::copy(m_rom.begin(), m_rom.begin() + BOOT_ROM.size(), m_rom_banks[0].begin()); // Unmap boot ROM
    }   
  }
  else
  {
    (void)val;
  }
}

uint8_t Cpu::ReadIo(uint16_t address)
{
  if (address == static_cast<uint16_t>(JoypadController::JoypadAddress::STATE))
  {
    return mJoypadCtrl.ReadJoypad();
  }
  else if (address >= static_cast<uint16_t>(SerialController::Address::START) &&
           address <= static_cast<uint16_t>(SerialController::Address::END))
  {
    return mSerialCtrl.HandleRead(address);
  }
  else if (address >= static_cast<uint16_t>(TimerAddress::START) &&
           address <= static_cast<uint16_t>(TimerAddress::END))
  {
    return mTimerCtrl.HandleRead(address);
  }
  else if (address == static_cast<uint16_t>(InterruptController::InterruptAddress::FLAG))
  {
    return mInterruptCtrl.HandleRead(address);
  }
  else if (address >= static_cast<uint16_t>(AudioController::AudioAddress::START) &&
           address <= static_cast<uint16_t>(AudioController::AudioAddress::END))
  {
    return mAudioCtrl.HandleRead(address);
  }
  else if (address >= static_cast<uint16_t>(AudioController::AudioWaveAddress::START) &&
           address <= static_cast<uint16_t>(AudioController::AudioWaveAddress::END))
  {
    return mAudioCtrl.HandleRead(address);
  }
  else if (address >= static_cast<uint16_t>(LcdController::Address::START) &&
           address <= static_cast<uint16_t>(LcdController::Address::END))
  {
    return mLcdCtrl.HandleRead(address);
  }
  else if (address == static_cast<uint16_t>(ExecutionAddress::BOOT_COMPLETE))
  {
    return mBooted;
  }
  else
  {
    return DEFAULT_READ;
  }
}

void Cpu::WriteAddress(uint16_t address, uint8_t val)
{
  if (address == 0xFFFF) // (FFFF-FFFF) Interrupts Enable Register (IE)
  {
    mInterruptCtrl.HandleWrite(address, val);
  }
  else if (address >= 0xFF80) // (FF80-FFFE) High RAM (HRAM)
  {
    address -= 0xFF80;
    m_hram[address] = val;
  }
  else if (address >= 0xFF00) // (FF00-FF7F) I/O Registers
  {
    WriteIo(address, val);
  }
  else if (address >= 0xFEA0) // (FEA0-FEFF) Not Usable
  {
    (void)val;
  }
  else if (address >= 0xFE00) // (FE00-FE9F) Sprite attribute table (OAM)
  {
    mLcdCtrl.HandleWrite(address, val);
  }
  else if (address >= 0xE000) // (E000-FDFF) Mirror of C000~DDFF (ECHO RAM) Typically not used
  {
    address -= 0xE000;
    m_wram[address] = val;
  }
  else if (address >= 0xC000) // (C000-DFFF) 8KB Work RAM (WRAM)
  {
    address -= 0xC000;
    m_wram[address] = val;
  }
  else if (address >= 0xA000) // (A000-BFFF) 8KB External RAM In cartridge, switchable bank if any
  {
    if (m_external_ram_enable)
    {
      address -= 0xA000;
      m_ram_banks[m_ram_bank][address] = val;
    }
  }
  else if (address >= 0x8000) // (8000-9FFF) 8KB Video RAM (VRAM) bank 0
  {
    mLcdCtrl.HandleWrite(address, val);
  }
  else if (address >= 0x6000) // (6000-7FFF) MBC Mode
  {
    switch (m_rom_header.GetCartridgeType())
    {
      case CartridgeType::MBC1:
        // Fall through
      case CartridgeType::MBC1_RAM:
        // Fall through
      case CartridgeType::MBC1_RAM_BATTERY:
        Mbc1_7FFF(val);
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
        Mbc3_7FFF(val);
        break;
      default:
        break;
    }
    
  }
  else if (address >= 0x4000) // (4000-5FFF) RAM bank or Upper 2 bits of ROM bank
  {
    switch (m_rom_header.GetCartridgeType())
    {
      case CartridgeType::MBC1:
        // Fall through
      case CartridgeType::MBC1_RAM:
        // Fall through
      case CartridgeType::MBC1_RAM_BATTERY:
        Mbc1_5FFF(val);
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
        Mbc3_5FFF(val);
        break;
      default:
        break;
    }
  }
  else if (address >= 0x2000) // (2000-3FFF) Lower 5 bits of ROM bank
  {
    switch (m_rom_header.GetCartridgeType())
    {
      case CartridgeType::MBC1:
        // Fall through
      case CartridgeType::MBC1_RAM:
        // Fall through
      case CartridgeType::MBC1_RAM_BATTERY:
        Mbc1_3FFF(val);
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
        Mbc3_3FFF(val);
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
        Mbc1_1FFF(val);
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
        Mbc3_1FFF(val);
        break;
      default:
        break;
    }
  }
}

uint8_t Cpu::ReadAddress(uint16_t address) 
{
  if (address > 0xFFFE) // (FFFF-FFFF) Interrupts Enable Register (IE)
  {
    return mInterruptCtrl.HandleRead(address);
  }
  else if (address > 0xFF7F) // (FF80-FFFE) High RAM (HRAM)
  {
    address -= 0xFF80;
    return m_hram[address];
  }
  else if (address >= 0xFF00) // (FF00-FF7F) I/O Registers
  {
    return ReadIo(address);
  }
  else if (address >= 0xFEA0) // (FEA0-FEFF) Not Usable
  {
    return DEFAULT_READ;
  }
  else if (address >= 0xFE00) // (FE00-FE9F) Sprite attribute table (OAM)
  {
    return mLcdCtrl.HandleRead(address);
  }
  else if (address >= 0xE000) // (E000-FDFF) Mirror of C000~DDFF (ECHO RAM) Typically not used
  {
    address -= 0xE000;
    return m_wram[address];
  }
  else if (address >= 0xC000) // (C000-DFFF) 8KB Work RAM (WRAM)
  {
    address -= 0xC000;
    return m_wram[address];
  }
  else if (address >= 0xA000) // (A000-BFFF) 8KB External RAM In cartridge, switchable bank if any
  {
    if (m_external_ram_enable)
    {
      address -= 0xA000;
      return m_ram_banks[m_ram_bank][address];
    }
    else
      return DEFAULT_READ;
  }
  else if (address >= 0x8000) // (8000-9FFF) 8KB Video RAM (VRAM) bank 0
  {
    return mLcdCtrl.HandleRead(address);
  }
  else if (address >= 0x4000) // (4000-7FFF) 16KB ROM bank 01~NN From cartridge, switchable bank via MBC (if any)
  {
    address -= 0x4000;
    return m_rom_banks[m_rom_bank][address];
  }
  else //if (address > 0x0000) // (0000-3FFF) 16KB ROM bank 00 From cartridge, usually a fixed bank, BOOT_ROM mapped to 00-FF at start
  {
    return m_rom_banks[0][address];
  }
}

void Cpu::SetState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime, uint8_t ie)
{
  ime;
  ie;

  _pc.hl = pc;
  _sp.hl = sp;
  _af.h = a;
  _bc.h = b;
  _bc.l = c;
  _de.h = d;
  _de.l = e;
  _af.l = f;
  _hl.h = h;
  _hl.l = l;
}

bool Cpu::CheckState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime)
{
  ime;

  if (pc != _pc.hl)
    return false;
  else if (sp != _sp.hl)
    return false;
  else if (a != _af.h)
    return false;
  else if (b != _bc.h)
    return false;
  else if (c != _bc.l)
    return false;
  else if (d != _de.h)
    return false;
  else if (e != _de.l)
    return false;
  else if (f != _af.l)
    return false;
  else if (h != _hl.h)
    return false;
  else if (l != _hl.l)
    return false;
  else
    return true;
}