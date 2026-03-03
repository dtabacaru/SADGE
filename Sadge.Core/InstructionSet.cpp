#include "Cpu.h"

// Flags: - - - -
void Cpu::LD_rr_nn(Register& rr)
{
  switch (_opcycle)
  {
    case 0:
      _nn.l, _dataBus = ReadNextUint8();
      _opcycle += 1;
      break;
    case 1:
      _nn.h, _dataBus = ReadNextUint8();
      _opcycle += 1;
      break;
    case 2:
      rr = _nn;
      break;
    default:
      break;
  }
}

// Flags: - - - -
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
      _opcycle = 0;
      break;
    default:
      break;
  }
}

// Flags: - 0 H C
void Cpu::ADD_HL_rr(Register rr)
{
  int result{};

  switch (_opcycle)
  {
    case 0:
      _addressBus = 0x0000;
      //result = _hl.l + RR.l;
      //ResetFlag(FlagBitMask::Subtract);
      //SetHalfCarryFlag(_hl.l, RR.l);
      //SetCarry8Bit(result);
      //_hl.l = result;
      _opcycle += 1;
      break;
    case 1:
      //result = _hl.h + RR.h + ReadFlag(FlagBitMask::Carry);
      //ResetFlag(FlagBitMask::Subtract);
      //SetHalfCarryFlag(_hl.h, RR.h, ReadFlag(FlagBitMask::Carry));
      //SetCarry8Bit(result);
      //_hl.h = result;
      result = _hl.hl + rr.hl;
      ResetFlag(FlagBitMask::Subtract);
      SetHalfCarryFlag(_hl.hl, rr.hl);
      SetCarry16Bit(result);
      _hl.hl = result;
      _opcycle = 0;
      break;
    default:
      break;
  }
}

void Cpu::INC_rr(Register& rr)
{
  ADD_rr(rr, 1);
}

// Flags: - - - -
void Cpu::DEC_rr(Register& rr)
{
  ADD_rr(rr, -1);
}

void Cpu::Op0x00()
{
  // Crickets
}

void Cpu::Op0x01()
{
  LD_rr_nn(_bc);
}

void Cpu::Op0x03()
{
  INC_rr(_bc);
}

void Cpu::Op0x09()
{
  ADD_HL_rr(_bc);
}

void Cpu::Op0x0B()
{
  DEC_rr(_bc);
}

void Cpu::Op0x11()
{
  LD_rr_nn(_de);
}

void Cpu::Op0x13()
{
  INC_rr(_de);
}

void Cpu::Op0x19()
{
  ADD_HL_rr(_de);
}

void Cpu::Op0x1B()
{
  DEC_rr(_de);
}

void Cpu::Op0x21()
{
  LD_rr_nn(_hl);
}

void Cpu::Op0x23()
{
  INC_rr(_hl);
}

void Cpu::Op0x29()
{
  ADD_HL_rr(_hl);
}

void Cpu::Op0x2B()
{
  DEC_rr(_hl);
}

void Cpu::Op0x31()
{
  LD_rr_nn(_sp);
}

void Cpu::Op0x33()
{
  INC_rr(_sp);
}

void Cpu::Op0x39()
{
  ADD_HL_rr(_sp);
}

void Cpu::Op0x3B()
{
  DEC_rr(_sp);
}
