#pragma once

#include "Cpu.h"

class SstCpu : public Cpu
{
public:
  SstCpu() : Cpu() {}

  void Read()
  {
    mDataBus = mFlatRam[mAddressBus];
  }

  void Write()
  {
    mFlatRam[mAddressBus] = mDataBus;
  }

  void InstructionCompleteEvent() 
  {
    // No  overlap fetch in SSTs
  }

  void Main()
  {
    mTestCycles.clear();

    Fetch();

    do
    {
      mTestCycles.push_back({mAddressBus, mDataBus});
      TickExecution();
    } while (mExeCycle > 0);
  }

  std::vector<TestCycle> GetTestCycles()
  {
    return mTestCycles;
  }

private:
  std::vector<uint8_t>   mFlatRam = std::vector<uint8_t>(64 * 1024);
  std::vector<TestCycle> mTestCycles;
};