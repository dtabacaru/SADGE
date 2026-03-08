#include "Cpu.h"

void Cpu::RLC_R(uint8_t& R)
{
  (R & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  uint8_t result = R << 1 | R >> 7;
  SetZeroFlag(result);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  R = result;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::RLC__HL()
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l << 1 | _wz.l >> 7;
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      SetZeroFlag(result);
      ResetFlag(FlagBitMask::Subtract);
      ResetFlag(FlagBitMask::HalfCarry);
      (_wz.l & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::RRC_R(uint8_t& R)
{
  (R & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  uint8_t result = R >> 1 | R << 7;
  SetZeroFlag(result);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  R = result;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::RRC__HL()
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l >> 1 | _wz.l << 7;
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      SetZeroFlag(result);
      ResetFlag(FlagBitMask::Subtract);
      ResetFlag(FlagBitMask::HalfCarry);
      (_wz.l & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::RL_R(uint8_t& R)
{
  uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
  (R & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  uint8_t result = R << 1 | carry_bit;
  SetZeroFlag(result);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  R = result;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::RL__HL()
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l << 1 | static_cast<uint8_t>(ReadFlag(FlagBitMask::Carry));
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      SetZeroFlag(result);
      ResetFlag(FlagBitMask::Subtract);
      ResetFlag(FlagBitMask::HalfCarry);
      (_wz.l & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::RR_R(uint8_t& R)
{
  uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
  (R & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  uint8_t result = R >> 1 | (carry_bit << 7);
  SetZeroFlag(result);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  R = result;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::RR__HL()
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l >> 1 | (ReadFlag(FlagBitMask::Carry) << 7);
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      SetZeroFlag(result);
      ResetFlag(FlagBitMask::Subtract);
      ResetFlag(FlagBitMask::HalfCarry);
      (_wz.l & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SLA_R(uint8_t& R)
{
  (R & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  uint8_t result = R << 1;
  SetZeroFlag(result);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  R = result;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::SLA__HL()
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l << 1;
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      SetZeroFlag(result);
      ResetFlag(FlagBitMask::Subtract);
      ResetFlag(FlagBitMask::HalfCarry);
      (_wz.l & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SRA_R(uint8_t& R)
{
  (R & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  uint8_t result = R >> 1 | (R & 0b10000000);
  SetZeroFlag(result);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  R = result;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::SRA__HL()
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l >> 1 | (_wz.l & 0b10000000);
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      SetZeroFlag(result);
      ResetFlag(FlagBitMask::Subtract);
      ResetFlag(FlagBitMask::HalfCarry);
      (_wz.l & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;

      break;
    default:
      break;
  }
}

void Cpu::SWAP_R(uint8_t& R)
{
  uint8_t result = (R >> 4) | (R << 4);
  SetZeroFlag(result);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  ResetFlag(FlagBitMask::Carry);
  R = result;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::SWAP__HL()
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = (_wz.l >> 4) | (_wz.l << 4);
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      SetZeroFlag(result);
      ResetFlag(FlagBitMask::Subtract);
      ResetFlag(FlagBitMask::HalfCarry);
      ResetFlag(FlagBitMask::Carry);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SRL_R(uint8_t& R)
{
  (R & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  uint8_t result = R >> 1;
  SetZeroFlag(result);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  R = result;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::SRL__HL()
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l >> 1;
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      SetZeroFlag(result);
      ResetFlag(FlagBitMask::Subtract);
      ResetFlag(FlagBitMask::HalfCarry);
      (_wz.l & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::BIT_R(uint8_t bit_mask, uint8_t R)
{
  (R & bit_mask) ? ResetFlag(FlagBitMask::Zero) : SetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  SetFlag(FlagBitMask::HalfCarry);
  mExeCycle = EXE_COMPLETE;
}

void Cpu::BIT__HL(uint8_t bit_mask)
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      (_wz.l & bit_mask) ? ResetFlag(FlagBitMask::Zero) : SetFlag(FlagBitMask::Zero);
      ResetFlag(FlagBitMask::Subtract);
      SetFlag(FlagBitMask::HalfCarry);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::RES_R(uint8_t bit_mask, uint8_t& R)
{
  R &= ~bit_mask;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::RES__HL(uint8_t bit_mask)
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l & ~bit_mask;
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SET_R(uint8_t bit_mask, uint8_t& R)
{
  R |= bit_mask;
  mExeCycle = EXE_COMPLETE;
}

void Cpu::SET__HL(uint8_t bit_mask)
{
  uint8_t result{};
  switch (mExeCycle)
  {
    case 1:
      mAddressBus = _hl.hl;
      mDataBus = ReadAddress(mAddressBus);
      _wz.l = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      result = _wz.l | bit_mask;
      mDataBus = result;
      WriteAddress(mAddressBus, mDataBus);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::OpCb0x00()
{
  RLC_R(_bc.h);
}

void Cpu::OpCb0x01()
{
  RLC_R(_bc.l);
}

void Cpu::OpCb0x02()
{
  RLC_R(_de.h);
}

void Cpu::OpCb0x03()
{
  RLC_R(_de.l);
}

void Cpu::OpCb0x04()
{
  RLC_R(_hl.h);
}

void Cpu::OpCb0x05()
{
  RLC_R(_hl.l);
}

void Cpu::OpCb0x06()
{
  RLC__HL();
}

void Cpu::OpCb0x07()
{
  RLC_R(_af.h);
}

void Cpu::OpCb0x08()
{
  RRC_R(_bc.h);
}

void Cpu::OpCb0x09()
{
  RRC_R(_bc.l);
}

void Cpu::OpCb0x0A()
{
  RRC_R(_de.h);
}

void Cpu::OpCb0x0B()
{
  RRC_R(_de.l);
}

void Cpu::OpCb0x0C()
{
  RRC_R(_hl.h);
}

void Cpu::OpCb0x0D()
{
  RRC_R(_hl.l);
}

void Cpu::OpCb0x0E()
{
  RRC__HL();
}

void Cpu::OpCb0x0F()
{
  RRC_R(_af.h);
}

void Cpu::OpCb0x10()
{
  RL_R(_bc.h);
}

void Cpu::OpCb0x11()
{
  RL_R(_bc.l);
}

void Cpu::OpCb0x12()
{
  RL_R(_de.h);
}

void Cpu::OpCb0x13()
{
  RL_R(_de.l);
}

void Cpu::OpCb0x14()
{
  RL_R(_hl.h);
}

void Cpu::OpCb0x15()
{
  RL_R(_hl.l);
}

void Cpu::OpCb0x16()
{
  RL__HL();
}

void Cpu::OpCb0x17()
{
  RL_R(_af.h);
}

void Cpu::OpCb0x18()
{
  RR_R(_bc.h);
}

void Cpu::OpCb0x19()
{
  RR_R(_bc.l);
}

void Cpu::OpCb0x1A()
{
  RR_R(_de.h);
}

void Cpu::OpCb0x1B()
{
  RR_R(_de.l);
}

void Cpu::OpCb0x1C()
{
  RR_R(_hl.h);
}

void Cpu::OpCb0x1D()
{
  RR_R(_hl.l);
}

void Cpu::OpCb0x1E()
{
  RR__HL();
}

void Cpu::OpCb0x1F()
{
  RR_R(_af.h);
}

void Cpu::OpCb0x20()
{
  SLA_R(_bc.h);
}

void Cpu::OpCb0x21()
{
  SLA_R(_bc.l);
}

void Cpu::OpCb0x22()
{
  SLA_R(_de.h);
}

void Cpu::OpCb0x23()
{
  SLA_R(_de.l);
}

void Cpu::OpCb0x24()
{
  SLA_R(_hl.h);
}

void Cpu::OpCb0x25()
{
  SLA_R(_hl.l);
}

void Cpu::OpCb0x26()
{
  SLA__HL();
}

void Cpu::OpCb0x27()
{
  SLA_R(_af.h);
}

void Cpu::OpCb0x28()
{
  SRA_R(_bc.h);
}

void Cpu::OpCb0x29()
{
  SRA_R(_bc.l);
}

void Cpu::OpCb0x2A()
{
  SRA_R(_de.h);
}

void Cpu::OpCb0x2B()
{
  SRA_R(_de.l);
}

void Cpu::OpCb0x2C()
{
  SRA_R(_hl.h);
}

void Cpu::OpCb0x2D()
{
  SRA_R(_hl.l);
}

void Cpu::OpCb0x2E()
{
  SRA__HL();
}

void Cpu::OpCb0x2F()
{
  SRA_R(_af.h);
}

void Cpu::OpCb0x30()
{
  SWAP_R(_bc.h);
}

void Cpu::OpCb0x31()
{
  SWAP_R(_bc.l);
}

void Cpu::OpCb0x32()
{
  SWAP_R(_de.h);
}

void Cpu::OpCb0x33()
{
  SWAP_R(_de.l);
}

void Cpu::OpCb0x34()
{
  SWAP_R(_hl.h);
}

void Cpu::OpCb0x35()
{
  SWAP_R(_hl.l);
}

void Cpu::OpCb0x36()
{
  SWAP__HL();
}

void Cpu::OpCb0x37()
{
  SWAP_R(_af.h);
}

void Cpu::OpCb0x38()
{
  SRL_R(_bc.h);
}

void Cpu::OpCb0x39()
{
  SRL_R(_bc.l);
}

void Cpu::OpCb0x3A()
{
  SRL_R(_de.h);
}

void Cpu::OpCb0x3B()
{
  SRL_R(_de.l);
}

void Cpu::OpCb0x3C()
{
  SRL_R(_hl.h);
}

void Cpu::OpCb0x3D()
{
  SRL_R(_hl.l);
}

void Cpu::OpCb0x3E()
{
  SRL__HL();
}

void Cpu::OpCb0x3F()
{
  SRL_R(_af.h);
}

void Cpu::OpCb0x40()
{
  BIT_R(0b00000001, _bc.h);
}

void Cpu::OpCb0x41()
{
  BIT_R(0b00000001, _bc.l);
}

void Cpu::OpCb0x42()
{
  BIT_R(0b00000001, _de.h);
}

void Cpu::OpCb0x43()
{
  BIT_R(0b00000001, _de.l);
}

void Cpu::OpCb0x44()
{
  BIT_R(0b00000001, _hl.h);
}

void Cpu::OpCb0x45()
{
  BIT_R(0b00000001, _hl.l);
}

void Cpu::OpCb0x46()
{
  BIT__HL(0b00000001);
}

void Cpu::OpCb0x47()
{
  BIT_R(0b00000001, _af.h);
}

void Cpu::OpCb0x48()
{
  BIT_R(0b00000010, _bc.h);
}

void Cpu::OpCb0x49()
{
  BIT_R(0b00000010, _bc.l);
}

void Cpu::OpCb0x4A()
{
  BIT_R(0b00000010, _de.h);
}

void Cpu::OpCb0x4B()
{
  BIT_R(0b00000010, _de.l);
}

void Cpu::OpCb0x4C()
{
  BIT_R(0b00000010, _hl.h);
}

void Cpu::OpCb0x4D()
{
  BIT_R(0b00000010, _hl.l);
}

void Cpu::OpCb0x4E()
{
  BIT__HL(0b00000010);
}

void Cpu::OpCb0x4F()
{
  BIT_R(0b00000010, _af.h);
}

void Cpu::OpCb0x50()
{
  BIT_R(0b00000100, _bc.h);
}

void Cpu::OpCb0x51()
{
  BIT_R(0b00000100, _bc.l);
}

void Cpu::OpCb0x52()
{
  BIT_R(0b00000100, _de.h);
}

void Cpu::OpCb0x53()
{
  BIT_R(0b00000100, _de.l);
}

void Cpu::OpCb0x54()
{
  BIT_R(0b00000100, _hl.h);
}

void Cpu::OpCb0x55()
{
  BIT_R(0b00000100, _hl.l);
}

void Cpu::OpCb0x56()
{
  BIT__HL(0b00000100);
}

void Cpu::OpCb0x57()
{
  BIT_R(0b00000100, _af.h);
}

void Cpu::OpCb0x58()
{
  BIT_R(0b00001000, _bc.h);
}

void Cpu::OpCb0x59()
{
  BIT_R(0b00001000, _bc.l);
}

void Cpu::OpCb0x5A()
{
  BIT_R(0b00001000, _de.h);
}

void Cpu::OpCb0x5B()
{
  BIT_R(0b00001000, _de.l);
}

void Cpu::OpCb0x5C()
{
  BIT_R(0b00001000, _hl.h);
}

void Cpu::OpCb0x5D()
{
  BIT_R(0b00001000, _hl.l);
}

void Cpu::OpCb0x5E()
{
  BIT__HL(0b00001000);
}

void Cpu::OpCb0x5F()
{
  BIT_R(0b00001000, _af.h);
}

void Cpu::OpCb0x60()
{
  BIT_R(0b00010000, _bc.h);
}

void Cpu::OpCb0x61()
{
  BIT_R(0b00010000, _bc.l);
}

void Cpu::OpCb0x62()
{
  BIT_R(0b00010000, _de.h);
}

void Cpu::OpCb0x63()
{
  BIT_R(0b00010000, _de.l);
}

void Cpu::OpCb0x64()
{
  BIT_R(0b00010000, _hl.h);
}

void Cpu::OpCb0x65()
{
  BIT_R(0b00010000, _hl.l);
}

void Cpu::OpCb0x66()
{
  BIT__HL(0b00010000);
}

void Cpu::OpCb0x67()
{
  BIT_R(0b00010000, _af.h);
}

void Cpu::OpCb0x68()
{
  BIT_R(0b00100000, _bc.h);
}

void Cpu::OpCb0x69()
{
  BIT_R(0b00100000, _bc.l);
}

void Cpu::OpCb0x6A()
{
  BIT_R(0b00100000, _de.h);
}

void Cpu::OpCb0x6B()
{
  BIT_R(0b00100000, _de.l);
}

void Cpu::OpCb0x6C()
{
  BIT_R(0b00100000, _hl.h);
}

void Cpu::OpCb0x6D()
{
  BIT_R(0b00100000, _hl.l);
}

void Cpu::OpCb0x6E()
{
  BIT__HL(0b00100000);
}

void Cpu::OpCb0x6F()
{
  BIT_R(0b00100000, _af.h);
}

void Cpu::OpCb0x70()
{
  BIT_R(0b01000000, _bc.h);
}

void Cpu::OpCb0x71()
{
  BIT_R(0b01000000, _bc.l);
}

void Cpu::OpCb0x72()
{
  BIT_R(0b01000000, _de.h);
}

void Cpu::OpCb0x73()
{
  BIT_R(0b01000000, _de.l);
}

void Cpu::OpCb0x74()
{
  BIT_R(0b01000000, _hl.h);
}

void Cpu::OpCb0x75()
{
  BIT_R(0b01000000, _hl.l);
}

void Cpu::OpCb0x76()
{
  BIT__HL(0b01000000);
}

void Cpu::OpCb0x77()
{
  BIT_R(0b01000000, _af.h);
}

void Cpu::OpCb0x78()
{
  BIT_R(0b10000000, _bc.h);
}

void Cpu::OpCb0x79()
{
  BIT_R(0b10000000, _bc.l);
}

void Cpu::OpCb0x7A()
{
  BIT_R(0b10000000, _de.h);
}

void Cpu::OpCb0x7B()
{
  BIT_R(0b10000000, _de.l);
}

void Cpu::OpCb0x7C()
{
  BIT_R(0b10000000, _hl.h);
}

void Cpu::OpCb0x7D()
{
  BIT_R(0b10000000, _hl.l);
}

void Cpu::OpCb0x7E()
{
  BIT__HL(0b10000000);
}

void Cpu::OpCb0x7F()
{
  BIT_R(0b10000000, _af.h);
}

void Cpu::OpCb0x80()
{
  RES_R(0b00000001, _bc.h);
}

void Cpu::OpCb0x81()
{
  RES_R(0b00000001, _bc.l);
}

void Cpu::OpCb0x82()
{
  RES_R(0b00000001, _de.h);
}

void Cpu::OpCb0x83()
{
  RES_R(0b00000001, _de.l);
}

void Cpu::OpCb0x84()
{
  RES_R(0b00000001, _hl.h);
}

void Cpu::OpCb0x85()
{
  RES_R(0b00000001, _hl.l);
}

void Cpu::OpCb0x86()
{
  RES__HL(0b00000001);
}

void Cpu::OpCb0x87()
{
  RES_R(0b00000001, _af.h);
}

void Cpu::OpCb0x88()
{
  RES_R(0b00000010, _bc.h);
}

void Cpu::OpCb0x89()
{
  RES_R(0b00000010, _bc.l);
}

void Cpu::OpCb0x8A()
{
  RES_R(0b00000010, _de.h);
}

void Cpu::OpCb0x8B()
{
  RES_R(0b00000010, _de.l);
}

void Cpu::OpCb0x8C()
{
  RES_R(0b00000010, _hl.h);
}

void Cpu::OpCb0x8D()
{
  RES_R(0b00000010, _hl.l);
}

void Cpu::OpCb0x8E()
{
  RES__HL(0b00000010);
}

void Cpu::OpCb0x8F()
{
  RES_R(0b00000010, _af.h);
}

void Cpu::OpCb0x90()
{
  RES_R(0b00000100, _bc.h);
}

void Cpu::OpCb0x91()
{
  RES_R(0b00000100, _bc.l);
}

void Cpu::OpCb0x92()
{
  RES_R(0b00000100, _de.h);
}

void Cpu::OpCb0x93()
{
  RES_R(0b00000100, _de.l);
}

void Cpu::OpCb0x94()
{
  RES_R(0b00000100, _hl.h);
}

void Cpu::OpCb0x95()
{
  RES_R(0b00000100, _hl.l);
}

void Cpu::OpCb0x96()
{
  RES__HL(0b00000100);
}

void Cpu::OpCb0x97()
{
  RES_R(0b00000100, _af.h);
}

void Cpu::OpCb0x98()
{
  RES_R(0b00001000, _bc.h);
}

void Cpu::OpCb0x99()
{
  RES_R(0b00001000, _bc.l);
}

void Cpu::OpCb0x9A()
{
  RES_R(0b00001000, _de.h);
}

void Cpu::OpCb0x9B()
{
  RES_R(0b00001000, _de.l);
}

void Cpu::OpCb0x9C()
{
  RES_R(0b00001000, _hl.h);
}

void Cpu::OpCb0x9D()
{
  RES_R(0b00001000, _hl.l);
}

void Cpu::OpCb0x9E()
{
  RES__HL(0b00001000);
}

void Cpu::OpCb0x9F()
{
  RES_R(0b00001000, _af.h);
}

void Cpu::OpCb0xA0()
{
  RES_R(0b00010000, _bc.h);
}

void Cpu::OpCb0xA1()
{
  RES_R(0b00010000, _bc.l);
}

void Cpu::OpCb0xA2()
{
  RES_R(0b00010000, _de.h);
}

void Cpu::OpCb0xA3()
{
  RES_R(0b00010000, _de.l);
}

void Cpu::OpCb0xA4()
{
  RES_R(0b00010000, _hl.h);
}

void Cpu::OpCb0xA5()
{
  RES_R(0b00010000, _hl.l);
}

void Cpu::OpCb0xA6()
{
  RES__HL(0b00010000);
}

void Cpu::OpCb0xA7()
{
  RES_R(0b00010000, _af.h);
}

void Cpu::OpCb0xA8()
{
  RES_R(0b00100000, _bc.h);
}

void Cpu::OpCb0xA9()
{
  RES_R(0b00100000, _bc.l);
}

void Cpu::OpCb0xAA()
{
  RES_R(0b00100000, _de.h);
}

void Cpu::OpCb0xAB()
{
  RES_R(0b00100000, _de.l);
}

void Cpu::OpCb0xAC()
{
  RES_R(0b00100000, _hl.h);
}

void Cpu::OpCb0xAD()
{
  RES_R(0b00100000, _hl.l);
}

void Cpu::OpCb0xAE()
{
  RES__HL(0b00100000);
}

void Cpu::OpCb0xAF()
{
  RES_R(0b00100000, _af.h);
}

void Cpu::OpCb0xB0()
{
  RES_R(0b01000000, _bc.h);
}

void Cpu::OpCb0xB1()
{
  RES_R(0b01000000, _bc.l);
}

void Cpu::OpCb0xB2()
{
  RES_R(0b01000000, _de.h);
}

void Cpu::OpCb0xB3()
{
  RES_R(0b01000000, _de.l);
}

void Cpu::OpCb0xB4()
{
  RES_R(0b01000000, _hl.h);
}

void Cpu::OpCb0xB5()
{
  RES_R(0b01000000, _hl.l);
}

void Cpu::OpCb0xB6()
{
  RES__HL(0b01000000);
}

void Cpu::OpCb0xB7()
{
  RES_R(0b01000000, _af.h);
}

void Cpu::OpCb0xB8()
{
  RES_R(0b10000000, _bc.h);
}

void Cpu::OpCb0xB9()
{
  RES_R(0b10000000, _bc.l);
}

void Cpu::OpCb0xBA()
{
  RES_R(0b10000000, _de.h);
}

void Cpu::OpCb0xBB()
{
  RES_R(0b10000000, _de.l);
}

void Cpu::OpCb0xBC()
{
  RES_R(0b10000000, _hl.h);
}

void Cpu::OpCb0xBD()
{
  RES_R(0b10000000, _hl.l);
}

void Cpu::OpCb0xBE()
{
  RES__HL(0b10000000);
}

void Cpu::OpCb0xBF()
{
  RES_R(0b10000000, _af.h);
}

void Cpu::OpCb0xC0()
{
  SET_R(0b00000001, _bc.h);
}

void Cpu::OpCb0xC1()
{
  SET_R(0b00000001, _bc.l);
}

void Cpu::OpCb0xC2()
{
  SET_R(0b00000001, _de.h);
}

void Cpu::OpCb0xC3()
{
  SET_R(0b00000001, _de.l);
}

void Cpu::OpCb0xC4()
{
  SET_R(0b00000001, _hl.h);
}

void Cpu::OpCb0xC5()
{
  SET_R(0b00000001, _hl.l);
}

void Cpu::OpCb0xC6()
{
  SET__HL(0b00000001);
}

void Cpu::OpCb0xC7()
{
  SET_R(0b00000001, _af.h);
}

void Cpu::OpCb0xC8()
{
  SET_R(0b00000010, _bc.h);
}

void Cpu::OpCb0xC9()
{
  SET_R(0b00000010, _bc.l);
}

void Cpu::OpCb0xCA()
{
  SET_R(0b00000010, _de.h);
}

void Cpu::OpCb0xCB()
{
  SET_R(0b00000010, _de.l);
}

void Cpu::OpCb0xCC()
{
  SET_R(0b00000010, _hl.h);
}

void Cpu::OpCb0xCD()
{
  SET_R(0b00000010, _hl.l);
}

void Cpu::OpCb0xCE()
{
  SET__HL(0b00000010);
}

void Cpu::OpCb0xCF()
{
  SET_R(0b00000010, _af.h);
}

void Cpu::OpCb0xD0()
{
  SET_R(0b00000100, _bc.h);
}

void Cpu::OpCb0xD1()
{
  SET_R(0b00000100, _bc.l);
}

void Cpu::OpCb0xD2()
{
  SET_R(0b00000100, _de.h);
}

void Cpu::OpCb0xD3()
{
  SET_R(0b00000100, _de.l);
}

void Cpu::OpCb0xD4()
{
  SET_R(0b00000100, _hl.h);
}

void Cpu::OpCb0xD5()
{
  SET_R(0b00000100, _hl.l);
}

void Cpu::OpCb0xD6()
{
  SET__HL(0b00000100);
}

void Cpu::OpCb0xD7()
{
  SET_R(0b00000100, _af.h);
}

void Cpu::OpCb0xD8()
{
  SET_R(0b00001000, _bc.h);
}

void Cpu::OpCb0xD9()
{
  SET_R(0b00001000, _bc.l);
}

void Cpu::OpCb0xDA()
{
  SET_R(0b00001000, _de.h);
}

void Cpu::OpCb0xDB()
{
  SET_R(0b00001000, _de.l);
}

void Cpu::OpCb0xDC()
{
  SET_R(0b00001000, _hl.h);
}

void Cpu::OpCb0xDD()
{
  SET_R(0b00001000, _hl.l);
}

void Cpu::OpCb0xDE()
{
  SET__HL(0b00001000);
}

void Cpu::OpCb0xDF()
{
  SET_R(0b00001000, _af.h);
}

void Cpu::OpCb0xE0()
{
  SET_R(0b00010000, _bc.h);
}

void Cpu::OpCb0xE1()
{
  SET_R(0b00010000, _bc.l);
}

void Cpu::OpCb0xE2()
{
  SET_R(0b00010000, _de.h);
}

void Cpu::OpCb0xE3()
{
  SET_R(0b00010000, _de.l);
}

void Cpu::OpCb0xE4()
{
  SET_R(0b00010000, _hl.h);
}

void Cpu::OpCb0xE5()
{
  SET_R(0b00010000, _hl.l);
}

void Cpu::OpCb0xE6()
{
  SET__HL(0b00010000);
}

void Cpu::OpCb0xE7()
{
  SET_R(0b00010000, _af.h);
}

void Cpu::OpCb0xE8()
{
  SET_R(0b00100000, _bc.h);
}

void Cpu::OpCb0xE9()
{
  SET_R(0b00100000, _bc.l);
}

void Cpu::OpCb0xEA()
{
  SET_R(0b00100000, _de.h);
}

void Cpu::OpCb0xEB()
{
  SET_R(0b00100000, _de.l);
}

void Cpu::OpCb0xEC()
{
  SET_R(0b00100000, _hl.h);
}

void Cpu::OpCb0xED()
{
  SET_R(0b00100000, _hl.l);
}

void Cpu::OpCb0xEE()
{
  SET__HL(0b00100000);
}

void Cpu::OpCb0xEF()
{
  SET_R(0b00100000, _af.h);
}

void Cpu::OpCb0xF0()
{
  SET_R(0b01000000, _bc.h);
}

void Cpu::OpCb0xF1()
{
  SET_R(0b01000000, _bc.l);
}

void Cpu::OpCb0xF2()
{
  SET_R(0b01000000, _de.h);
}

void Cpu::OpCb0xF3()
{
  SET_R(0b01000000, _de.l);
}

void Cpu::OpCb0xF4()
{
  SET_R(0b01000000, _hl.h);
}

void Cpu::OpCb0xF5()
{
  SET_R(0b01000000, _hl.l);
}

void Cpu::OpCb0xF6()
{
  SET__HL(0b01000000);
}

void Cpu::OpCb0xF7()
{
  SET_R(0b01000000, _af.h);
}

void Cpu::OpCb0xF8()
{
  SET_R(0b10000000, _bc.h);
}

void Cpu::OpCb0xF9()
{
  SET_R(0b10000000, _bc.l);
}

void Cpu::OpCb0xFA()
{
  SET_R(0b10000000, _de.h);
}

void Cpu::OpCb0xFB()
{
  SET_R(0b10000000, _de.l);
}

void Cpu::OpCb0xFC()
{
  SET_R(0b10000000, _hl.h);
}

void Cpu::OpCb0xFD()
{
  SET_R(0b10000000, _hl.l);
}

void Cpu::OpCb0xFE()
{
  SET__HL(0b10000000);
}

void Cpu::OpCb0xFF()
{
  SET_R(0b10000000, _af.h);
}
