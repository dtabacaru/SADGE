#include "FrameBufferCpu.h"

#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

TEST(Blargg, cpu_instrs)
{
  FrameBufferCpu cpu;

  std::filesystem::path rom_path = "blargg\\cpu_instrs.gb";
  uint64_t file_size = std::filesystem::file_size(rom_path);
  std::ifstream rom_stream(rom_path, std::ios_base::binary);
  std::vector<uint8_t> rom(file_size);
  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());

  cpu.SetRom(rom_path, rom);
  cpu.RunUntil(254000000);

  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "blargg\\cpu_instrs_frame_dump.bin"));
}

TEST(Blargg, instr_timing)
{
  FrameBufferCpu cpu;

  std::filesystem::path rom_path = "blargg\\instr_timing.gb";
  uint64_t file_size = std::filesystem::file_size(rom_path);
  std::ifstream rom_stream(rom_path, std::ios_base::binary);
  std::vector<uint8_t> rom(file_size);
  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());

  cpu.SetRom(rom_path, rom);
  cpu.RunUntil(30000000);

  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "blargg\\instr_timing_frame_dump.bin"));
}