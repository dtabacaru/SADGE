#pragma once

#include "RomHeaderEnums.h"

#include <execution>
#include <iostream>
#include <vector>

enum class RomState
{
  VALID,
  BAD_NINTENDO_LOGO,
  BAD_HEADER_CHECKSUM,
  BAD_GLOBAL_CHECKSUM
};

class RomHeader
{
public:

  constexpr static uint16_t ENTRY_POINT_SIZE = 4;
  constexpr static uint16_t NINTENDO_LOGO_SIZE = 48;
  constexpr static uint16_t ROM_TITLE_SIZE = 16;
  constexpr static uint16_t NEW_LICENSEE_CODE_SIZE = 2;
  constexpr static uint16_t SGB_FLAG_SIZE = 1;
  constexpr static uint16_t CARTRIDGE_TYPE_SIZE = 1;
  constexpr static uint16_t ROM_SIZE = 1;
  constexpr static uint16_t RAM_SIZE = 1;
  constexpr static uint16_t REGION_SIZE = 1;
  constexpr static uint16_t OLD_LICENSEE_SIZE = 1;
  constexpr static uint16_t VERSION_SIZE = 1;
  constexpr static uint16_t HEADER_CHECKSUM_SIZE = 1;
  constexpr static uint16_t GLOBAL_CHECKSUM_SIZE = 2;

  constexpr static uint16_t SIZE =
    ENTRY_POINT_SIZE +
    NINTENDO_LOGO_SIZE +
    ROM_TITLE_SIZE +
    NEW_LICENSEE_CODE_SIZE +
    SGB_FLAG_SIZE +
    CARTRIDGE_TYPE_SIZE +
    ROM_SIZE +
    RAM_SIZE +
    REGION_SIZE +
    OLD_LICENSEE_SIZE +
    VERSION_SIZE +
    HEADER_CHECKSUM_SIZE +
    GLOBAL_CHECKSUM_SIZE;

  constexpr static uint8_t NINTENDO_LOGO_BYTES[NINTENDO_LOGO_SIZE] =
  {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
  };

  std::vector<uint8_t> GetEntryPoint()
  {
    std::vector<uint8_t> entry_point_bytes;
    for (uint8_t byte : m_entry_point)
      entry_point_bytes.push_back(byte);
    return entry_point_bytes;
  }

  std::string GetRomTitle() const { return m_rom_title; }

  uint8_t GetSgbFlag() const { return m_sgb_flag; }

  CartridgeType GetCartridgeType() const { return m_cartridge_type; }

  RomSize GetRomSize() const { return m_rom_size; }

  RamSize GetRamSize() const { return m_ram_size; }

  Region GetRegion() const { return m_region; }

  std::string GetLicenseeString() const
  {
    if (m_old_licensee == OldLicensee::NEW_LICENSEE)
      return NewLicenseeToString(NewLicenseeCodeToEnum(m_new_licensee_code));
    else
      return OldLicenseeToString(m_old_licensee);
  }

  uint8_t GetVersion() const { return m_version; }

  uint16_t GetGlobalChecksum() const { return m_global_checksum; }

  uint8_t GetHeaderChecksum() const { return m_header_checksum; }

  RomState Valid(const std::vector<uint8_t>&rom) const;

  uint8_t GetColorHash() const
  {
    uint8_t sum = 0;
    std::string rom_title = GetRomTitle();
    for (int i = 0; i < rom_title.size(); i += 1)
      sum += rom_title[i];
  }

  void DrawNintendoLogo(NintendoLogoSource source) const
  {
    switch (source)
    {
      case NintendoLogoSource::EMU:
        DrawNintendoLogo(NINTENDO_LOGO_BYTES);
        break;
      case NintendoLogoSource::ROM:
        DrawNintendoLogo(m_nintendo_logo);
        break;
    }
  }

  bool IsTypicalEntryPoint() const
  {
    // NOP
    // JP $0150
    return m_entry_point[0] == 0x00 &&
      m_entry_point[1] == 0xC3 &&
      m_entry_point[2] == 0x50 &&
      m_entry_point[3] == 0x01;
  }

  bool SupportsSgbFunctions() const { return m_sgb_flag == 0x03; }

  bool CheckNintendoLogo() const;

  bool CheckHeaderChecksum() const;

  bool CheckGlobalChecksum(const std::vector<uint8_t>&rom) const;

private:
  uint8_t m_entry_point[ENTRY_POINT_SIZE]{};
  uint8_t m_nintendo_logo[NINTENDO_LOGO_SIZE]{};
  char m_rom_title[ROM_TITLE_SIZE]{};
  char m_new_licensee_code[NEW_LICENSEE_CODE_SIZE]{};
  uint8_t m_sgb_flag{};
  CartridgeType m_cartridge_type{};
  RomSize m_rom_size{};
  RamSize m_ram_size{};
  Region m_region{};
  OldLicensee m_old_licensee{};
  uint8_t m_version{};
  uint8_t m_header_checksum{};
  uint16_t m_global_checksum{};

  constexpr static size_t LOGO_ROW_SIZE = 8;
  constexpr static size_t LOGO_COLUMN_SIZE = 12;
  void DrawNintendoLogo(const uint8_t* bytes) const;

  static uint16_t EndianConvert(uint16_t num)
  {
    return (num << 8) + (num >> 8);
  }
};