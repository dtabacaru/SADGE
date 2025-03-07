#include "RomHeaderEnums.h"

NewLicensee NewLicenseeCodeToEnum(const char* new_licensee_code)
{
  std::string new_licensee_code_str(new_licensee_code, 2);

  if (new_licensee_code_str == "00")
    return NewLicensee::NONE;
  if (new_licensee_code_str == "01")
    return NewLicensee::NINTENDO_RESEARCH_AND_DEVELOPMENT_1;
  if (new_licensee_code_str == "08")
    return NewLicensee::CAPCOM;
  if (new_licensee_code_str == "13")
    return NewLicensee::EA;
  if (new_licensee_code_str == "18")
    return NewLicensee::HUDSON_SOFT;
  if (new_licensee_code_str == "19")
    return NewLicensee::BAI;
  if (new_licensee_code_str == "20")
    return NewLicensee::KSS;
  if (new_licensee_code_str == "22")
    return NewLicensee::PLANNING_OFFICE_WADA;
  if (new_licensee_code_str == "24")
    return NewLicensee::PCM_COMPLETE;
  if (new_licensee_code_str == "25")
    return NewLicensee::SAN_X;
  if (new_licensee_code_str == "28")
    return NewLicensee::KEMCO;
  if (new_licensee_code_str == "29")
    return NewLicensee::SETA_CORPORATION;
  if (new_licensee_code_str == "30")
    return NewLicensee::VIACOM;
  if (new_licensee_code_str == "31")
    return NewLicensee::NINTENDO;
  if (new_licensee_code_str == "32")
    return NewLicensee::BANDAI;
  if (new_licensee_code_str == "33")
    return NewLicensee::OCEAN_SOFTWARE_ACCLAIM_ENTERTAINMENT;
  if (new_licensee_code_str == "34")
    return NewLicensee::KONAMI;
  if (new_licensee_code_str == "35")
    return NewLicensee::HECTOR_SOFT;
  if (new_licensee_code_str == "37")
    return NewLicensee::TAITO;
  if (new_licensee_code_str == "38")
    return NewLicensee::HUDSON_SOFT;
  if (new_licensee_code_str == "39")
    return NewLicensee::BANPRESTO;
  if (new_licensee_code_str == "41")
    return NewLicensee::UBI_SOFT_1;
  if (new_licensee_code_str == "42")
    return NewLicensee::ATLUS;
  if (new_licensee_code_str == "44")
    return NewLicensee::MALIBU_INTERACTIVE;
  if (new_licensee_code_str == "46")
    return NewLicensee::ANGEL;
  if (new_licensee_code_str == "47")
    return NewLicensee::BULLET_PROOF_SOFTWARE_2;
  if (new_licensee_code_str == "49")
    return NewLicensee::IREM;
  if (new_licensee_code_str == "50")
    return NewLicensee::ABSOLUTE;
  if (new_licensee_code_str == "51")
    return NewLicensee::ACCLAIM_ENTERTAINMENT;
  if (new_licensee_code_str == "52")
    return NewLicensee::ACTIVISION;
  if (new_licensee_code_str == "53")
    return NewLicensee::SAMMY_USA_CORPORATION;
  if (new_licensee_code_str == "54")
    return NewLicensee::KONAMI;
  if (new_licensee_code_str == "55")
    return NewLicensee::HI_TECH_EXPRESSIONS;
  if (new_licensee_code_str == "56")
    return NewLicensee::LJN;
  if (new_licensee_code_str == "57")
    return NewLicensee::MATCHBOX;
  if (new_licensee_code_str == "58")
    return NewLicensee::MATTEL;
  if (new_licensee_code_str == "59")
    return NewLicensee::MILTON_BRADLEY_COMPANY;
  if (new_licensee_code_str == "60")
    return NewLicensee::TITUS_INTERACTIVE;
  if (new_licensee_code_str == "61")
    return NewLicensee::VIRGIN_GAMES_LTD_3;
  if (new_licensee_code_str == "64")
    return NewLicensee::LUCASFILM_GAMES_4;
  if (new_licensee_code_str == "67")
    return NewLicensee::OCEAN_SOFTWARE;
  if (new_licensee_code_str == "69")
    return NewLicensee::EA;
  if (new_licensee_code_str == "70")
    return NewLicensee::INFOGRAMES_5;
  if (new_licensee_code_str == "71")
    return NewLicensee::INTERPLAY_ENTERTAINMENT;
  if (new_licensee_code_str == "72")
    return NewLicensee::BRODERBUND;
  if (new_licensee_code_str == "73")
    return NewLicensee::SCULPTURED_SOFTWARE_6;
  if (new_licensee_code_str == "75")
    return NewLicensee::THE_SALES_CURVE_LIMITED_7;
  if (new_licensee_code_str == "78")
    return NewLicensee::THQ;
  if (new_licensee_code_str == "79")
    return NewLicensee::ACCOLADE;
  if (new_licensee_code_str == "80")
    return NewLicensee::MISAWA_ENTERTAINMENT;
  if (new_licensee_code_str == "83")
    return NewLicensee::LOZC;
  if (new_licensee_code_str == "86")
    return NewLicensee::TOKUMA_SHOTEN;
  if (new_licensee_code_str == "87")
    return NewLicensee::TSUKUDA_ORIGINAL;
  if (new_licensee_code_str == "91")
    return NewLicensee::CHUNSOFT_CO_8;
  if (new_licensee_code_str == "92")
    return NewLicensee::VIDEO_SYSTEM;
  if (new_licensee_code_str == "93")
    return NewLicensee::OCEAN_SOFTWARE_ACCLAIM_ENTERTAINMENT;
  if (new_licensee_code_str == "95")
    return NewLicensee::VARIE;
  if (new_licensee_code_str == "96")
    return NewLicensee::YONEZAWA_SPAL;
  if (new_licensee_code_str == "97")
    return NewLicensee::KANEKO;
  if (new_licensee_code_str == "99")
    return NewLicensee::PACK_IN_VIDEO;
  if (new_licensee_code_str == "9H")
    return NewLicensee::BOTTOM_UP;
  if (new_licensee_code_str == "A4")
    return NewLicensee::KONAMI_YU_GI_OH;
  if (new_licensee_code_str == "BL")
    return NewLicensee::MTO;
  if (new_licensee_code_str == "DK")
    return NewLicensee::KODANSHA;
  else
    return NewLicensee::NONE;
}

