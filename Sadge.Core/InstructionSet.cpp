#include "Cpu.h"

void Cpu::NOP()
{
  // Hi mom!
}

void Cpu::CB()
{
  this->mExecutionMode = ExecutionMode::EXT_INSTRUCTION;
  mExeCycle += 1;
  Fetch();
}

void Cpu::LD_rr_nn(Register& rr)
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ReadNextUint8();
      mWZ.H = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      rr = mWZ;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_r_n(uint8_t& r)
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      r = mWZ.L;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_r_x(uint8_t& r, uint8_t val)
{
  r = val;
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::LD_r__HL(uint8_t& r)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      r = mWZ.L;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD__HL_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = mHL.HL;
      mDataBus = mWZ.L;
      Write();
      mExeCycle += 1;
      break;
    case 2:
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::LD__rr_r(Register rr, uint8_t r)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = rr.HL;
      mDataBus = r;
      Write();
      mExeCycle += 1;
      break;
    case 1:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD__rr_A(Register rr)
{
  LD__rr_r(rr, _af.H);
}

void Cpu::LD__nn_A()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ReadNextUint8();
      mWZ.H = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      mAddressBus = mWZ.HL;
      mDataBus = _af.H;
      Write();
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD__HLx_A(int val)
{
  LD__rr_r(mHL, _af.H);

  switch (mExeCycle)
  {
    case 0:
      mHL.HL += val;
      break;
    case 1:
    default:
      break;
  }
}

void Cpu::LD_A__rr(Register rr)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = rr.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      _af.H = mWZ.L;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_A__nn()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ReadNextUint8();
      mWZ.H = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      mAddressBus = mWZ.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 3:
      _af.H = mWZ.L;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_A__HLx(int val)
{
  LD_A__rr(mHL);

  switch (mExeCycle)
  {
    case 0:
      mHL.HL += val;
      break;
    case 1:
    default:
      break;
  }
}

void Cpu::LD__nn_SP()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ReadNextUint8();
      mWZ.H = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      mAddressBus = mWZ.HL;
      mDataBus = mSP.L;
      Write();
      mWZ.HL += 1;
      mExeCycle += 1;
      break;
    case 3:
      mAddressBus = mWZ.HL;
      mDataBus = mSP.H;
      Write();
      mExeCycle += 1;
      break;
    case 4:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_SP_HL()
{
  switch (mExeCycle)
  {
    case 0:
      mSP = mHL;
      mExeCycle += 1;
      break;
    case 1:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_HL_SP_e()
{
  int8_t val;
  int result;
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = 0x0000;
      val = static_cast<int8_t>(mWZ.L);
      result = mSP.HL + val;
      ResetFlag(FlagBitMask::Zero);
      ResetFlag(FlagBitMask::Subtract);
      SetHalfCarryFlag(static_cast<uint8_t>(mSP.HL), static_cast<uint8_t>(val));
      SetCarry8Bit(static_cast<uint8_t>(mSP.HL) + static_cast<uint8_t>(val));
      mHL.HL = static_cast<uint16_t>(result);
      mExeCycle += 1;
      break;
    case 2:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_SP_e()
{
  int8_t val;
  int result;
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = 0x0000;
      val = static_cast<int8_t>(mWZ.L);
      result = mSP.HL + val;
      ResetFlag(FlagBitMask::Zero);
      ResetFlag(FlagBitMask::Subtract);
      SetHalfCarryFlag(static_cast<uint8_t>(mSP.HL), static_cast<uint8_t>(val));
      SetCarry8Bit(static_cast<uint8_t>(mSP.HL) + static_cast<uint8_t>(val));
      mWZ.HL = static_cast<uint16_t>(result);
      mExeCycle += 1;
      break;
    case 2:
      mExeCycle += 1;
      break;
    case 3:
      mSP = mWZ;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LDH__n_A()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = static_cast<uint16_t>(ExecutionAddress::IO) + mWZ.L;
      mDataBus = _af.H;
      Write();
      mExeCycle += 1;
      break;
    case 2:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LDH__C_A()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = static_cast<uint16_t>(ExecutionAddress::IO) + _bc.L;
      mDataBus = _af.H;
      Write();
      mExeCycle += 1;
      break;
    case 1:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LDH_A__C()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = static_cast<uint16_t>(ExecutionAddress::IO) + _bc.L;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      _af.H = mWZ.L;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LDH_A__n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = static_cast<uint16_t>(ExecutionAddress::IO) + mWZ.L;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      _af.H = mWZ.L;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_rr(Register& rr, int val)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = rr.HL;
      rr.HL += val;
      mExeCycle += 1;
      break;
    case 1:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_HL_rr(Register rr)
{
  int result{};

  switch (mExeCycle)
  {
    case 0:
      mAddressBus = 0x0000;
      mExeCycle += 1;
      break;
    case 1:
      result = mHL.HL + rr.HL;
      ResetFlag(FlagBitMask::Subtract);
      SetHalfCarryFlag(mHL.HL, rr.HL);
      SetCarry16Bit(result);
      mHL.HL = result;
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD__HL(int val)
{
  uint8_t result{};

  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      result = mWZ.L + val;
      mDataBus = result;
      Write();
      SetHalfCarryFlag(mWZ.L, val);
      SetZeroFlag(result);
      SetSubtractionFlag(val);
      mExeCycle += 1;
      break;
    case 2:
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_x(uint8_t& r, int val, bool set_carry)
{
  int result = r + val;
  SetHalfCarryFlag(r, val);
  r = result;
  SetZeroFlag(r);
  if (set_carry) SetCarry8Bit(result);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::ADC_A_x(int val)
{
  int carry = ReadFlag(FlagBitMask::Carry) ? 1 : 0;
  int result = _af.H + val + carry;
  SetHalfCarryFlag(_af.H, val, carry);
  _af.H = result;
  SetZeroFlag(_af.H);
  ResetFlag(FlagBitMask::Subtract);
  SetCarry8Bit(result);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::ADC_A__HL()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ADC_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADC_A_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ADC_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SBC_A_x(int val)
{
  int carry = ReadFlag(FlagBitMask::Carry) ? 1 : 0;
  int result = _af.H - val - carry;
  SetHalfCarryFlag(_af.H, -val, -carry);
  _af.H = result;
  SetZeroFlag(_af.H);
  SetFlag(FlagBitMask::Subtract);
  SetCarry8Bit(result);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::SBC_A__HL()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      SBC_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SBC_A_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      SBC_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::AND_A_x(uint8_t val)
{
  _af.H &= val;
  SetZeroFlag(_af.H);
  ResetFlag(FlagBitMask::Subtract);
  SetFlag(FlagBitMask::HalfCarry);
  ResetFlag(FlagBitMask::Carry);
}

void Cpu::AND_A__HL()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      AND_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::AND_A_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      AND_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::XOR_A_x(uint8_t val)
{
  _af.H ^= val;
  SetZeroFlag(_af.H);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  ResetFlag(FlagBitMask::Carry);
}

void Cpu::XOR_A__HL()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      XOR_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::XOR_A_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      XOR_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::OR_A_X(uint8_t val)
{
  _af.H |= val;
  SetZeroFlag(_af.H);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  ResetFlag(FlagBitMask::Carry);
}

void Cpu::OR_A__HL()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      OR_A_X(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::OR_A_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      OR_A_X(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::CP_A_x(uint8_t val)
{
  int result = _af.H - val;
  SetZeroFlag(result);
  SetFlag(FlagBitMask::Subtract);
  SetHalfCarryFlag(_af.H, -val);
  SetCarry8Bit(result);
}

void Cpu::CP_A__HL()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      CP_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::CP_A_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      CP_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_A_x(int val)
{
  ADD_x(_af.H, val, true);
  ResetFlag(FlagBitMask::Subtract);
}

void Cpu::ADD_A__HL()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ADD_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_A_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ADD_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SUB_A_x(int val)
{
  ADD_x(_af.H, -val, true);
  SetFlag(FlagBitMask::Subtract);
}

void Cpu::SUB_A__HL()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mHL.HL;
      Read();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      SUB_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SUB_A_n()
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      SUB_A_x(mWZ.L);
      mExeCycle = EXE_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::INC_r(uint8_t& r)
{
  ADD_x(r, 1);
  ResetFlag(FlagBitMask::Subtract);
}

void Cpu::DEC_r(uint8_t& r)
{
  ADD_x(r, -1);
  SetFlag(FlagBitMask::Subtract);
}

void Cpu::RLCA()
{
  _af.H & 0b10000000 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  _af.H = _af.H << 1 | _af.H >> 7;
  ResetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::RRCA()
{
  _af.H & 0b00000001 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  _af.H = _af.H >> 1 | _af.H << 7;
  ResetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::RLA()
{
  uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
  _af.H & 0b10000000 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  _af.H = _af.H << 1 | carry_bit;
  ResetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::RRA()
{
  uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
  _af.H & 0b00000001 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  _af.H = _af.H >> 1 | carry_bit << 7;
  ResetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::DAA()
{
  if (!ReadFlag(FlagBitMask::Subtract))
  {
    if (ReadFlag(FlagBitMask::Carry) || _af.H > 0x99) { _af.H += 0x60; SetFlag(FlagBitMask::Carry); }
    if (ReadFlag(FlagBitMask::HalfCarry) || (_af.H & 0x0f) > 0x09)   _af.H += 0x06;
  }
  else
  {
    if (ReadFlag(FlagBitMask::Carry))     _af.H -= 0x60;
    if (ReadFlag(FlagBitMask::HalfCarry)) _af.H -= 0x06;
  }

  SetZeroFlag(_af.H);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::CPL()
{
  _af.H = ~_af.H;
  SetFlag(FlagBitMask::Subtract);
  SetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::SCF()
{
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  SetFlag(FlagBitMask::Carry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::CCF()
{
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  ReadFlag(FlagBitMask::Carry) ? ResetFlag(FlagBitMask::Carry) : SetFlag(FlagBitMask::Carry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::PUSH_rr(Register rr)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mSP.HL;
      mSP.HL -= 1;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = mSP.HL;
      mDataBus = rr.H;
      Write();
      mSP.HL -= 1;
      mExeCycle += 1;
      break;
    case 2:
      mAddressBus = mSP.HL;
      mDataBus = rr.L;
      Write();
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::CALL(bool call)
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ReadNextUint8();
      mWZ.H = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      if (!call)
      {
        mExeCycle = EXE_COMPLETE;
        break;
      }
      mAddressBus = mSP.HL;
      mSP.HL -= 1;
      mExeCycle += 1;
      break;
    case 3:
      mAddressBus = mSP.HL;
      mDataBus = mPC.H;
      Write();
      mSP.HL -= 1;
      mExeCycle += 1;
      break;
    case 4:
      mAddressBus = mSP.HL;
      mDataBus = mPC.L;
      Write();
      mPC = mWZ;
      mExeCycle += 1;
      break;
    case 5:
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::JP_HL()
{
  mPC = mHL;
}

void Cpu::JP_cc_nn(bool jump)
{
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      ReadNextUint8();
      mWZ.H = mDataBus;
      mExeCycle += 1;
      break;
    case 2:
      mAddressBus = 0x0000;
      if (jump) mPC = mWZ;
      mExeCycle = jump ? mExeCycle + 1 : EXE_COMPLETE;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::JR_cc_e(bool jump)
{
  int8_t val;
  switch (mExeCycle)
  {
    case 0:
      ReadNextUint8();
      mWZ.L = mDataBus;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = mPC.H;
      mExeCycle = jump ? mExeCycle + 1 : EXE_COMPLETE;
      break;
    case 2:
      val = static_cast<int8_t>(mWZ.L);
      mPC.HL += val;
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::POP_rr(Register& rr)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mSP.HL;
      Read();
      mWZ.L = mDataBus;
      mSP.HL += 1;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = mSP.HL;
      Read();
      mWZ.H = mDataBus;
      mSP.HL += 1;
      mExeCycle += 1;
      break;
    case 2:
      rr = mWZ;
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::RET(bool set_ime)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mSP.HL;
      Read();
      mWZ.L = mDataBus;
      mSP.HL += 1;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = mSP.HL;
      Read();
      mWZ.H = mDataBus;
      mSP.HL += 1;
      mExeCycle += 1;
      break;
    case 2:
      mPC = mWZ;
      if (set_ime) mInterruptCtrl.EnableInterrupts();
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::RET_cc(bool ret)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = 0x0000;
      mExeCycle += 1;
      break;
    case 1:
      if (!ret)
      {
        mExeCycle = EXE_COMPLETE;
        break;
      }
      mAddressBus = mSP.HL;
      Read();
      mWZ.L = mDataBus;
      mSP.HL += 1;
      mExeCycle += 1;
      break;
    case 2:
      mAddressBus = mSP.HL;
      Read();
      mWZ.H = mDataBus;
      mSP.HL += 1;
      mExeCycle += 1;
      break;
    case 3:
      mPC = mWZ;
      mExeCycle += 1;
      break;
    case 4:
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::POP_AF()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mSP.HL;
      Read();
      mWZ.L = mDataBus;
      mSP.HL += 1;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = mSP.HL;
      Read();
      mWZ.H = mDataBus;
      mSP.HL += 1;
      mExeCycle += 1;
      break;
    case 2:
      _af = mWZ;
      _af.L &= 0xF0;
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::RST(RestartVector vec)
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mSP.HL;
      mSP.HL -= 1;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = mSP.HL;
      mDataBus = mPC.H;
      Write();
      mSP.HL -= 1;
      mExeCycle += 1;
      break;
    case 2:
      mAddressBus = mSP.HL;
      mDataBus = mPC.L;
      Write();
      mPC.HL = GetRestartVectorAddress(vec);
      mExeCycle += 1;
      break;
    case 3:
      mExeCycle = EXE_COMPLETE;
      break;
  }
}

void Cpu::Op0x00()
{
  NOP();
}

void Cpu::Op0x01()
{
  LD_rr_nn(_bc);
}

void Cpu::Op0x02()
{
  LD__rr_A(_bc);
}

void Cpu::Op0x03()
{
  ADD_rr(_bc, 1);
}

void Cpu::Op0x04()
{
  INC_r(_bc.H);
}

void Cpu::Op0x05()
{
  DEC_r(_bc.H);
}

void Cpu::Op0x06()
{
  LD_r_n(_bc.H);
}

void Cpu::Op0x07()
{
  RLCA();
}

void Cpu::Op0x08()
{
  LD__nn_SP();
}

void Cpu::Op0x09()
{
  ADD_HL_rr(_bc);
}

void Cpu::Op0x0A()
{
  LD_A__rr(_bc);
}

void Cpu::Op0x0B()
{
  ADD_rr(_bc, -1);;
}

void Cpu::Op0x0C()
{
  INC_r(_bc.L);
}

void Cpu::Op0x0D()
{
  DEC_r(_bc.L);
}

void Cpu::Op0x0E()
{
  LD_r_n(_bc.L);
}

void Cpu::Op0x0F()
{
  RRCA();
}

void Cpu::Op0x10()
{
  this->mExecutionMode = ExecutionMode::STOP;
}

void Cpu::Op0x11()
{
  LD_rr_nn(_de);
}

void Cpu::Op0x12()
{
  LD__rr_A(_de);
}

void Cpu::Op0x13()
{
  ADD_rr(_de, 1);
}

void Cpu::Op0x14()
{
  INC_r(_de.H);
}

void Cpu::Op0x15()
{
  DEC_r(_de.H);
}

void Cpu::Op0x16()
{
  LD_r_n(_de.H);
}

void Cpu::Op0x17()
{
  RLA();
}

void Cpu::Op0x18()
{
  JR_cc_e(true);
}

void Cpu::Op0x19()
{
  ADD_HL_rr(_de);
}

void Cpu::Op0x20()
{
  JR_cc_e(!ReadFlag(FlagBitMask::Zero));
}

void Cpu::Op0x1A()
{
  LD_A__rr(_de);
}

void Cpu::Op0x1B()
{
  ADD_rr(_de, -1);
}

void Cpu::Op0x1C()
{
  INC_r(_de.L);
}

void Cpu::Op0x1D()
{
  DEC_r(_de.L);
}

void Cpu::Op0x1E()
{
  LD_r_n(_de.L);
}

void Cpu::Op0x1F()
{
  RRA();
}

void Cpu::Op0x21()
{
  LD_rr_nn(mHL);
}

void Cpu::Op0x22()
{
  LD__HLx_A(1);
}

void Cpu::Op0x23()
{
  ADD_rr(mHL, 1);
}

void Cpu::Op0x24()
{
  INC_r(mHL.H);
}

void Cpu::Op0x25()
{
  DEC_r(mHL.H);
}

void Cpu::Op0x26()
{
  LD_r_n(mHL.H);
}

void Cpu::Op0x27()
{
  DAA();
}

void Cpu::Op0x28()
{
  JR_cc_e(ReadFlag(FlagBitMask::Zero));
}

void Cpu::Op0x29()
{
  ADD_HL_rr(mHL);
}

void Cpu::Op0x2A()
{
  LD_A__HLx(1);
}

void Cpu::Op0x2B()
{
  ADD_rr(mHL, -1);
}

void Cpu::Op0x2C()
{
  INC_r(mHL.L);
}

void Cpu::Op0x2D()
{
  DEC_r(mHL.L);
}

void Cpu::Op0x2E()
{
  LD_r_n(mHL.L);
}

void Cpu::Op0x2F()
{
  CPL();
}

void Cpu::Op0x30()
{
  JR_cc_e(!ReadFlag(FlagBitMask::Carry));
}

void Cpu::Op0x31()
{
  LD_rr_nn(mSP);
}

void Cpu::Op0x32()
{
  LD__HLx_A(-1);
}

void Cpu::Op0x33()
{
  ADD_rr(mSP, 1);
}

void Cpu::Op0x34()
{
  ADD__HL(1);
}

void Cpu::Op0x35()
{
  ADD__HL(-1);
}

void Cpu::Op0x36()
{
  LD__HL_n();
}

void Cpu::Op0x37()
{
  SCF();
}

void Cpu::Op0x38()
{
  JR_cc_e(ReadFlag(FlagBitMask::Carry));
}

void Cpu::Op0x39()
{
  ADD_HL_rr(mSP);
}

void Cpu::Op0x3A()
{
  LD_A__HLx(-1);
}

void Cpu::Op0x3B()
{
  ADD_rr(mSP, -1);
}

void Cpu::Op0x3C()
{
  INC_r(_af.H);
}

void Cpu::Op0x3D()
{
  DEC_r(_af.H);
}

void Cpu::Op0x3E()
{
  LD_r_n(_af.H);
}

void Cpu::Op0x3F()
{
  CCF();
}

void Cpu::Op0x40()
{
  LD_r_x(_bc.H, _bc.H);
}

void Cpu::Op0x41()
{
  LD_r_x(_bc.H, _bc.L);
}

void Cpu::Op0x42()
{
  LD_r_x(_bc.H, _de.H);
}

void Cpu::Op0x43()
{
  LD_r_x(_bc.H, _de.L);
}

void Cpu::Op0x44()
{
  LD_r_x(_bc.H, mHL.H);
}

void Cpu::Op0x45()
{
  LD_r_x(_bc.H, mHL.L);
}

void Cpu::Op0x46()
{
  LD_r__HL(_bc.H);
}

void Cpu::Op0x47()
{
  LD_r_x(_bc.H, _af.H);

}

void Cpu::Op0x48()
{
  LD_r_x(_bc.L, _bc.H);

}

void Cpu::Op0x49()
{
  LD_r_x(_bc.L, _bc.L);

}

void Cpu::Op0x4A()
{
  LD_r_x(_bc.L, _de.H);

}

void Cpu::Op0x4B()
{
  LD_r_x(_bc.L, _de.L);
}

void Cpu::Op0x4C()
{
  LD_r_x(_bc.L, mHL.H);
}

void Cpu::Op0x4D()
{
  LD_r_x(_bc.L, mHL.L);
}

void Cpu::Op0x4E()
{
  LD_r__HL(_bc.L);
}

void Cpu::Op0x4F()
{
  LD_r_x(_bc.L, _af.H);
}

void Cpu::Op0x50()
{
  LD_r_x(_de.H, _bc.H);
}

void Cpu::Op0x51()
{
  LD_r_x(_de.H, _bc.L);
}

void Cpu::Op0x52()
{
  LD_r_x(_de.H, _de.H);
}

void Cpu::Op0x53()
{
  LD_r_x(_de.H, _de.L);
}

void Cpu::Op0x54()
{
  LD_r_x(_de.H, mHL.H);
}

void Cpu::Op0x55()
{
  LD_r_x(_de.H, mHL.L);
}

void Cpu::Op0x56()
{
  LD_r__HL(_de.H);
}

void Cpu::Op0x57()
{
  LD_r_x(_de.H, _af.H);
}

void Cpu::Op0x58()
{
  LD_r_x(_de.L, _bc.H);
}

void Cpu::Op0x59()
{
  LD_r_x(_de.L, _bc.L);
}

void Cpu::Op0x5A()
{
  LD_r_x(_de.L, _de.H);
}

void Cpu::Op0x5B()
{
  LD_r_x(_de.L, _de.L);
}

void Cpu::Op0x5C()
{
  LD_r_x(_de.L, mHL.H);

}

void Cpu::Op0x5D()
{
  LD_r_x(_de.L, mHL.L);
}

void Cpu::Op0x5E()
{
  LD_r__HL(_de.L);
}

void Cpu::Op0x5F()
{
  LD_r_x(_de.L, _af.H);
}

void Cpu::Op0x60()
{
  LD_r_x(mHL.H, _bc.H);
}

void Cpu::Op0x61()
{
  LD_r_x(mHL.H, _bc.L);
}

void Cpu::Op0x62()
{
  LD_r_x(mHL.H, _de.H);
}

void Cpu::Op0x63()
{
  LD_r_x(mHL.H, _de.L);
}

void Cpu::Op0x64()
{
  LD_r_x(mHL.H, mHL.H);
}

void Cpu::Op0x65()
{
  LD_r_x(mHL.H, mHL.L);
}

void Cpu::Op0x66()
{
  LD_r__HL(mHL.H);
}

void Cpu::Op0x67()
{
  LD_r_x(mHL.H, _af.H);
}

void Cpu::Op0x68()
{
  LD_r_x(mHL.L, _bc.H);
}

void Cpu::Op0x69()
{
  LD_r_x(mHL.L, _bc.L);
}

void Cpu::Op0x6A()
{
  LD_r_x(mHL.L, _de.H);
}

void Cpu::Op0x6B()
{
  LD_r_x(mHL.L, _de.L);
}

void Cpu::Op0x6C()
{
  LD_r_x(mHL.L, mHL.H);
}

void Cpu::Op0x6D()
{
  LD_r_x(mHL.L, mHL.L);
}

void Cpu::Op0x6E()
{
  LD_r__HL(mHL.L);
}

void Cpu::Op0x6F()
{
  LD_r_x(mHL.L, _af.H);
}

void Cpu::Op0x70()
{
  LD__rr_r(mHL, _bc.H);
}

void Cpu::Op0x71()
{
  LD__rr_r(mHL, _bc.L);
}

void Cpu::Op0x72()
{
  LD__rr_r(mHL, _de.H);
}

void Cpu::Op0x73()
{
  LD__rr_r(mHL, _de.L);
}

void Cpu::Op0x74()
{
  LD__rr_r(mHL, mHL.H);
}

void Cpu::Op0x75()
{
  LD__rr_r(mHL, mHL.L);
}

void Cpu::Op0x76()
{
  this->mExecutionMode = ExecutionMode::HALT;
}

void Cpu::Op0x77()
{
  LD__rr_r(mHL, _af.H);
}

void Cpu::Op0x78()
{
  LD_r_x(_af.H, _bc.H);
}

void Cpu::Op0x79()
{
  LD_r_x(_af.H, _bc.L);
}

void Cpu::Op0x7A()
{
  LD_r_x(_af.H, _de.H);
}

void Cpu::Op0x7B()
{
  LD_r_x(_af.H, _de.L);
}

void Cpu::Op0x7C()
{
  LD_r_x(_af.H, mHL.H);
}

void Cpu::Op0x7D()
{
  LD_r_x(_af.H, mHL.L);
}

void Cpu::Op0x7E()
{
  LD_r__HL(_af.H);
}

void Cpu::Op0x7F()
{
  LD_r_x(_af.H, _af.H);
}

void Cpu::Op0x80()
{
  ADD_A_x(_bc.H);
}

void Cpu::Op0x81()
{
  ADD_A_x(_bc.L);
}

void Cpu::Op0x82()
{
  ADD_A_x(_de.H);
}

void Cpu::Op0x83()
{
  ADD_A_x(_de.L);
}

void Cpu::Op0x84()
{
  ADD_A_x(mHL.H);
}

void Cpu::Op0x85()
{
  ADD_A_x(mHL.L);
}

void Cpu::Op0x86()
{
  ADD_A__HL();
}

void Cpu::Op0x87()
{
  ADD_A_x(_af.H);
}

void Cpu::Op0x88()
{
  ADC_A_x(_bc.H);
}

void Cpu::Op0x89()
{
  ADC_A_x(_bc.L);
}

void Cpu::Op0x8A()
{
  ADC_A_x(_de.H);
}

void Cpu::Op0x8B()
{
  ADC_A_x(_de.L);
}

void Cpu::Op0x8C()
{
  ADC_A_x(mHL.H);
}

void Cpu::Op0x8D()
{
  ADC_A_x(mHL.L);
}

void Cpu::Op0x8E()
{
  ADC_A__HL();
}

void Cpu::Op0x8F()
{
  ADC_A_x(_af.H);
}

void Cpu::Op0x90()
{
  SUB_A_x(_bc.H);
}

void Cpu::Op0x91()
{
  SUB_A_x(_bc.L);
}

void Cpu::Op0x92()
{
  SUB_A_x(_de.H);
}

void Cpu::Op0x93()
{
  SUB_A_x(_de.L);
}

void Cpu::Op0x94()
{
  SUB_A_x(mHL.H);
}

void Cpu::Op0x95()
{
  SUB_A_x(mHL.L);
}

void Cpu::Op0x96()
{
  SUB_A__HL();
}

void Cpu::Op0x97()
{
  SUB_A_x(_af.H);
}

void Cpu::Op0x98()
{
  SBC_A_x(_bc.H);
}

void Cpu::Op0x99()
{
  SBC_A_x(_bc.L);
}

void Cpu::Op0x9A()
{
  SBC_A_x(_de.H);
}

void Cpu::Op0x9B()
{
  SBC_A_x(_de.L);
}

void Cpu::Op0x9C()
{
  SBC_A_x(mHL.H);
}

void Cpu::Op0x9D()
{
  SBC_A_x(mHL.L);
}

void Cpu::Op0x9E()
{
  SBC_A__HL();
}

void Cpu::Op0x9F()
{
  SBC_A_x(_af.H);
}

void Cpu::Op0xA0()
{
  AND_A_x(_bc.H);
}

void Cpu::Op0xA1()
{
  AND_A_x(_bc.L);
}

void Cpu::Op0xA2()
{
  AND_A_x(_de.H);
}

void Cpu::Op0xA3()
{
  AND_A_x(_de.L);
}

void Cpu::Op0xA4()
{
  AND_A_x(mHL.H);
}

void Cpu::Op0xA5()
{
  AND_A_x(mHL.L);
}

void Cpu::Op0xA6()
{
  AND_A__HL();
}

void Cpu::Op0xA7()
{
  AND_A_x(_af.H);
}

void Cpu::Op0xA8()
{
  XOR_A_x(_bc.H);
}

void Cpu::Op0xA9()
{
  XOR_A_x(_bc.L);
}

void Cpu::Op0xAA()
{
  XOR_A_x(_de.H);
}

void Cpu::Op0xAB()
{
  XOR_A_x(_de.L);
}

void Cpu::Op0xAC()
{
  XOR_A_x(mHL.H);
}

void Cpu::Op0xAD()
{
  XOR_A_x(mHL.L);
}

void Cpu::Op0xAE()
{
  XOR_A__HL();
}

void Cpu::Op0xAF()
{
  XOR_A_x(_af.H);
}

void Cpu::Op0xB0()
{
  OR_A_X(_bc.H);
}

void Cpu::Op0xB1()
{
  OR_A_X(_bc.L);
}

void Cpu::Op0xB2()
{
  OR_A_X(_de.H);
}

void Cpu::Op0xB3()
{
  OR_A_X(_de.L);
}

void Cpu::Op0xB4()
{
  OR_A_X(mHL.H);
}

void Cpu::Op0xB5()
{
  OR_A_X(mHL.L);
}

void Cpu::Op0xB6()
{
  OR_A__HL();
}

void Cpu::Op0xB7()
{
  OR_A_X(_af.H);
}

void Cpu::Op0xB8()
{
  CP_A_x(_bc.H);
}

void Cpu::Op0xB9()
{
  CP_A_x(_bc.L);
}

void Cpu::Op0xBA()
{
  CP_A_x(_de.H);
}

void Cpu::Op0xBB()
{
  CP_A_x(_de.L);
}

void Cpu::Op0xBC()
{
  CP_A_x(mHL.H);
}

void Cpu::Op0xBD()
{
  CP_A_x(mHL.L);
}

void Cpu::Op0xBE()
{
  CP_A__HL();
}

void Cpu::Op0xBF()
{
  CP_A_x(_af.H);
}

void Cpu::Op0xC0()
{
  RET_cc(!ReadFlag(FlagBitMask::Zero));
}

void Cpu::Op0xC1()
{
  POP_rr(_bc);
}

void Cpu::Op0xC2()
{
  JP_cc_nn(!ReadFlag(FlagBitMask::Zero));
}

void Cpu::Op0xC3()
{
  JP_cc_nn(true);
}

void Cpu::Op0xC4()
{
  CALL(!ReadFlag(FlagBitMask::Zero));
}

void Cpu::Op0xC5()
{
  PUSH_rr(_bc);
}

void Cpu::Op0xC6()
{
  ADD_A_n();
}

void Cpu::Op0xC7()
{
  RST(RestartVector::RST0);
}

void Cpu::Op0xC8()
{
  RET_cc(ReadFlag(FlagBitMask::Zero));
}

void Cpu::Op0xC9()
{
  RET();
}

void Cpu::Op0xCA()
{
  JP_cc_nn(ReadFlag(FlagBitMask::Zero));
}

void Cpu::Op0xCB()
{
  CB();
}

void Cpu::Op0xCC()
{
  CALL(ReadFlag(FlagBitMask::Zero));
}

void Cpu::Op0xCD()
{
  CALL(true);
}

void Cpu::Op0xCE()
{
  ADC_A_n();
}

void Cpu::Op0xCF()
{
  RST(RestartVector::RST1);
}

void Cpu::Op0xD0()
{
  RET_cc(!ReadFlag(FlagBitMask::Carry));
}

void Cpu::Op0xD1()
{
  POP_rr(_de);
}

void Cpu::Op0xD2()
{
  JP_cc_nn(!ReadFlag(FlagBitMask::Carry));
}

void Cpu::Op0xD3()
{
  throw std::exception("Invalid Instruction: 0xD3");
}

void Cpu::Op0xD4()
{
  CALL(!ReadFlag(FlagBitMask::Carry));
}

void Cpu::Op0xD5()
{
  PUSH_rr(_de);
}

void Cpu::Op0xD6()
{
  SUB_A_n();
}

void Cpu::Op0xD7()
{
  RST(RestartVector::RST2);
}

void Cpu::Op0xD8()
{
  RET_cc(ReadFlag(FlagBitMask::Carry));
}

void Cpu::Op0xD9()
{
  RET(true);
}

void Cpu::Op0xDA()
{
  JP_cc_nn(ReadFlag(FlagBitMask::Carry));
}

void Cpu::Op0xDB()
{
  throw std::exception("Invalid Instruction: 0xDB");
}

void Cpu::Op0xDC()
{
  CALL(ReadFlag(FlagBitMask::Carry));
}

void Cpu::Op0xDD()
{
  throw std::exception("Invalid Instruction: 0xDD");
}

void Cpu::Op0xDE()
{
  SBC_A_n();
}

void Cpu::Op0xDF()
{
  RST(RestartVector::RST3);
}

void Cpu::Op0xE0()
{
  LDH__n_A();
}

void Cpu::Op0xE1()
{
  POP_rr(mHL);
}

void Cpu::Op0xE2()
{
  LDH__C_A();
}

void Cpu::Op0xE3()
{
  throw std::exception("Invalid Instruction: 0xE3");
}

void Cpu::Op0xE4()
{
  throw std::exception("Invalid Instruction: 0xE4");
}

void Cpu::Op0xE5()
{
  PUSH_rr(mHL);
}

void Cpu::Op0xE6()
{
  AND_A_n();
}

void Cpu::Op0xE7()
{
  RST(RestartVector::RST4);
}

void Cpu::Op0xE8()
{
  ADD_SP_e();
}

void Cpu::Op0xE9()
{
  JP_HL();
}

void Cpu::Op0xEA()
{
  LD__nn_A();
}

void Cpu::Op0xEB()
{
  throw std::exception("Invalid Instruction: 0xEB");
}

void Cpu::Op0xEC()
{
  throw std::exception("Invalid Instruction: 0xEC");
}

void Cpu::Op0xED()
{
  throw std::exception("Invalid Instruction: 0xED");
}

void Cpu::Op0xEE()
{
  XOR_A_n();
}

void Cpu::Op0xEF()
{
  RST(RestartVector::RST5);
}

void Cpu::Op0xF0()
{
  LDH_A__n();
}

void Cpu::Op0xF1()
{
  POP_AF();
}

void Cpu::Op0xF2()
{
  LDH_A__C();
}

void Cpu::Op0xF3()
{
  mInterruptCtrl.DisableInterrupts();
}

void Cpu::Op0xF4()
{
  throw std::exception("Invalid Instruction: 0xF4");
}

void Cpu::Op0xF5()
{
  PUSH_rr(_af);
}

void Cpu::Op0xF6()
{
  OR_A_n();
}

void Cpu::Op0xF7()
{
  RST(RestartVector::RST6);
}

void Cpu::Op0xF8()
{
  LD_HL_SP_e();
}

void Cpu::Op0xF9()
{
  LD_SP_HL();
}

void Cpu::Op0xFA()
{
  LD_A__nn();
}

void Cpu::Op0xFB()
{
  mImeRequest = true;
}

void Cpu::Op0xFC()
{
  throw std::exception("Invalid Instruction: 0xFC");
}

void Cpu::Op0xFD()
{
  throw std::exception("Invalid Instruction: 0xFD");
}

void Cpu::Op0xFE()
{
  CP_A_n();
}

void Cpu::Op0xFF()
{
  RST(RestartVector::RST7);
}
