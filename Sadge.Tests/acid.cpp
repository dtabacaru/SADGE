#include "FrameBufferCpu.h"

#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

TEST(Acid, DmgAcid2)
{ 
  FrameBufferCpu cpu;

  std::filesystem::path rom_path = "acid\\dmg-acid2.gb";
  uint64_t file_size = std::filesystem::file_size(rom_path);
  std::ifstream rom_stream(rom_path, std::ios_base::binary);
  std::vector<uint8_t> rom(file_size);
  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());

  cpu.SetRom(rom_path, rom);
  cpu.RunUntil(25000000);

  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "acid\\dmg-acid2_frame_dump.bin"));
}