std::string NewLicenseeToString(NewLicensee licensee)
{
  switch (licensee)
  {
    case NewLicensee::NONE:
      return "None";
    case NewLicensee::NINTENDO_RESEARCH_AND_DEVELOPMENT_1:
      return "Nintendo Research & Development 1";
    case NewLicensee::CAPCOM:
      return "Capcom";
    case NewLicensee::EA:
      return "EA (Electronic Arts)";
    case NewLicensee::HUDSON_SOFT:
      return "Hudson Soft";
    case NewLicensee::BAI:
      return "B-AI";
    case NewLicensee::KSS:
      return "KSS";
    case NewLicensee::PLANNING_OFFICE_WADA:
      return "Planning Office WADA";
    case NewLicensee::PCM_COMPLETE:
      return "PCM Complete";
    case NewLicensee::SAN_X:
      return "San-X";
    case NewLicensee::KEMCO:
      return "Kemco";
    case NewLicensee::SETA_CORPORATION:
      return "SETA Corporation";
    case NewLicensee::VIACOM:
      return "Viacom";
    case NewLicensee::NINTENDO:
      return "Nintendo";
    case NewLicensee::BANDAI:
      return "Bandai";
    case NewLicensee::OCEAN_SOFTWARE_ACCLAIM_ENTERTAINMENT:
      return "Ocean Software/Acclaim Entertainment";
    case NewLicensee::KONAMI:
      return "Konami";
    case NewLicensee::HECTOR_SOFT:
      return "HectorSoft";
    case NewLicensee::TAITO:
      return "Taito";
    case NewLicensee::BANPRESTO:
      return "Banpresto";
    case NewLicensee::UBI_SOFT_1:
      return "Ubi Soft1";
    case NewLicensee::ATLUS:
      return "Atlus";
    case NewLicensee::MALIBU_INTERACTIVE:
      return "Malibu Interactive";
    case NewLicensee::ANGEL:
      return "Angel";
    case NewLicensee::BULLET_PROOF_SOFTWARE_2:
      return "Bullet-Proof Software2";
    case NewLicensee::IREM:
      return "Irem";
    case NewLicensee::ABSOLUTE:
      return "Absolute";
    case NewLicensee::ACCLAIM_ENTERTAINMENT:
      return "Acclaim Entertainment";
    case NewLicensee::ACTIVISION:
      return "Activision";
    case NewLicensee::SAMMY_USA_CORPORATION:
      return "Sammy USA Corporation";
    case NewLicensee::HI_TECH_EXPRESSIONS:
      return "Hi Tech Expressions";
    case NewLicensee::LJN:
      return "LJN";
    case NewLicensee::MATCHBOX:
      return "Matchbox";
    case NewLicensee::MATTEL:
      return "Mattel";
    case NewLicensee::MILTON_BRADLEY_COMPANY:
      return "Milton Bradley Company";
    case NewLicensee::TITUS_INTERACTIVE:
      return "Titus Interactive";
    case NewLicensee::VIRGIN_GAMES_LTD_3:
      return "Virgin Games Ltd.3";
    case NewLicensee::LUCASFILM_GAMES_4:
      return "Lucasfilm Games4";
    case NewLicensee::OCEAN_SOFTWARE:
      return "Ocean Software";
    case NewLicensee::INFOGRAMES_5:
      return "Infogrames5";
    case NewLicensee::INTERPLAY_ENTERTAINMENT:
      return "Interplay Entertainment";
    case NewLicensee::BRODERBUND:
      return "Broderbund";
    case NewLicensee::SCULPTURED_SOFTWARE_6:
      return "Sculptured Software6";
    case NewLicensee::THE_SALES_CURVE_LIMITED_7:
      return "The Sales Curve Limited7";
    case NewLicensee::THQ:
      return "THQ";
    case NewLicensee::ACCOLADE:
      return "Accolade";
    case NewLicensee::MISAWA_ENTERTAINMENT:
      return "Misawa Entertainment";
    case NewLicensee::LOZC:
      return "lozc";
    case NewLicensee::TOKUMA_SHOTEN:
      return "Tokuma Shoten";
    case NewLicensee::TSUKUDA_ORIGINAL:
      return "Tsukuda Original";
    case NewLicensee::CHUNSOFT_CO_8:
      return "Chunsoft Co.8";
    case NewLicensee::VIDEO_SYSTEM:
      return "Video System";
    case NewLicensee::VARIE:
      return "Varie";
    case NewLicensee::YONEZAWA_SPAL:
      return "Yonezawa/s’pal";
    case NewLicensee::KANEKO:
      return "Kaneko";
    case NewLicensee::PACK_IN_VIDEO:
      return "Pack-In-Video";
    case NewLicensee::BOTTOM_UP:
      return "Bottom Up";
    case NewLicensee::KONAMI_YU_GI_OH:
      return "Konami (Yu-Gi-Oh!)";
    case NewLicensee::MTO:
      return "MTO";
    case NewLicensee::KODANSHA:
      return "Kodansha";
    default:
      return "";
  }
}

