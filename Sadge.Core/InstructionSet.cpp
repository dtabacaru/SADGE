#include "Cpu.h"

void Cpu::NOP()
{
  // Hi mom!
}

void Cpu::CB()
{
  _cb_mode = true;
  _opCycle += 1;
  Fetch();
}

void Cpu::LD_rr_nn(Register& rr)
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _dataBus = ReadNextUint8();
      _wz.h = _dataBus;
      _opCycle += 1;
      break;
    case 2:
      rr = _wz;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_r_n(uint8_t& r)
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      r = _wz.l;
      _opCycle = INSTRUCTION_COMPLETE;
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
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      r = _wz.l;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD__HL_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = _hl.hl;
      _dataBus = _wz.l;
      WriteAddress(_addressBus, _dataBus);
      _opCycle += 1;
      break;
    case 2:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::LD__rr_r(Register rr, uint8_t r)
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = rr.hl;
      _dataBus = r;
      WriteAddress(_addressBus, _dataBus);
      _opCycle += 1;
      break;
    case 1:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD__rr_A(Register rr)
{
  LD__rr_r(rr, _af.h);
}

void Cpu::LD__nn_A()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _dataBus = ReadNextUint8();
      _wz.h = _dataBus;
      _opCycle += 1;
      break;
    case 2:
      _addressBus = _wz.hl;
      _dataBus = _af.h;
      WriteAddress(_addressBus, _dataBus);
      _opCycle += 1;
      break;
    case 3:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD__HLx_A(int val)
{
  LD__rr_r(_hl, _af.h);

  switch (_opCycle)
  {
    case 0:
      _hl.hl += val;
      break;
    case 1:
    default:
      break;
  }
}

void Cpu::LD_A__rr(Register rr)
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = rr.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _af.h = _wz.l;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_A__nn()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _dataBus = ReadNextUint8();
      _wz.h = _dataBus;
      _opCycle += 1;
      break;
    case 2:
      _addressBus = _wz.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 3:
      _af.h = _wz.l;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_A__HLx(int val)
{
  LD_A__rr(_hl);

  switch (_opCycle)
  {
    case 0:
      _hl.hl += val;
      break;
    case 1:
    default:
      break;
  }
}

