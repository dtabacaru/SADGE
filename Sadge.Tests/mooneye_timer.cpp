//#include "FrameBufferCpu.h"
//
//#include "gtest/gtest.h"
//
//#include <filesystem>
//#include <fstream>
//
//TEST(Mooneye, div_write)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\div_write.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(22500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\div_write_frame_dump.bin"));
//}
//
//TEST(Mooneye, rapid_toggle)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\rapid_toggle.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\rapid_toggle_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim00)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim00.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim00_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim00_div_trigger)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim00_div_trigger.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim00_div_trigger_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim01)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim01.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim01_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim01_div_trigger)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim01_div_trigger.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim01_div_trigger_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim10)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim10.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim10_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim10_div_trigger)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim10_div_trigger.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim10_div_trigger_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim11)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim11.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim11_frame_dump.bin"));
//}
//
//TEST(Mooneye, tim11_div_trigger)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tim11_div_trigger.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tim11_div_trigger_frame_dump.bin"));
//}
//
//TEST(Mooneye, tima_reload)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tima_reload.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tima_reload_frame_dump.bin"));
//}
//
//TEST(Mooneye, tima_write_reloading)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tima_write_reloading.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tima_write_reloading_frame_dump.bin"));
//}
//
//TEST(Mooneye, tma_write_reloading)
//{
//  FrameBufferCpu cpu;
//
//  std::filesystem::path rom_path = "mooneye\\timer\\tma_write_reloading.gb";
//  uint64_t file_size = std::filesystem::file_size(rom_path);
//  std::ifstream rom_stream(rom_path, std::ios_base::binary);
//  std::vector<uint8_t> rom(file_size);
//  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom.size());
//
//  cpu.SetRom(rom_path, rom);
//  cpu.RunUntil(17500000);
//
//  EXPECT_TRUE(CheckFrameBuffer(cpu.GetLcdController().GetCurrentFrameBuffer(), "mooneye\\timer\\tma_write_reloading_frame_dump.bin"));
//}
