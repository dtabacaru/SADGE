#pragma once

#include "Cpu.h"

struct TestCycle
{
  uint16_t addrBus;
  uint8_t  dataBus;
};

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
    // No overlap fetch in SSTs
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

  void SetAddressBus(uint16_t addr)
  {
    mAddressBus = addr;
  }

  void SetDataBus(uint8_t data)
  {
    mDataBus = data;
  }

  uint8_t GetDataBus()
  {
    return mDataBus;
  }

  void SetState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime, uint8_t ie)
  {
    ime;
    ie;

    mPC.HL = pc;
    mSP.HL = sp;
    mAF.H = a;
    mBC.H = b;
    mBC.L = c;
    mDE.H = d;
    mDE.L = e;
    mAF.L = f;
    mHL.H = h;
    mHL.L = l;
  }

  bool CheckState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime)
  {
    ime;

    if (pc != mPC.HL)
      return false;
    else if (sp != mSP.HL)
      return false;
    else if (a != mAF.H)
      return false;
    else if (b != mBC.H)
      return false;
    else if (c != mBC.L)
      return false;
    else if (d != mDE.H)
      return false;
    else if (e != mDE.L)
      return false;
    else if (f != mAF.L)
      return false;
    else if (h != mHL.H)
      return false;
    else if (l != mHL.L)
      return false;
    else
      return true;
  }

  std::vector<TestCycle> GetTestCycles()
  {
    return mTestCycles;
  }

private:
  std::vector<uint8_t>   mFlatRam = std::vector<uint8_t>(64 * 1024);
  std::vector<TestCycle> mTestCycles;
};
