#pragma once

#include "Cpu.h"

class FrameBufferCpu : public Cpu
{
public:

  FrameBufferCpu(uint64_t numCycles) : 
    Cpu(), 
    mNumCycles(numCycles)
  {
  }

  void Run() override
  {
    Init();

    do
    {
      Main();
    } while (mTEdgeCount < mNumCycles);
  }

  bool CheckFrameBuffer(std::filesystem::path refFrameBufPath)
  {
    mFrameBuf = mLcdCtrl.GetCurrentFrameBuffer();

    std::ifstream refFrameBufStream(refFrameBufPath, std::ios_base::binary);

    if (!refFrameBufStream)
      return false;

    refFrameBufStream.read(reinterpret_cast<char*>(mRefFrameBuf.data()), SCREEN_SIZE * sizeof(Pixel));

    for (int i = 0; i < mRefFrameBuf.size(); i += 1)
    {
      if (mRefFrameBuf[i] != mFrameBuf[i])
        return false;
    }

    return true;
  }

private:

  void WaitFrame() override
  {
  }

  std::array<Pixel, SCREEN_SIZE> mRefFrameBuf;
  std::array<Pixel, SCREEN_SIZE> mFrameBuf;
  uint64_t mNumCycles;
};
