#include "FrameBufferCpu.h"

#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

TEST(Acid, DmgAcid2)
{ 
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path romPath = "acid\\dmg-acid2.gb";

  cpu.InsertRom(romPath);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("acid\\dmg-acid2_frame_dump.bin"));
}