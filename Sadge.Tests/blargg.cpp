//#include "Cpu.h"
//
//#include "gtest/gtest.h"
//
//
//#include <filesystem>
//#include <fstream>
//
//// Don't wait at all
//class CpuFrameBufferTest : public Cpu
//{
//public:
//  CpuFrameBufferTest() : Cpu() {}
//private:
//  void WaitFrame() {}
//};
//
//static inline bool ColorIsEqual(Pixel a, Pixel b)
//{
//  return a.R == b.R && a.G == b.G && a.B == b.B && a.A == b.A;
//}
//
//static bool CheckFrameBuffer(std::array<Pixel, SCREEN_SIZE>& frame_buffer, std::filesystem::path ref_frame_buffer_path)
//{
//  std::ifstream ref_frame_buffer_stream(ref_frame_buffer_path);
//  std::vector<Pixel> ref_frame_buffer(SCREEN_SIZE);
//  ref_frame_buffer_stream.read(reinterpret_cast<char*>(ref_frame_buffer.data()), SCREEN_SIZE * sizeof(Pixel));
//
//  for (int i = 0; i < ref_frame_buffer.size(); i += 1)
//  {
//    if (!ColorIsEqual(ref_frame_buffer[i], frame_buffer[i]))
//      return false;
//  }
//
//  return true;
//}
//
//TEST(Blargg, cpu_instrs)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "blargg\\cpu_instrs.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(254000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "blargg\\cpu_instrs_frame_dump.bin"));
//}
//
//TEST(Blargg, instr_timing)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "blargg\\instr_timing.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(30000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "blargg\\instr_timing_frame_dump.bin"));
//}