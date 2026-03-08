#pragma once

#include "Cpu.h"

// Don't wait at all
class FrameBufferCpu : public Cpu
{
public:
  FrameBufferCpu() : Cpu() {}
private:
  void WaitFrame() {}
};

static bool ColorIsEqual(Pixel a, Pixel b)
{
  return a.R == b.R && a.G == b.G && a.B == b.B && a.A == b.A;
}

static bool CheckFrameBuffer(std::array<Pixel, SCREEN_SIZE>& frame_buffer, std::filesystem::path ref_frame_buffer_path)
{
  std::ifstream ref_frame_buffer_stream(ref_frame_buffer_path);
  std::vector<Pixel> ref_frame_buffer(SCREEN_SIZE);
  ref_frame_buffer_stream.read(reinterpret_cast<char*>(ref_frame_buffer.data()), SCREEN_SIZE * sizeof(Pixel));

  for (int i = 0; i < ref_frame_buffer.size(); i += 1)
  {
    if (!ColorIsEqual(ref_frame_buffer[i], frame_buffer[i]))
      return false;
  }

  return true;
}