std::string CartridgeTypeToString(CartridgeType cartridge_type)
{
  switch (cartridge_type)
  {
    case CartridgeType::ROM_ONLY:
      return "ROM ONLY";
    case CartridgeType::MBC1:
      return "MBC1";
    case CartridgeType::MBC1_RAM:
      return "MBC1+RAM";
    case CartridgeType::MBC1_RAM_BATTERY:
      return "MBC1+RAM+BATTERY";
    case CartridgeType::MBC2:
      return "MBC2";
    case CartridgeType::MBC2_BATTERY:
      return "MBC2+BATTERY";
    case CartridgeType::ROM_RAM:
      return "ROM+RAM";
    case CartridgeType::ROM_RAM_BATTERY:
      return "ROM+RAM+BATTERY";
    case CartridgeType::MMM01:
      return "MMM01";
    case CartridgeType::MMM01_RAM:
      return "MMM01+RAM";
    case CartridgeType::MMM01_RAM_BATTERY:
      return "MMM01+RAM+BATTERYreturn ";
    case CartridgeType::MBC3_TIMER_BATTERY:
      return "MBC3+TIMER+BATTERY";
    case CartridgeType::MBC3_TIMER_RAM_BATTERY:
      return "MBC3+TIMER+RAM+BATTERY";
    case CartridgeType::MBC3:
      return "MBC3";
    case CartridgeType::MBC3_RAM:
      return "MBC3+RAM";
    case CartridgeType::MBC3_RAM_BATTERY:
      return "MBC3+RAM+BATTERY";
    case CartridgeType::MBC5:
      return "MBC5";
    case CartridgeType::MBC5_RAM:
      return "MBC5+RAM";
    case CartridgeType::MBC5_RAM_BATTERY:
      return "MBC5+RAM+BATTERY";
    case CartridgeType::MBC5_RUMBLE:
      return "MBC5+RUMBLE";
    case CartridgeType::MBC5_RUMBLE_RAM:
      return "MBC5+RUMBLE+RAM";
    case CartridgeType::MBC5_RUMBLE_RAM_BATTERY:
      return "MBC5+RUMBLE+RAM+BATTERY";
    case CartridgeType::MBC6:
      return "MBC6";
    case CartridgeType::MBC7_SENSOR_RUMBLE_RAM_BATTERY:
      return "MBC7+SENSOR+RUMBLE+RAM+BATTERY";
    case CartridgeType::POCKET_CAMERA:
      return "POCKET CAMERA";
    case CartridgeType::BANDAI_TAMA5:
      return "BANDAI TAMA5";
    case CartridgeType::HuC3:
      return "HuC3";
    case CartridgeType::HuC1_RAM_BATTERY:
      return "HuC1+RAM+BATTERY";
    default:
      return "";
  }
}

