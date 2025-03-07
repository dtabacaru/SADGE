#include "RomHeader.h"

RomState RomHeader::Valid(const std::vector<uint8_t>&rom) const
{
  bool is_valid = true;

  is_valid = CheckNintendoLogo();
  if (!is_valid)
  {
    return RomState::BAD_NINTENDO_LOGO;
  }

  is_valid = CheckHeaderChecksum();
  if (!is_valid)
  {
    return RomState::BAD_HEADER_CHECKSUM;
  }

  is_valid = CheckGlobalChecksum(rom);
  if (!is_valid)
  {
    return RomState::BAD_GLOBAL_CHECKSUM;
  }

  return RomState::VALID;
}

//
// [][]      [][]  [][]                                                          [][]
// [][][]    [][]  [][]                [][]                                      [][]
// [][][]    [][]                    [][][][]                                    [][]
// [][]  []  [][]  [][]  [][]  [][]    [][]    [][][][]    [][]  [][]      [][][][][]    [][][][]
// [][]  []  [][]  [][]  [][][]  [][]  [][]  [][]    [][]  [][][]  [][]  [][]    [][]  [][]    [][]
// [][]    [][][]  [][]  [][]    [][]  [][]  [][][][][][]  [][]    [][]  [][]    [][]  [][]    [][]
// [][]    [][][]  [][]  [][]    [][]  [][]  [][]          [][]    [][]  [][]    [][]  [][]    [][]
// [][]      [][]  [][]  [][]    [][]  [][]    [][][][][]  [][]    [][]    [][][][][]    [][][][]
//

void RomHeader::DrawNintendoLogo(const uint8_t* bytes) const
{
  // Eight rows of pixels
  for (size_t row = 0; row < LOGO_ROW_SIZE; row += 1)
  {
    // Draw line by line
    for (size_t column = 0; column < LOGO_COLUMN_SIZE; column += 1)
    {
      size_t index = (row % 4) < 2 ? column * 2 : column * 2 + 1;
      index += row < 4 ? 0 : NINTENDO_LOGO_SIZE / 2;

      uint8_t val = bytes[index] >> (row % 2 ? 0 : 4);
      for (int test_bit = 3; test_bit >= 0; test_bit -= 1)
      {
        std::cout << (((val >> test_bit) &0x01) ? "[]" : "  ");
      }
    }
    std::cout << std::endl;
  }
}

bool RomHeader::CheckGlobalChecksum(const std::vector<uint8_t>&rom) const
{
  uint16_t checksum = EndianConvert(m_global_checksum);
  uint16_t sum = 0;

  std::for_each(std::execution::seq, rom.begin(), rom.end(), [&sum](const uint8_t&b)
  {
    sum += b;
  });

  // Checksum bytes not included
  sum -= rom[0x014E];
  sum -= rom[0x014F];

  return sum == checksum;
}

bool RomHeader::CheckHeaderChecksum() const
{
  constexpr uint16_t CHECKSUM_SIZE = ROM_TITLE_SIZE +
    NEW_LICENSEE_CODE_SIZE +
    SGB_FLAG_SIZE +
    CARTRIDGE_TYPE_SIZE +
    ROM_SIZE +
    RAM_SIZE +
    REGION_SIZE +
    OLD_LICENSEE_SIZE +
    VERSION_SIZE;

  uint8_t checksum_bytes[CHECKSUM_SIZE]{};
  int index = 0;
  memcpy(checksum_bytes + index, m_rom_title, ROM_TITLE_SIZE);
  index += ROM_TITLE_SIZE;
  memcpy(checksum_bytes + index, m_new_licensee_code, NEW_LICENSEE_CODE_SIZE);
  index += NEW_LICENSEE_CODE_SIZE;
  memcpy(checksum_bytes + index, &m_sgb_flag, SGB_FLAG_SIZE);
  index += SGB_FLAG_SIZE;
  memcpy(checksum_bytes + index, &m_cartridge_type, CARTRIDGE_TYPE_SIZE);
  index += CARTRIDGE_TYPE_SIZE;
  memcpy(checksum_bytes + index, &m_rom_size, ROM_SIZE);
  index += ROM_SIZE;
  memcpy(checksum_bytes + index, &m_ram_size, RAM_SIZE);
  index += RAM_SIZE;
  memcpy(checksum_bytes + index, &m_region, REGION_SIZE);
  index += REGION_SIZE;
  memcpy(checksum_bytes + index, &m_old_licensee, OLD_LICENSEE_SIZE);
  index += OLD_LICENSEE_SIZE;
  memcpy(checksum_bytes + index, &m_version, VERSION_SIZE);
  index += VERSION_SIZE;

  uint8_t checksum = 0;
  for (size_t i = 0; i < CHECKSUM_SIZE; i += 1)
  {
    checksum -= checksum_bytes[i] + 1;
  }

  return m_header_checksum == checksum;
}

bool RomHeader::CheckNintendoLogo() const
{
  bool is_valid = true;

  for (size_t i = 0; i < NINTENDO_LOGO_SIZE; i += 1)
  {
    if (NINTENDO_LOGO_BYTES[i] != m_nintendo_logo[i])
    {
      is_valid = false;
      break;
    }
  }

  return is_valid;
}
