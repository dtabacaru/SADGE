#pragma once

#include "Cpu.h"


class SstCpu : public Cpu
{
public:
  SstCpu() : Cpu() {}

  uint8_t ReadAddress(uint16_t address)
  {
    return m_test_ram[address];
  }

  void WriteAddress(uint16_t address, uint8_t val)
  {
    m_test_ram[address] = val;
  }

  void InstructionCompleteEvent() 
  {
    // No  overlap fetch in SSTs
  }

  void Main()
  {
    m_test_cycles.clear();

    Fetch();

    do
    {
      m_test_cycles.push_back({_addressBus, _dataBus});
      TickExecution();
    } while (_exeCycle > 0);
  }

  std::vector<TestCycle> GetTestCycles()
  {
    return m_test_cycles;
  }

private:
  std::vector<uint8_t>   m_test_ram = std::vector<uint8_t>(64 * 1024); // Cpu tests require flat ram
  std::vector<TestCycle> m_test_cycles;
};