#include "Cpu.h"

void Cpu::LD_RR_dd(uint16_t& RR)
{
  static uint8_t l{};
  static uint8_t h{};

  switch (_op_cycle)
  {
    case 0:
      _dataBus = ReadNextUint8();
      l = _dataBus;
      break;
    case 1:
      _dataBus = ReadNextUint8();
      h = _dataBus;
      break;
    case 2:
      RR = (h << 8) | l;
      _op_cycle = 0;
      return;
    default:
      break;
  }

  _op_cycle += 1;
}

void Cpu::Op0x00()
{
  // Crickets
}

void Cpu::Op0x01()
{
  LD_RR_dd(m_bc.hl);
}

void Cpu::Op0x11()
{
  LD_RR_dd(m_de.hl);
}

void Cpu::Op0x21()
{
  LD_RR_dd(m_hl.hl);
}

void Cpu::Op0x31()
{
  LD_RR_dd(m_sp);
}

uint16_t Cpu::ReadNextUint16()
{
  _addressBus = m_pc;
  uint8_t l = ReadAddress(m_pc);
  m_pc += 1;
  uint8_t h = ReadAddress(m_pc);
  m_pc += 1;

  return (h << 8) | l;
}