std::string RomSizeToString(RomSize rom_size)
{
  switch (rom_size)
  {
    case RomSize::_32_KiB_2_BANKS:
      return "32 KiB 2 banks (no banking)";
    case   RomSize::_64_KiB_4_BANKS:
      return "64 KiB 4 banks";
    case   RomSize::_128_KiB_8_BANKS:
      return "128 KiB 8 banks";
    case RomSize::_256_KiB_16_BANKS:
      return "256 KiB 16 banks";
    case RomSize::_512_KiB_32_BANKS:
      return "512 KiB 32 banks";
    case RomSize::_1_MiB_64_BANKS:
      return "1 MiB 64 banks";
    case  RomSize::_2_MiB_128_BANKS:
      return "2 MiB 128 banks";
    case  RomSize::_4_MiB_256_BANKS:
      return "4 MiB 256 banks";
    case  RomSize::_8_MiB_512_BANKS:
      return "8 MiB 512 banks";
    case  RomSize::_1_1_MiB_72_BANKS:
      return "1.1 MiB 72 banks";
    case  RomSize::_1_2_MiB_80_BANKS:
      return "1.2 MiB 80 banks";
    case  RomSize::_1_5_MiB_96_BANKS:
      return "1.5 MiB 96 banks";
    default:
      return "";
  }
}

std::string RamSizeToString(RamSize ram_size)
{
  switch (ram_size)
  {
    case RamSize::_0_KiB_0_BANKS:
      return "0 KiB 0 banks";
    case RamSize::UNUSED:
      return "UNUSED";
    case RamSize::_8_KiB_1_BANKS:
      return "8 KiB 1 banks";
    case RamSize::_32_KiB_4_BANKS:
      return "32 KiB 4 banks";
    case RamSize::_128_KiB_16_BANKS:
      return "128 KiB 16 banks";
    case RamSize::_64_KiB_8_BANKS:
      return "64 KiB 8 banks";
    default:
      return "";
  }
}

