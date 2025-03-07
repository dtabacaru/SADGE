#include "GameBoy.h"

#include <fstream>
#include <iostream>

Status GameBoy::InsertRom(const std::filesystem::path& rom_path)
{
  Status status;

  size_t rom_size = std::filesystem::file_size(rom_path);

  if (rom_size > Cpu::MAX_ROM_SIZE)
  {
    std::string msg = "ROM too large to load.";

    status.SetMsg(msg);
    return status;
  }

  std::ifstream rom_stream(rom_path, std::ios_base::binary);

  if (!rom_stream)
  {
    std::string msg = "Could not open ROM file.";

    status.SetMsg(msg);
    return status;
  }

  std::vector<uint8_t> rom;
  rom.resize(rom_size);

  rom_stream.read(reinterpret_cast<char*>(rom.data()), rom_size);

  if (!rom_stream)
  {
    std::string msg = "Could not read ROM file.";

    status.SetMsg(msg);
    return status;
  }

  m_cpu.SetRom(rom_path, std::move(rom));

  return status;
}
