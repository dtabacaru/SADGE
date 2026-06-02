#include "FrameBufferCpu.h"

#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

TEST(Mooneye, div_write)
{
  static FrameBufferCpu cpu(22500000);

  std::filesystem::path rom_path = "mooneye\\timer\\div_write.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\div_write_frame_dump.bin"));
}

TEST(Mooneye, rapid_toggle)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\rapid_toggle.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\rapid_toggle_frame_dump.bin"));
}

TEST(Mooneye, tim00)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tim00.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tim00_frame_dump.bin"));
}

TEST(Mooneye, tim00_div_trigger)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tim00_div_trigger.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tim00_div_trigger_frame_dump.bin"));
}

TEST(Mooneye, tim01)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tim01.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tim01_frame_dump.bin"));
}

TEST(Mooneye, tim01_div_trigger)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tim01_div_trigger.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tim01_div_trigger_frame_dump.bin"));
}

TEST(Mooneye, tim10)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tim10.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tim10_frame_dump.bin"));
}

TEST(Mooneye, tim10_div_trigger)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tim10_div_trigger.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tim10_div_trigger_frame_dump.bin"));
}

TEST(Mooneye, tim11)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tim11.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tim11_frame_dump.bin"));
}

TEST(Mooneye, tim11_div_trigger)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tim11_div_trigger.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tim11_div_trigger_frame_dump.bin"));
}

TEST(Mooneye, tima_reload)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tima_reload.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tima_reload_frame_dump.bin"));
}

TEST(Mooneye, tima_write_reloading)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tima_write_reloading.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tima_write_reloading_frame_dump.bin"));
}

TEST(Mooneye, tma_write_reloading)
{
  static FrameBufferCpu cpu(17500000);

  std::filesystem::path rom_path = "mooneye\\timer\\tma_write_reloading.gb";

  cpu.InsertRom(rom_path);
  cpu.Run();

  EXPECT_TRUE(cpu.CheckFrameBuffer("mooneye\\timer\\tma_write_reloading_frame_dump.bin"));
}