std::string RegionToString(Region region)
{
  switch (region)
  {
    case Region::JAPAN:
      return "Japan";
    case Region::OVERSEAS_ONLY:
      return "Overseas only";
    default:
      return "";
  }
}

std::string OldLicenseeToString(OldLicensee licensee)
{
  switch (licensee)
  {
    case OldLicensee::NONE:
      return "None";
    case OldLicensee::NINTENDO:
      return "Nintendo";
    case OldLicensee::CAPCOM:
      return "Capcom";
    case OldLicensee::HOT_B:
      return "HOT-B";
    case OldLicensee::JALECO:
      return "Jaleco";
    case OldLicensee::COCONUTS_JAPAN:
      return "Coconuts Japan";
    case OldLicensee::ELITE_SYSTEMS:
      return "Elite Systems";
    case OldLicensee::EA_ELECTRONIC_ARTS:
      return "EA (Electronic Arts)";
    case OldLicensee::HUDSON_SOFT:
      return "Hudson Soft";
    case OldLicensee::ITC_ENTERTAINMENT:
      return "ITC Entertainment";
    case OldLicensee::YANOMAN:
      return "Yanoman";
    case OldLicensee::JAPAN_CLARY:
      return "Japan Clary";
    case OldLicensee::VIRGIN_GAMES_LTD_3:
      return "Virgin Games Ltd.3";
    case OldLicensee::PCM_COMPLETE:
      return "PCM Complete";
    case OldLicensee::SAN_X:
      return "San-X";
    case OldLicensee::KEMCO:
      return "Kemco";
    case OldLicensee::SETA_CORPORATION:
      return "SETA Corporation";
    case OldLicensee::INFOGRAMES_5:
      return "Infogrames5";
    case OldLicensee::NINTENDO_2:
      return "Nintendo";
    case OldLicensee::BANDAI:
      return "Bandai";
    case OldLicensee::NEW_LICENSEE:
      return "New Licensee";
    case OldLicensee::KONAMI:
      return "Konami";
    case OldLicensee::HECTOR_SOFT:
      return "HectorSoft";
    case OldLicensee::CAPCOM_2:
      return "Capcom";
    case OldLicensee::BANPRESTO:
      return "Banpresto";
    case OldLicensee::ENTERTAINMENT_I:
      return ".Entertainment i";
    case OldLicensee::GREMLIN:
      return "Gremlin";
    case OldLicensee::UBI_SOFT_1:
      return "Ubi Soft1";
    case OldLicensee::ATLUS:
      return "Atlus";
    case OldLicensee::MALIBU_INTERACTIVE:
      return "Malibu Interactive";
    case OldLicensee::ANGEL:
      return "Angel";
    case OldLicensee::SPECTRUM_HOLOBY:
      return "Spectrum Holoby";
    case OldLicensee::IREM:
      return "Irem";
    case OldLicensee::VIRGIN_GAMES_LTD_3_2:
      return "Virgin Games Ltd.3";
    case OldLicensee::MALIBU_INTERACTIVE_2:
      return "Malibu Interactive";
    case OldLicensee::US_GOLD:
      return "U.S. Gold";
    case OldLicensee::ABSOLUTE:
      return "Absolute";
    case OldLicensee::ACCLAIM_ENTERTAINMENT:
      return "Acclaim Entertainment";
    case OldLicensee::ACTIVISION:
      return "Activision";
    case OldLicensee::SAMMY_USA_CORPORATION:
      return "Sammy USA Corporation";
    case OldLicensee::GAMETEK:
      return "GameTek";
    case OldLicensee::PARK_PLACE:
      return "Park Place";
    case OldLicensee::LJN:
      return "LJN";
    case OldLicensee::MATCHBOX:
      return "Matchbox";
    case OldLicensee::MILTON_BRADLEY_COMPANY:
      return "Milton Bradley Company";
    case OldLicensee::MINDSCAPE:
      return "Mindscape";
    case OldLicensee::ROMSTAR:
      return "Romstar";
    case OldLicensee::NAXAT_SOFT_13:
      return "Naxat Soft13";
    case OldLicensee::TRADEWEST:
      return "Tradewest";
    case OldLicensee::TITUS_INTERACTIVE:
      return "Titus Interactive";
    case OldLicensee::VIRGIN_GAMES_LTD_3_3:
      return "Virgin Games Ltd.3";
    case OldLicensee::OCEAN_SOFTWARE:
      return "Ocean Software";
    case OldLicensee::EA_ELECTRONIC_ARTS_2:
      return "EA (Electronic Arts)";
    case OldLicensee::ELITE_SYSTEMS_2:
      return "Elite Systems";
    case OldLicensee::ELECTRO_BRAIN:
      return "Electro Brain";
    case OldLicensee::INFOGRAMES_5_2:
      return "Infogrames5";
    case OldLicensee::INTERPLAY_ENTERTAINMENT:
      return "Interplay Entertainment";
    case OldLicensee::BRODERBUND:
      return "Broderbund";
    case OldLicensee::SCULPTURED_SOFTWARE_6:
      return "Sculptured Software6";
    case OldLicensee::THE_SALES_CURVE_LIMITED_7:
      return "The Sales Curve Limited7";
    case OldLicensee::THQ:
      return "THQ";
    case OldLicensee::ACCOLADE:
      return "Accolade";
    case OldLicensee::TRIFFIX_ENTERTAINMENT:
      return "Triffix Entertainment";
    case OldLicensee::MICROPROSE:
      return "Microprose";
    case OldLicensee::KEMCO_2:
      return "Kemco";
    case OldLicensee::MISAWA_ENTERTAINMENT:
      return "Misawa Entertainment";
    case OldLicensee::LOZC:
      return "Lozc";
    case OldLicensee::TOKUMA_SHOTEN:
      return "Tokuma Shoten";
    case OldLicensee::BULLET_PROOF_SOFTWARE_2:
      return "Bullet-Proof Software2";
    case OldLicensee::VIC_TOKAI:
      return "Vic Tokai";
    case OldLicensee::APE:
      return "Ape";
    case OldLicensee::I_MAX:
      return "I’Max";
    case OldLicensee::CHUNSOFT_CO_8:
      return "Chunsoft Co.8";
    case OldLicensee::VIDEO_SYSTEM:
      return "Video System";
    case OldLicensee::TSUBARAYA_PRODUCTIONS:
      return "Tsubaraya Productions";
    case OldLicensee::VARIE:
      return "Varie";
    case OldLicensee::YONEZAWA_S_PAL:
      return "Yonezawa/S’Pal";
    case OldLicensee::KEMCO_3:
      return "Kemco";
    case OldLicensee::ARC:
      return "Arc";
    case OldLicensee::NIHON_BUSSAN:
      return "Nihon Bussan";
    case OldLicensee::TECMO:
      return "Tecmo";
    case OldLicensee::IMAGINEER:
      return "Imagineer";
    case OldLicensee::BANPRESTO_2:
      return "Banpresto";
    case OldLicensee::NOVA:
      return "Nova";
    case OldLicensee::HORI_ELECTRIC:
      return "Hori Electric";
    case OldLicensee::BANDAI_2:
      return "Bandai";
    case OldLicensee::KONAMI_2:
      return "Konami";
    case OldLicensee::KAWADA:
      return "Kawada";
    case OldLicensee::TAKARA:
      return "Takara";
    case OldLicensee::TECHNOS_JAPAN:
      return "Technos Japan";
    case OldLicensee::BRODERBUND_2:
      return "Broderbund";
    case OldLicensee::TOEI_ANIMATION:
      return "Toei Animation";
    case OldLicensee::TOHO:
      return "Toho";
    case OldLicensee::NAMCO:
      return "Namco";
    case OldLicensee::ACCLAIM_ENTERTAINMENT_2:
      return "Acclaim Entertainment";
    case OldLicensee::ASCII_CORPORATION_OR_NEXSOFT:
      return "ASCII Corporation or Nexsoft";
    case OldLicensee::BANDAI_3:
      return "Bandai";
    case OldLicensee::SQUARE_ENIX:
      return "Square Enix";
    case OldLicensee::HAL_LABORATORY:
      return "HAL Laboratory";
    case OldLicensee::SNK:
      return "SNK";
    case OldLicensee::PONY_CANYON:
      return "Pony Canyon";
    case OldLicensee::CULTURE_BRAIN:
      return "Culture Brain";
    case OldLicensee::SUNSOFT:
      return "Sunsoft";
    case OldLicensee::SONY_IMAGESOFT:
      return "Sony Imagesoft";
    case OldLicensee::SAMMY_CORPORATION:
      return "Sammy Corporation";
    case OldLicensee::TAITO:
      return "Taito";
    case OldLicensee::KEMCO_4:
      return "Kemco";
    case OldLicensee::SQUARE:
      return "Square";
    case OldLicensee::TOKUMA_SHOTEN_2:
      return "Tokuma Shoten";
    case OldLicensee::DATA_EAST:
      return "Data East";
    case OldLicensee::TONKINHOUSE:
      return "Tonkinhouse";
    case OldLicensee::KOEI:
      return "Koei";
    case OldLicensee::UFL:
      return "UFL";
    case OldLicensee::ULTRA:
      return "Ultra";
    case OldLicensee::VAP:
      return "Vap";
    case OldLicensee::USE_CORPORATION:
      return "Use Corporation";
    case OldLicensee::MELDAC:
      return "Meldac";
    case OldLicensee::PONY_CANYON_2:
      return "Pony Canyon";
    case OldLicensee::ANGEL_2:
      return "Angel";
    case OldLicensee::TAITO_2:
      return "Taito";
    case OldLicensee::SOFEL:
      return "Sofel";
    case OldLicensee::QUEST:
      return "Quest";
    case OldLicensee::SIGMA_ENTERPRISES:
      return "Sigma Enterprises";
    case OldLicensee::ASK_KODANSHA_CO:
      return "ASK Kodansha Co.";
    case OldLicensee::NAXAT_SOFT_13_2:
      return "Naxat Soft13";
    case OldLicensee::COPYA_SYSTEM:
      return "Copya System";
    case OldLicensee::BANPRESTO_3:
      return "Banpresto";
    case OldLicensee::TOMY:
      return "Tomy";
    case OldLicensee::LJN_2:
      return "LJN";
    case OldLicensee::NCS:
      return "NCS";
    case OldLicensee::HUMAN:
      return "Human";
    case OldLicensee::ALTRON:
      return "Altron";
    case OldLicensee::JALECO_2:
      return "Jaleco";
    case OldLicensee::TOWA_CHIKI:
      return "Towa Chiki";
    case OldLicensee::YUTAKA:
      return "Yutaka";
    case OldLicensee::VARIE_2:
      return "Varie";
    case OldLicensee::EPCOH:
      return "Epcoh";
    case OldLicensee::ATHENA:
      return "Athena";
    case OldLicensee::ASMIMK_ACE_ENTERTAINMENT:
      return "Asmik Ace Entertainment";
    case OldLicensee::NATSUME:
      return "Natsume";
    case OldLicensee::KING_RECORDS:
      return "King Records";
    case OldLicensee::ATLUS_2:
      return "Atlus";
    case OldLicensee::EPIC_SONY_RECORDS:
      return "Epic/Sony Records";
    case OldLicensee::IGS:
      return "IGS";
    case OldLicensee::A_WAVE:
      return "A Wave";
    case OldLicensee::EXTREME_ENTERTAINMENT:
      return "Extreme Entertainment";
    case OldLicensee::LJN_3:
      return "LJN";
    default:
      return "";
  }
}

int RamSizeToNumBanks(RamSize ram_size)
{
  switch (ram_size)
  {
    case RamSize::_0_KiB_0_BANKS:
      // Fall through
    case RamSize::UNUSED:
      // Fall through
    case RamSize::_8_KiB_1_BANKS:
      return 1;
    case RamSize::_32_KiB_4_BANKS:
      return 4;
    case RamSize::_128_KiB_16_BANKS:
      return 16;
    case RamSize::_64_KiB_8_BANKS:
      return 8;
    default:
      return 1;
  }
}
