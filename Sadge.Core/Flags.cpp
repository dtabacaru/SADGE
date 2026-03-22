#include "Cpu.h"

void Cpu::SetHalfCarryFlag(uint8_t val1, int val2)
{
  bool bit_4_set = val2 < 0 ? ((val1 & 0xF) - (-val2 & 0xF)) & 0x10
    : ((val1 & 0xF) + (+val2 & 0xF)) & 0x10;

  bit_4_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
}

void Cpu::SetHalfCarryFlag(uint8_t val1, int val2, int carry)
{
  bool bit_4_set = val2 < 0 || carry < 0 ? ((val1 & 0xF) - (-val2 & 0xF) - (-carry & 0xF)) & 0x10
    : ((val1 & 0xF) + (+val2 & 0xF) + (+carry & 0xF)) & 0x10;

  bit_4_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
}

void Cpu::SetHalfCarryFlag(uint16_t val1, int val2)
{
  bool bit_12_set = val2 < 0 ? ((val1 & 0xFFF) - (-val2 & 0xFFF)) & 0x1000
    : ((val1 & 0xFFF) + (+val2 & 0xFFF)) & 0x1000;

  bit_12_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
}

void Cpu::SetCarry16Bit(int result)
{
  (result > 0xFFFF || result < 0) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
}

void Cpu::SetCarry8Bit(int result)
{
  (result > 0xFF || result < 0) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
}

void Cpu::SetZeroFlag(uint8_t val)
{
  val ? ResetFlag(FlagBitMask::Zero) : SetFlag(FlagBitMask::Zero);
}

void Cpu::SetSubtractionFlag(int val)
{
  val < 0 ? SetFlag(FlagBitMask::Subtract) : ResetFlag(FlagBitMask::Subtract);
}

void Cpu::SetFlag(FlagBitMask flag)
{
  _af.L |= static_cast<uint8_t>(flag);
}

void Cpu::ResetFlag(FlagBitMask flag)
{
  _af.L &= ~static_cast<uint8_t>(flag);
}

bool Cpu::ReadFlag(FlagBitMask flag) const
{
  return _af.L & static_cast<uint8_t>(flag);
}