void Cpu::LD__nn_SP()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _dataBus = ReadNextUint8();
      _wz.h = _dataBus;
      _opCycle += 1;
      break;
    case 2:
      _addressBus = _wz.hl;
      _dataBus = _sp.l;
      WriteAddress(_addressBus, _dataBus);
      _wz.hl += 1;
      _opCycle += 1;
      break;
    case 3:
      _addressBus = _wz.hl;
      _dataBus = _sp.h;
      WriteAddress(_addressBus, _dataBus);
      _opCycle += 1;
      break;
    case 4:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_SP_HL()
{
  switch (_opCycle)
  {
    case 0:
      _sp = _hl;
      _opCycle += 1;
      break;
    case 1:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_HL_SP_e()
{
  int8_t val;
  int result;
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = 0x0000;
      val = static_cast<int8_t>(_wz.l);
      result = _sp.hl + val;
      ResetFlag(FlagBitMask::Zero);
      ResetFlag(FlagBitMask::Subtract);
      SetHalfCarryFlag(static_cast<uint8_t>(_sp.hl), static_cast<uint8_t>(val));
      SetCarry8Bit(static_cast<uint8_t>(_sp.hl) + static_cast<uint8_t>(val));
      _hl.hl = static_cast<uint16_t>(result);
      _opCycle += 1;
      break;
    case 2:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_SP_e()
{
  int8_t val;
  int result;
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = 0x0000;
      val = static_cast<int8_t>(_wz.l);
      result = _sp.hl + val;
      ResetFlag(FlagBitMask::Zero);
      ResetFlag(FlagBitMask::Subtract);
      SetHalfCarryFlag(static_cast<uint8_t>(_sp.hl), static_cast<uint8_t>(val));
      SetCarry8Bit(static_cast<uint8_t>(_sp.hl) + static_cast<uint8_t>(val));
      _wz.hl = static_cast<uint16_t>(result);
      _opCycle += 1;
      break;
    case 2:
      _opCycle += 1;
      break;
    case 3:
      _sp = _wz;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LDH__n_A()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = static_cast<uint16_t>(ExecutionAddress::IO) + _wz.l;
      _dataBus = _af.h;
      WriteAddress(_addressBus, _dataBus);
      _opCycle += 1;
      break;
    case 2:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LDH__C_A()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = static_cast<uint16_t>(ExecutionAddress::IO) + _bc.l;
      _dataBus = _af.h;
      WriteAddress(_addressBus, _dataBus);
      _opCycle += 1;
      break;
    case 1:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LDH_A__C()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = static_cast<uint16_t>(ExecutionAddress::IO) + _bc.l;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _af.h = _wz.l;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LDH_A__n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = static_cast<uint16_t>(ExecutionAddress::IO) + _wz.l;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 2:
      _af.h = _wz.l;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_rr(Register& rr, int val)
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = rr.hl;
      rr.hl += val;
      _opCycle += 1;
      break;
    case 1:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_HL_rr(Register rr)
{
  int result{};

  switch (_opCycle)
  {
    case 0:
      _addressBus = 0x0000;
      _opCycle += 1;
      break;
    case 1:
      result = _hl.hl + rr.hl;
      ResetFlag(FlagBitMask::Subtract);
      SetHalfCarryFlag(_hl.hl, rr.hl);
      SetCarry16Bit(result);
      _hl.hl = result;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD__HL(int val)
{
  uint8_t result{};

  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      result = _wz.l + val;
      _dataBus = result;
      WriteAddress(_addressBus, _dataBus);
      SetHalfCarryFlag(_wz.l, val);
      SetZeroFlag(result);
      SetSubtractionFlag(val);
      _opCycle += 1;
      break;
    case 2:
      _opCycle = INSTRUCTION_COMPLETE;
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
  int result = _af.h + val + carry;
  SetHalfCarryFlag(_af.h, val, carry);
  _af.h = result;
  SetZeroFlag(_af.h);
  ResetFlag(FlagBitMask::Subtract);
  SetCarry8Bit(result);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::ADC_A__HL()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      ADC_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADC_A_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      ADC_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SBC_A_x(int val)
{
  int carry = ReadFlag(FlagBitMask::Carry) ? 1 : 0;
  int result = _af.h - val - carry;
  SetHalfCarryFlag(_af.h, -val, -carry);
  _af.h = result;
  SetZeroFlag(_af.h);
  SetFlag(FlagBitMask::Subtract);
  SetCarry8Bit(result);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::SBC_A__HL()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      SBC_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SBC_A_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      SBC_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::AND_A_x(uint8_t val)
{
  _af.h &= val;
  SetZeroFlag(_af.h);
  ResetFlag(FlagBitMask::Subtract);
  SetFlag(FlagBitMask::HalfCarry);
  ResetFlag(FlagBitMask::Carry);
}

void Cpu::AND_A__HL()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      AND_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::AND_A_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      AND_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::XOR_A_x(uint8_t val)
{
  _af.h ^= val;
  SetZeroFlag(_af.h);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  ResetFlag(FlagBitMask::Carry);
}

void Cpu::XOR_A__HL()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      XOR_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::XOR_A_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      XOR_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::OR_A_X(uint8_t val)
{
  _af.h |= val;
  SetZeroFlag(_af.h);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  ResetFlag(FlagBitMask::Carry);
}

void Cpu::OR_A__HL()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      OR_A_X(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::OR_A_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      OR_A_X(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::CP_A_x(uint8_t val)
{
  int result = _af.h - val;
  SetZeroFlag(result);
  SetFlag(FlagBitMask::Subtract);
  SetHalfCarryFlag(_af.h, -val);
  SetCarry8Bit(result);
}

void Cpu::CP_A__HL()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      CP_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::CP_A_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      CP_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_A_x(int val)
{
  ADD_x(_af.h, val, true);
  ResetFlag(FlagBitMask::Subtract);
}

void Cpu::ADD_A__HL()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      ADD_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_A_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      ADD_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SUB_A_x(int val)
{
  ADD_x(_af.h, -val, true);
  SetFlag(FlagBitMask::Subtract);
}

void Cpu::SUB_A__HL()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      SUB_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::SUB_A_n()
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      SUB_A_x(_wz.l);
      _opCycle = INSTRUCTION_COMPLETE;
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
  _af.h & 0b10000000 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  _af.h = _af.h << 1 | _af.h >> 7;
  ResetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::RRCA()
{
  _af.h & 0b00000001 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  _af.h = _af.h >> 1 | _af.h << 7;
  ResetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::RLA()
{
  uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
  _af.h & 0b10000000 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  _af.h = _af.h << 1 | carry_bit;
  ResetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::RRA()
{
  uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
  _af.h & 0b00000001 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  _af.h = _af.h >> 1 | carry_bit << 7;
  ResetFlag(FlagBitMask::Zero);
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::DAA()
{
  if (!ReadFlag(FlagBitMask::Subtract))
  {
    if (ReadFlag(FlagBitMask::Carry) || _af.h > 0x99) { _af.h += 0x60; SetFlag(FlagBitMask::Carry); }
    if (ReadFlag(FlagBitMask::HalfCarry) || (_af.h & 0x0f) > 0x09)   _af.h += 0x06;
  }
  else
  {
    if (ReadFlag(FlagBitMask::Carry))     _af.h -= 0x60;
    if (ReadFlag(FlagBitMask::HalfCarry)) _af.h -= 0x06;
  }

  SetZeroFlag(_af.h);
  ResetFlag(FlagBitMask::HalfCarry);
  //_opcycle = INSTRUCTION_COMPLETE;
}

void Cpu::CPL()
{
  _af.h = ~_af.h;
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
  switch (_opCycle)
  {
    case 0:
      _addressBus = _sp.hl;
      _sp.hl -= 1;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = _sp.hl;
      _dataBus = rr.h;
      WriteAddress(_addressBus, _dataBus);
      _sp.hl -= 1;
      _opCycle += 1;
      break;
    case 2:
      _addressBus = _sp.hl;
      _dataBus = rr.l;
      WriteAddress(_addressBus, _dataBus);
      _opCycle += 1;
      break;
    case 3:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::CALL(bool call)
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _dataBus = ReadNextUint8();
      _wz.h = _dataBus;
      _opCycle += 1;
      break;
    case 2:
      if (!call)
      {
        _opCycle = INSTRUCTION_COMPLETE;
        break;
      }
      _addressBus = _sp.hl;
      _sp.hl -= 1;
      _opCycle += 1;
      break;
    case 3:
      _addressBus = _sp.hl;
      _dataBus = _pc.h;
      WriteAddress(_addressBus, _dataBus);
      _sp.hl -= 1;
      _opCycle += 1;
      break;
    case 4:
      _addressBus = _sp.hl;
      _dataBus = _pc.l;
      WriteAddress(_addressBus, _dataBus);
      _pc = _wz;
      _opCycle += 1;
      break;
    case 5:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::JP_HL()
{
  _pc = _hl;
}

void Cpu::JP_cc_nn(bool jump)
{
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _dataBus = ReadNextUint8();
      _wz.h = _dataBus;
      _opCycle += 1;
      break;
    case 2:
      _addressBus = 0x0000;
      if (jump) _pc = _wz;
      _opCycle = jump ? _opCycle + 1 : INSTRUCTION_COMPLETE;
      break;
    case 3:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::JR_cc_e(bool jump)
{
  int8_t val;
  switch (_opCycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = _pc.h;
      _opCycle = jump ? _opCycle + 1 : INSTRUCTION_COMPLETE;
      break;
    case 2:
      val = static_cast<int8_t>(_wz.l);
      _pc.hl += val;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::POP_rr(Register& rr)
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _sp.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _sp.hl += 1;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = _sp.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.h = _dataBus;
      _sp.hl += 1;
      _opCycle += 1;
      break;
    case 2:
      rr = _wz;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::RET(bool set_ime)
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _sp.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _sp.hl += 1;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = _sp.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.h = _dataBus;
      _sp.hl += 1;
      _opCycle += 1;
      break;
    case 2:
      _pc = _wz;
      if (set_ime) m_interrupt_controller.EnableInterrupts();
      _opCycle += 1;
      break;
    case 3:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::RET_cc(bool ret)
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = 0x0000;
      _opCycle += 1;
      break;
    case 1:
      if (!ret)
      {
        _opCycle = INSTRUCTION_COMPLETE;
        break;
      }
      _addressBus = _sp.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _sp.hl += 1;
      _opCycle += 1;
      break;
    case 2:
      _addressBus = _sp.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.h = _dataBus;
      _sp.hl += 1;
      _opCycle += 1;
      break;
    case 3:
      _pc = _wz;
      _opCycle += 1;
      break;
    case 4:
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::POP_AF()
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _sp.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _sp.hl += 1;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = _sp.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.h = _dataBus;
      _sp.hl += 1;
      _opCycle += 1;
      break;
    case 2:
      _af = _wz;
      _af.l &= 0xF0;
      _opCycle = INSTRUCTION_COMPLETE;
      break;
  }
}

void Cpu::RST(RestartVector vec)
{
  switch (_opCycle)
  {
    case 0:
      _addressBus = _sp.hl;
      _sp.hl -= 1;
      _opCycle += 1;
      break;
    case 1:
      _addressBus = _sp.hl;
      _dataBus = _pc.h;
      WriteAddress(_addressBus, _dataBus);
      _sp.hl -= 1;
      _opCycle += 1;
      break;
    case 2:
      _addressBus = _sp.hl;
      _dataBus = _pc.l;
      WriteAddress(_addressBus, _dataBus);
      _pc.hl = GetRestartVectorAddress(vec);
      _opCycle += 1;
      break;
    case 3:
      _opCycle = INSTRUCTION_COMPLETE;
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
  INC_r(_bc.h);
}

void Cpu::Op0x05()
{
  DEC_r(_bc.h);
}

void Cpu::Op0x06()
{
  LD_r_n(_bc.h);
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
  INC_r(_bc.l);
}

void Cpu::Op0x0D()
{
  DEC_r(_bc.l);
}

void Cpu::Op0x0E()
{
  LD_r_n(_bc.l);
}

void Cpu::Op0x0F()
{
  RRCA();
}

void Cpu::Op0x10()
{
  m_stop_requested = true;
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
  INC_r(_de.h);
}

void Cpu::Op0x15()
{
  DEC_r(_de.h);
}

void Cpu::Op0x16()
{
  LD_r_n(_de.h);
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
  INC_r(_de.l);
}

void Cpu::Op0x1D()
{
  DEC_r(_de.l);
}

void Cpu::Op0x1E()
{
  LD_r_n(_de.l);
}

void Cpu::Op0x1F()
{
  RRA();
}

void Cpu::Op0x21()
{
  LD_rr_nn(_hl);
}

void Cpu::Op0x22()
{
  LD__HLx_A(1);
}

void Cpu::Op0x23()
{
  ADD_rr(_hl, 1);
}

void Cpu::Op0x24()
{
  INC_r(_hl.h);
}

void Cpu::Op0x25()
{
  DEC_r(_hl.h);
}

void Cpu::Op0x26()
{
  LD_r_n(_hl.h);
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
  ADD_HL_rr(_hl);
}

void Cpu::Op0x2A()
{
  LD_A__HLx(1);
}

void Cpu::Op0x2B()
{
  ADD_rr(_hl, -1);
}

void Cpu::Op0x2C()
{
  INC_r(_hl.l);
}

void Cpu::Op0x2D()
{
  DEC_r(_hl.l);
}

void Cpu::Op0x2E()
{
  LD_r_n(_hl.l);
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
  LD_rr_nn(_sp);
}

void Cpu::Op0x32()
{
  LD__HLx_A(-1);
}

void Cpu::Op0x33()
{
  ADD_rr(_sp, 1);
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
  ADD_HL_rr(_sp);
}

void Cpu::Op0x3A()
{
  LD_A__HLx(-1);
}

void Cpu::Op0x3B()
{
  ADD_rr(_sp, -1);
}

void Cpu::Op0x3C()
{
  INC_r(_af.h);
}

void Cpu::Op0x3D()
{
  DEC_r(_af.h);
}

void Cpu::Op0x3E()
{
  LD_r_n(_af.h);
}

void Cpu::Op0x3F()
{
  CCF();
}

void Cpu::Op0x40()
{
  LD_r_x(_bc.h, _bc.h);
}

void Cpu::Op0x41()
{
  LD_r_x(_bc.h, _bc.l);
}

void Cpu::Op0x42()
{
  LD_r_x(_bc.h, _de.h);
}

void Cpu::Op0x43()
{
  LD_r_x(_bc.h, _de.l);
}

void Cpu::Op0x44()
{
  LD_r_x(_bc.h, _hl.h);
}

void Cpu::Op0x45()
{
  LD_r_x(_bc.h, _hl.l);
}

void Cpu::Op0x46()
{
  LD_r__HL(_bc.h);
}

void Cpu::Op0x47()
{
  LD_r_x(_bc.h, _af.h);

}

void Cpu::Op0x48()
{
  LD_r_x(_bc.l, _bc.h);

}

void Cpu::Op0x49()
{
  LD_r_x(_bc.l, _bc.l);

}

void Cpu::Op0x4A()
{
  LD_r_x(_bc.l, _de.h);

}

void Cpu::Op0x4B()
{
  LD_r_x(_bc.l, _de.l);
}

void Cpu::Op0x4C()
{
  LD_r_x(_bc.l, _hl.h);
}

void Cpu::Op0x4D()
{
  LD_r_x(_bc.l, _hl.l);
}

void Cpu::Op0x4E()
{
  LD_r__HL(_bc.l);
}

void Cpu::Op0x4F()
{
  LD_r_x(_bc.l, _af.h);
}

void Cpu::Op0x50()
{
  LD_r_x(_de.h, _bc.h);
}

void Cpu::Op0x51()
{
  LD_r_x(_de.h, _bc.l);
}

void Cpu::Op0x52()
{
  LD_r_x(_de.h, _de.h);
}

void Cpu::Op0x53()
{
  LD_r_x(_de.h, _de.l);
}

void Cpu::Op0x54()
{
  LD_r_x(_de.h, _hl.h);
}

void Cpu::Op0x55()
{
  LD_r_x(_de.h, _hl.l);
}

void Cpu::Op0x56()
{
  LD_r__HL(_de.h);
}

void Cpu::Op0x57()
{
  LD_r_x(_de.h, _af.h);
}

void Cpu::Op0x58()
{
  LD_r_x(_de.l, _bc.h);
}

void Cpu::Op0x59()
{
  LD_r_x(_de.l, _bc.l);
}

void Cpu::Op0x5A()
{
  LD_r_x(_de.l, _de.h);
}

void Cpu::Op0x5B()
{
  LD_r_x(_de.l, _de.l);
}

void Cpu::Op0x5C()
{
  LD_r_x(_de.l, _hl.h);

}

void Cpu::Op0x5D()
{
  LD_r_x(_de.l, _hl.l);
}

void Cpu::Op0x5E()
{
  LD_r__HL(_de.l);
}

void Cpu::Op0x5F()
{
  LD_r_x(_de.l, _af.h);
}

void Cpu::Op0x60()
{
  LD_r_x(_hl.h, _bc.h);
}

void Cpu::Op0x61()
{
  LD_r_x(_hl.h, _bc.l);
}

void Cpu::Op0x62()
{
  LD_r_x(_hl.h, _de.h);
}

void Cpu::Op0x63()
{
  LD_r_x(_hl.h, _de.l);
}

void Cpu::Op0x64()
{
  LD_r_x(_hl.h, _hl.h);
}

void Cpu::Op0x65()
{
  LD_r_x(_hl.h, _hl.l);
}

void Cpu::Op0x66()
{
  LD_r__HL(_hl.h);
}

void Cpu::Op0x67()
{
  LD_r_x(_hl.h, _af.h);
}

void Cpu::Op0x68()
{
  LD_r_x(_hl.l, _bc.h);
}

void Cpu::Op0x69()
{
  LD_r_x(_hl.l, _bc.l);
}

void Cpu::Op0x6A()
{
  LD_r_x(_hl.l, _de.h);
}

void Cpu::Op0x6B()
{
  LD_r_x(_hl.l, _de.l);
}

void Cpu::Op0x6C()
{
  LD_r_x(_hl.l, _hl.h);
}

void Cpu::Op0x6D()
{
  LD_r_x(_hl.l, _hl.l);
}

void Cpu::Op0x6E()
{
  LD_r__HL(_hl.l);
}

void Cpu::Op0x6F()
{
  LD_r_x(_hl.l, _af.h);
}

void Cpu::Op0x70()
{
  LD__rr_r(_hl, _bc.h);
}

void Cpu::Op0x71()
{
  LD__rr_r(_hl, _bc.l);
}

void Cpu::Op0x72()
{
  LD__rr_r(_hl, _de.h);
}

void Cpu::Op0x73()
{
  LD__rr_r(_hl, _de.l);
}

void Cpu::Op0x74()
{
  LD__rr_r(_hl, _hl.h);
}

void Cpu::Op0x75()
{
  LD__rr_r(_hl, _hl.l);
}

void Cpu::Op0x76()
{
  m_halt_requested = true;
}

void Cpu::Op0x77()
{
  LD__rr_r(_hl, _af.h);
}

void Cpu::Op0x78()
{
  LD_r_x(_af.h, _bc.h);
}

void Cpu::Op0x79()
{
  LD_r_x(_af.h, _bc.l);
}

void Cpu::Op0x7A()
{
  LD_r_x(_af.h, _de.h);
}

void Cpu::Op0x7B()
{
  LD_r_x(_af.h, _de.l);
}

void Cpu::Op0x7C()
{
  LD_r_x(_af.h, _hl.h);
}

void Cpu::Op0x7D()
{
  LD_r_x(_af.h, _hl.l);
}

void Cpu::Op0x7E()
{
  LD_r__HL(_af.h);
}

void Cpu::Op0x7F()
{
  LD_r_x(_af.h, _af.h);
}

void Cpu::Op0x80()
{
  ADD_A_x(_bc.h);
}

void Cpu::Op0x81()
{
  ADD_A_x(_bc.l);
}

void Cpu::Op0x82()
{
  ADD_A_x(_de.h);
}

void Cpu::Op0x83()
{
  ADD_A_x(_de.l);
}

void Cpu::Op0x84()
{
  ADD_A_x(_hl.h);
}

void Cpu::Op0x85()
{
  ADD_A_x(_hl.l);
}

void Cpu::Op0x86()
{
  ADD_A__HL();
}

void Cpu::Op0x87()
{
  ADD_A_x(_af.h);
}

void Cpu::Op0x88()
{
  ADC_A_x(_bc.h);
}

void Cpu::Op0x89()
{
  ADC_A_x(_bc.l);
}

void Cpu::Op0x8A()
{
  ADC_A_x(_de.h);
}

void Cpu::Op0x8B()
{
  ADC_A_x(_de.l);
}

void Cpu::Op0x8C()
{
  ADC_A_x(_hl.h);
}

void Cpu::Op0x8D()
{
  ADC_A_x(_hl.l);
}

void Cpu::Op0x8E()
{
  ADC_A__HL();
}

void Cpu::Op0x8F()
{
  ADC_A_x(_af.h);
}

void Cpu::Op0x90()
{
  SUB_A_x(_bc.h);
}

void Cpu::Op0x91()
{
  SUB_A_x(_bc.l);
}

void Cpu::Op0x92()
{
  SUB_A_x(_de.h);
}

void Cpu::Op0x93()
{
  SUB_A_x(_de.l);
}

void Cpu::Op0x94()
{
  SUB_A_x(_hl.h);
}

void Cpu::Op0x95()
{
  SUB_A_x(_hl.l);
}

void Cpu::Op0x96()
{
  SUB_A__HL();
}

void Cpu::Op0x97()
{
  SUB_A_x(_af.h);
}

void Cpu::Op0x98()
{
  SBC_A_x(_bc.h);
}

void Cpu::Op0x99()
{
  SBC_A_x(_bc.l);
}

void Cpu::Op0x9A()
{
  SBC_A_x(_de.h);
}

void Cpu::Op0x9B()
{
  SBC_A_x(_de.l);
}

void Cpu::Op0x9C()
{
  SBC_A_x(_hl.h);
}

void Cpu::Op0x9D()
{
  SBC_A_x(_hl.l);
}

void Cpu::Op0x9E()
{
  SBC_A__HL();
}

void Cpu::Op0x9F()
{
  SBC_A_x(_af.h);
}

void Cpu::Op0xA0()
{
  AND_A_x(_bc.h);
}

void Cpu::Op0xA1()
{
  AND_A_x(_bc.l);
}

void Cpu::Op0xA2()
{
  AND_A_x(_de.h);
}

void Cpu::Op0xA3()
{
  AND_A_x(_de.l);
}

void Cpu::Op0xA4()
{
  AND_A_x(_hl.h);
}

void Cpu::Op0xA5()
{
  AND_A_x(_hl.l);
}

void Cpu::Op0xA6()
{
  AND_A__HL();
}

void Cpu::Op0xA7()
{
  AND_A_x(_af.h);
}

void Cpu::Op0xA8()
{
  XOR_A_x(_bc.h);
}

void Cpu::Op0xA9()
{
  XOR_A_x(_bc.l);
}

void Cpu::Op0xAA()
{
  XOR_A_x(_de.h);
}

void Cpu::Op0xAB()
{
  XOR_A_x(_de.l);
}

void Cpu::Op0xAC()
{
  XOR_A_x(_hl.h);
}

void Cpu::Op0xAD()
{
  XOR_A_x(_hl.l);
}

void Cpu::Op0xAE()
{
  XOR_A__HL();
}

void Cpu::Op0xAF()
{
  XOR_A_x(_af.h);
}

void Cpu::Op0xB0()
{
  OR_A_X(_bc.h);
}

void Cpu::Op0xB1()
{
  OR_A_X(_bc.l);
}

void Cpu::Op0xB2()
{
  OR_A_X(_de.h);
}

void Cpu::Op0xB3()
{
  OR_A_X(_de.l);
}

void Cpu::Op0xB4()
{
  OR_A_X(_hl.h);
}

void Cpu::Op0xB5()
{
  OR_A_X(_hl.l);
}

void Cpu::Op0xB6()
{
  OR_A__HL();
}

void Cpu::Op0xB7()
{
  OR_A_X(_af.h);
}

void Cpu::Op0xB8()
{
  CP_A_x(_bc.h);
}

void Cpu::Op0xB9()
{
  CP_A_x(_bc.l);
}

void Cpu::Op0xBA()
{
  CP_A_x(_de.h);
}

void Cpu::Op0xBB()
{
  CP_A_x(_de.l);
}

void Cpu::Op0xBC()
{
  CP_A_x(_hl.h);
}

void Cpu::Op0xBD()
{
  CP_A_x(_hl.l);
}

void Cpu::Op0xBE()
{
  CP_A__HL();
}

void Cpu::Op0xBF()
{
  CP_A_x(_af.h);
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
  POP_rr(_hl);
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
  PUSH_rr(_hl);
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
  m_interrupt_controller.DisableInterrupts();
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
  m_interrupt_enabled_requested = true;
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
