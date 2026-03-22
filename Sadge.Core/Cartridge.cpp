#include "Cpu.h"

void Cpu::Mbc3_7FFF(uint8_t val)
{

}

void Cpu::Mbc3_5FFF(uint8_t val)
{
  mRamBank = val & 0b11;
}

void Cpu::Mbc3_3FFF(uint8_t val)
{
  m_rom_bank = val & 0x7F;
  m_rom_bank = m_rom_bank == 0 ? 1 : m_rom_bank;
  m_rom_bank %= m_num_rom_banks;
}

void Cpu::Mbc3_1FFF(uint8_t val)
{
  mEramEn = ((val & 0xF) == 0xA);
}

void Cpu::Mbc1_7FFF(uint8_t val)
{
  m_mbc_mode = static_cast<Mbc1Mode>(val & 0x1);
}

void Cpu::Mbc1_5FFF(uint8_t val)
{
  if (m_mbc_mode == Mbc1Mode::MULTIPLE_RAM)
    mRamBank = val & 0b11;
  else
  {
    m_upper_bank_bits = (val & 0b11) << 5;
    m_rom_bank = m_upper_bank_bits | m_lower_bank_bits;
    m_rom_bank %= m_num_rom_banks;
  }
}

void Cpu::Mbc1_3FFF(uint8_t val)
{
  m_lower_bank_bits = val & 0x1F;
  m_lower_bank_bits = m_lower_bank_bits == 0 ? 1 : m_lower_bank_bits;
  m_rom_bank = m_upper_bank_bits | m_lower_bank_bits;
  m_rom_bank %= m_num_rom_banks;
}

void Cpu::Mbc1_1FFF(uint8_t val)
{
  mEramEn = ((val & 0xF) == 0xA);
}
