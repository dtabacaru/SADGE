#include "Cpu.h"

void Cpu::NOP()
{
  // Hi mom!
}

void Cpu::LD_rr_nn(Register& rr)
{
  switch (_opcycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opcycle += 1;
      break;
    case 1:
      _dataBus = ReadNextUint8();
      _wz.h = _dataBus;
      _opcycle += 1;
      break;
    case 2:
      rr = _wz;
      _opcycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_r_n(uint8_t& r)
{
  switch (_opcycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      _wz.l = _dataBus;
      _opcycle += 1;
      break;
    case 1:
      r = _wz.l;
      _opcycle = INSTRUCTION_COMPLETE;
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
  switch (_opcycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opcycle += 1;
      break;
    case 1:
      r = _wz.l;
      _opcycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD__rr_r(Register rr, uint8_t r)
{
  switch (_opcycle)
  {
    case 0:
      _addressBus = rr.hl;
      _dataBus = r;
      WriteAddress(_addressBus, _dataBus);
      _opcycle += 1;
      break;
    case 1:
      _opcycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD__rr_A(Register rr)
{
  LD__rr_r(rr, _af.h);
}

void Cpu::LD__HLx_A(int val)
{
  LD__rr_r(_hl, _af.h);

  switch (_opcycle)
  {
    case 0:
      _hl.hl += val;
      break;
    case 1:
    default:
      break;
  }
}

void Cpu::LD_A__RR(Register rr)
{
  switch (_opcycle)
  {
    case 0:
      _addressBus = rr.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opcycle += 1;
      break;
    case 1:
      _af.h = _wz.l;
      _opcycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::LD_A__HLx(int val)
{
  LD_A__RR(_hl);

  switch (_opcycle)
  {
    case 0:
      _hl.hl += val;
      break;
    case 1:
    default:
      break;
  }
}

void Cpu::ADD_rr(Register& rr, int val)
{
  switch (_opcycle)
  {
    case 0:
      _addressBus = rr.hl;
      rr.hl += val;
      _opcycle += 1;
      break;
    case 1:
      _opcycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD_HL_rr(Register rr)
{
  int result{};

  switch (_opcycle)
  {
    case 0:
      _addressBus = 0x0000;
      _opcycle += 1;
      break;
    case 1:
      result = _hl.hl + rr.hl;
      ResetFlag(FlagBitMask::Subtract);
      SetHalfCarryFlag(_hl.hl, rr.hl);
      SetCarry16Bit(result);
      _hl.hl = result;
      _opcycle = INSTRUCTION_COMPLETE;
      break;
    default:
      break;
  }
}

void Cpu::ADD__HL(int val)
{
  uint8_t result{};

  switch (_opcycle)
  {
    case 0:
      _addressBus = _hl.hl;
      _dataBus = ReadAddress(_addressBus);
      _wz.l = _dataBus;
      _opcycle += 1;
      break;
    case 1:
      result = _wz.l + val;
      _dataBus = result;
      WriteAddress(_addressBus, _dataBus);
      SetHalfCarryFlag(_wz.l, val);
      SetZeroFlag(result);
      SetSubtractionFlag(val);
      _opcycle += 1;
      break;
    case 2:
      _opcycle = INSTRUCTION_COMPLETE;
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
}

void Cpu::ADD_A_X(int val)
{
  ADD_x(_af.h, val, true);
  ResetFlag(FlagBitMask::Subtract);
}

void Cpu::ADC_A_X(int val)
{
  int carry = ReadFlag(FlagBitMask::Carry) ? 1 : 0;
  int result = _af.h + val + carry;
  SetHalfCarryFlag(_af.h, val, carry);
  _af.h = result;
  SetZeroFlag(_af.h);
  ResetFlag(FlagBitMask::Subtract);
  SetCarry8Bit(result);
}

void Cpu::SBC_A_X(int val)
{
  int carry = ReadFlag(FlagBitMask::Carry) ? 1 : 0;
  int result = _af.h - val - carry;
  SetHalfCarryFlag(_af.h, -val, -carry);
  _af.h = result;
  SetZeroFlag(_af.h);
  SetFlag(FlagBitMask::Subtract);
  SetCarry8Bit(result);
}

void Cpu::SUB_A_X(int val)
{
  ADD_x(_af.h, -val, true);
  SetFlag(FlagBitMask::Subtract);
}

void Cpu::INC_R(uint8_t& r)
{
  ADD_x(r, 1);
  ResetFlag(FlagBitMask::Subtract);
}

void Cpu::DEC_R(uint8_t& r)
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
}

void Cpu::CCF()
{
  ResetFlag(FlagBitMask::Subtract);
  ResetFlag(FlagBitMask::HalfCarry);
  ReadFlag(FlagBitMask::Carry) ? ResetFlag(FlagBitMask::Carry) : SetFlag(FlagBitMask::Carry);
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
  INC_R(_bc.h);
}

void Cpu::Op0x05()
{
  DEC_R(_bc.h);
}

void Cpu::Op0x06()
{
  LD_r_n(_bc.h);
}

void Cpu::Op0x07()
{
  RLCA();
}

void Cpu::Op0x09()
{
  ADD_HL_rr(_bc);
}

void Cpu::Op0x0A()
{
  LD_A__RR(_bc);
}

void Cpu::Op0x0B()
{
  ADD_rr(_bc, -1);;
}

void Cpu::Op0x0C()
{
  INC_R(_bc.l);
}

void Cpu::Op0x0D()
{
  DEC_R(_bc.l);
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
  INC_R(_de.h);
}

void Cpu::Op0x15()
{
  DEC_R(_de.h);
}

void Cpu::Op0x16()
{
  LD_r_n(_de.h);
}

void Cpu::Op0x17()
{
  RLA();
}

void Cpu::Op0x19()
{
  ADD_HL_rr(_de);
}

void Cpu::Op0x1A()
{
  LD_A__RR(_de);
}

void Cpu::Op0x1B()
{
  ADD_rr(_de, -1);
}

void Cpu::Op0x1C()
{
  INC_R(_de.l);
}

void Cpu::Op0x1D()
{
  DEC_R(_de.l);
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
  INC_R(_hl.h);
}

void Cpu::Op0x25()
{
  DEC_R(_hl.h);
}

void Cpu::Op0x26()
{
  LD_r_n(_hl.h);
}

void Cpu::Op0x27()
{
  DAA();
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
  INC_R(_hl.l);
}

void Cpu::Op0x2D()
{
  DEC_R(_hl.l);
}

void Cpu::Op0x2E()
{
  LD_r_n(_hl.l);
}

void Cpu::Op0x2F()
{
  CPL();
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

void Cpu::Op0x37()
{
  SCF();
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
  INC_R(_af.h);
}

void Cpu::Op0x3D()
{
  DEC_R(_af.h);
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
  ADD_A_X(_bc.h);
}

void Cpu::Op0x81()
{
  ADD_A_X(_bc.l);
}

void Cpu::Op0x82()
{
  ADD_A_X(_de.h);
}

void Cpu::Op0x83()
{
  ADD_A_X(_de.l);
}

void Cpu::Op0x84()
{
  ADD_A_X(_hl.h);
}

void Cpu::Op0x85()
{
  ADD_A_X(_hl.l);
}

void Cpu::Op0x87()
{
  ADD_A_X(_af.h);
}

void Cpu::Op0x88()
{
  ADC_A_X(_bc.h);
}

void Cpu::Op0x89()
{
  ADC_A_X(_bc.l);
}

void Cpu::Op0x8A()
{
  ADC_A_X(_de.h);
}

void Cpu::Op0x8B()
{
  ADC_A_X(_de.l);
}

void Cpu::Op0x8C()
{
  ADC_A_X(_hl.h);
}

void Cpu::Op0x8D()
{
  ADC_A_X(_hl.l);
}

void Cpu::Op0x8F()
{
  ADC_A_X(_af.h);
}

void Cpu::Op0x90()
{
  SUB_A_X(_bc.h);
}

void Cpu::Op0x91()
{
  SUB_A_X(_bc.l);
}

void Cpu::Op0x92()
{
  SUB_A_X(_de.h);
}

void Cpu::Op0x93()
{
  SUB_A_X(_de.l);
}

void Cpu::Op0x94()
{
  SUB_A_X(_hl.h);
}

void Cpu::Op0x95()
{
  SUB_A_X(_hl.l);
}

void Cpu::Op0x97()
{
  SUB_A_X(_af.h);
}

void Cpu::Op0x98()
{
  SBC_A_X(_bc.h);
}

void Cpu::Op0x99()
{
  SBC_A_X(_bc.l);
}

void Cpu::Op0x9A()
{
  SBC_A_X(_de.h);
}

void Cpu::Op0x9B()
{
  SBC_A_X(_de.l);
}

void Cpu::Op0x9C()
{
  SBC_A_X(_hl.h);
}

void Cpu::Op0x9D()
{
  SBC_A_X(_hl.l);
}

void Cpu::Op0x9F()
{
  SBC_A_X(_af.h);
}