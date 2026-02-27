//#include "Cpu.h"
//
//#include "gtest/gtest.h"
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
//TEST(Mooneye, div_write)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\div_write.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(30000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\div_write_frame_dump.bin"));
//}
//
//TEST(Mooneye, rapid_toggle)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\rapid_toggle.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\rapid_toggle_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim00)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim00.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim00_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim00_div_trigger)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim00_div_trigger.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim00_div_trigger_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim01)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim01.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim01_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim01_div_trigger)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim01_div_trigger.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim01_div_trigger_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim10)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim10.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim10_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim10_div_trigger)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim10_div_trigger.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim10_div_trigger_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim11)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim11.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim11_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim11_div_trigger)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim11_div_trigger.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim11_div_trigger_frame_dump.bin"));
//}
//
//TEST(Mooneye, tima_reload)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tima_reload.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tima_reload_frame_dump.bin"));
//}
//
//TEST(Mooneye, tima_write_reloading)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tima_write_reloading.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tima_write_reloading_frame_dump.bin"));
//}
//
//TEST(Mooneye, tma_write_reloading)
//{
//  CpuFrameBufferTest cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tma_write_reloading.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(25000000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tma_write_reloading_frame_dump.bin"));
//}
