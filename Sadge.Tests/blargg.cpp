#include "FrameBufferCpu.h"

#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

TEST(Blargg, cpu_instrs)
{
  static FrameBufferCpu cpu(475000000);

  std::filesystem::path rom_path = "blargg\\cpu_instrs.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("blargg\\cpu_instrs_frame_dump.bin"));
}

TEST(Blargg, instr_timing)
{
  static FrameBufferCpu cpu(22500000);

  std::filesystem::path rom_path = "blargg\\instr_timing.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("blargg\\instr_timing_frame_dump.bin"));
}