#include "Cpu.h"

#include "gtest/gtest.h"
#include "json/json.h"

#include <fstream>

#define ENABLED

// Cpu tests require flat ram
class CpuInstructionTest : public Cpu
{
public:
  CpuInstructionTest() : Cpu() {}

  uint8_t ReadAddress(uint16_t address)
  {
    return m_test_ram[address];
  }

  void WriteAddress(uint16_t address, uint8_t val)
  {
    m_test_ram[address] = val;
  }
private:
  std::vector<uint8_t> m_test_ram = std::vector<uint8_t>(64 * 1024);
};

static void TestOpcode(uint8_t opcode)
{
  std::string test_file_path = std::format("sm83-main\\v1\\{:02x}.json", opcode);
  std::ifstream file(test_file_path, std::ifstream::binary);

  EXPECT_TRUE(file.is_open());

  Json::Value root;
  Json::CharReaderBuilder builder;
  builder["collectComments"] = false;
  JSONCPP_STRING errs;

  bool parsed = Json::parseFromStream(builder, file, &root, &errs);
  EXPECT_TRUE(parsed);

  EXPECT_TRUE(root.isArray());

  CpuInstructionTest cpu_;

  for (const auto& test : root)
  {
    const auto& initial = test["initial"];

    cpu_.SetTestState(initial["pc"].asUInt(),
                      initial["sp"].asUInt(),
                      initial["a"].asUInt(),
                      initial["b"].asUInt(),
                      initial["c"].asUInt(),
                      initial["d"].asUInt(),
                      initial["e"].asUInt(),
                      initial["f"].asUInt(),
                      initial["h"].asUInt(),
                      initial["l"].asUInt(),
                      initial["ime"].asUInt(),
                      initial["ie"].asUInt());

    for (const auto& ram : initial["ram"])
    {
      cpu_.WriteAddress(ram[0].asUInt(), ram[1].asUInt());
    }

    cpu_.TestExecute();

    const auto& final_ = test["final"];

    bool state_passed = cpu_.CheckTestState(final_["pc"].asUInt(),
                                            final_["sp"].asUInt(),
                                            final_["a"].asUInt(),
                                            final_["b"].asUInt(),
                                            final_["c"].asUInt(),
                                            final_["d"].asUInt(),
                                            final_["e"].asUInt(),
                                            final_["f"].asUInt(),
                                            final_["h"].asUInt(),
                                            final_["l"].asUInt(),
                                            final_["ime"].asUInt());

    EXPECT_TRUE(state_passed) << test["name"].asString();

    for (const auto& ram : final_["ram"])
    {
      bool ram_passed = cpu_.ReadAddress(ram[0].asUInt()) == ram[1].asUInt();

      EXPECT_TRUE(ram_passed) << test["name"].asString();
    }

    const auto& test_cycles = test["cycles"];

    for (int cycle_num = 0; cycle_num < test_cycles.size(); cycle_num += 1)
    {
      const auto& test_cycle = test_cycles[cycle_num];

      bool address_matched = test_cycle[0].asUInt() == cpu_.GetTestCycles()[cycle_num].address_bus;
      bool data_matched = test_cycle[1].asUInt() == cpu_.GetTestCycles()[cycle_num].data_bus;

      EXPECT_TRUE(address_matched) << test["name"].asString();
      EXPECT_TRUE(data_matched) << test["name"].asString();
    }
  }
}

#ifdef ENABLED
TEST(Instructions, OP0x00) { TestOpcode(0x00); } // ok
TEST(Instructions, OP0x01) { TestOpcode(0x01); } // ok
//TEST(Instructions, OP0x02) { TestOpcode(0x02); } 
//TEST(Instructions, OP0x03) { TestOpcode(0x03); } 
//TEST(Instructions, OP0x04) { TestOpcode(0x04); } 
//TEST(Instructions, OP0x05) { TestOpcode(0x05); } 
//TEST(Instructions, OP0x06) { TestOpcode(0x06); } 
//TEST(Instructions, OP0x07) { TestOpcode(0x07); } 
//TEST(Instructions, OP0x08) { TestOpcode(0x08); } 
//TEST(Instructions, OP0x09) { TestOpcode(0x09); } 
//TEST(Instructions, OP0x0A) { TestOpcode(0x0A); } 
//TEST(Instructions, OP0x0B) { TestOpcode(0x0B); } 
//TEST(Instructions, OP0x0C) { TestOpcode(0x0C); } 
//TEST(Instructions, OP0x0D) { TestOpcode(0x0D); } 
//TEST(Instructions, OP0x0E) { TestOpcode(0x0E); } 
//TEST(Instructions, OP0x0F) { TestOpcode(0x0F); } 
//
//TEST(Instructions, OP0x10) { TestOpcode(0x10); } 
TEST(Instructions, OP0x11) { TestOpcode(0x11); } // ok
//TEST(Instructions, OP0x12) { TestOpcode(0x12); } 
//TEST(Instructions, OP0x13) { TestOpcode(0x13); } 
//TEST(Instructions, OP0x14) { TestOpcode(0x14); } 
//TEST(Instructions, OP0x15) { TestOpcode(0x15); } 
//TEST(Instructions, OP0x16) { TestOpcode(0x16); } 
//TEST(Instructions, OP0x17) { TestOpcode(0x17); } 
//TEST(Instructions, OP0x18) { TestOpcode(0x18); } 
//TEST(Instructions, OP0x19) { TestOpcode(0x19); } 
//TEST(Instructions, OP0x1A) { TestOpcode(0x1A); } 
//TEST(Instructions, OP0x1B) { TestOpcode(0x1B); } 
//TEST(Instructions, OP0x1C) { TestOpcode(0x1C); } 
//TEST(Instructions, OP0x1D) { TestOpcode(0x1D); } 
//TEST(Instructions, OP0x1E) { TestOpcode(0x1E); } 
//TEST(Instructions, OP0x1F) { TestOpcode(0x1F); } 
//
//TEST(Instructions, OP0x20) { TestOpcode(0x20); } 
TEST(Instructions, OP0x21) { TestOpcode(0x21); } //ok
//TEST(Instructions, OP0x22) { TestOpcode(0x22); } 
//TEST(Instructions, OP0x23) { TestOpcode(0x23); } 
//TEST(Instructions, OP0x24) { TestOpcode(0x24); } 
//TEST(Instructions, OP0x25) { TestOpcode(0x25); } 
//TEST(Instructions, OP0x26) { TestOpcode(0x26); } 
//TEST(Instructions, OP0x27) { TestOpcode(0x27); } 
//TEST(Instructions, OP0x28) { TestOpcode(0x28); } 
//TEST(Instructions, OP0x29) { TestOpcode(0x29); } 
//TEST(Instructions, OP0x2A) { TestOpcode(0x2A); } 
//TEST(Instructions, OP0x2B) { TestOpcode(0x2B); } 
//TEST(Instructions, OP0x2C) { TestOpcode(0x2C); } 
//TEST(Instructions, OP0x2D) { TestOpcode(0x2D); } 
//TEST(Instructions, OP0x2E) { TestOpcode(0x2E); } 
//TEST(Instructions, OP0x2F) { TestOpcode(0x2F); } 
//
//TEST(Instructions, OP0x30) { TestOpcode(0x30); } 
TEST(Instructions, OP0x31) { TestOpcode(0x31); } 
//TEST(Instructions, OP0x32) { TestOpcode(0x32); } 
//TEST(Instructions, OP0x33) { TestOpcode(0x33); } 
//TEST(Instructions, OP0x34) { TestOpcode(0x34); } 
//TEST(Instructions, OP0x35) { TestOpcode(0x35); } 
//TEST(Instructions, OP0x36) { TestOpcode(0x36); } 
//TEST(Instructions, OP0x37) { TestOpcode(0x37); } 
//TEST(Instructions, OP0x38) { TestOpcode(0x38); } 
//TEST(Instructions, OP0x39) { TestOpcode(0x39); } 
//TEST(Instructions, OP0x3A) { TestOpcode(0x3A); } 
//TEST(Instructions, OP0x3B) { TestOpcode(0x3B); } 
//TEST(Instructions, OP0x3C) { TestOpcode(0x3C); } 
//TEST(Instructions, OP0x3D) { TestOpcode(0x3D); } 
//TEST(Instructions, OP0x3E) { TestOpcode(0x3E); } 
//TEST(Instructions, OP0x3F) { TestOpcode(0x3F); } 
//
//TEST(Instructions, OP0x40) { TestOpcode(0x40); } 
//TEST(Instructions, OP0x41) { TestOpcode(0x41); } 
//TEST(Instructions, OP0x42) { TestOpcode(0x42); } 
//TEST(Instructions, OP0x43) { TestOpcode(0x43); } 
//TEST(Instructions, OP0x44) { TestOpcode(0x44); } 
//TEST(Instructions, OP0x45) { TestOpcode(0x45); } 
//TEST(Instructions, OP0x46) { TestOpcode(0x46); } 
//TEST(Instructions, OP0x47) { TestOpcode(0x47); } 
//TEST(Instructions, OP0x48) { TestOpcode(0x48); } 
//TEST(Instructions, OP0x49) { TestOpcode(0x49); } 
//TEST(Instructions, OP0x4A) { TestOpcode(0x4A); } 
//TEST(Instructions, OP0x4B) { TestOpcode(0x4B); } 
//TEST(Instructions, OP0x4C) { TestOpcode(0x4C); } 
//TEST(Instructions, OP0x4D) { TestOpcode(0x4D); } 
//TEST(Instructions, OP0x4E) { TestOpcode(0x4E); } 
//TEST(Instructions, OP0x4F) { TestOpcode(0x4F); } 
//
//TEST(Instructions, OP0x50) { TestOpcode(0x50); } 
//TEST(Instructions, OP0x51) { TestOpcode(0x51); } 
//TEST(Instructions, OP0x52) { TestOpcode(0x52); } 
//TEST(Instructions, OP0x53) { TestOpcode(0x53); } 
//TEST(Instructions, OP0x54) { TestOpcode(0x54); } 
//TEST(Instructions, OP0x55) { TestOpcode(0x55); } 
//TEST(Instructions, OP0x56) { TestOpcode(0x56); } 
//TEST(Instructions, OP0x57) { TestOpcode(0x57); } 
//TEST(Instructions, OP0x58) { TestOpcode(0x58); } 
//TEST(Instructions, OP0x59) { TestOpcode(0x59); } 
//TEST(Instructions, OP0x5A) { TestOpcode(0x5A); } 
//TEST(Instructions, OP0x5B) { TestOpcode(0x5B); } 
//TEST(Instructions, OP0x5C) { TestOpcode(0x5C); } 
//TEST(Instructions, OP0x5D) { TestOpcode(0x5D); } 
//TEST(Instructions, OP0x5E) { TestOpcode(0x5E); } 
//TEST(Instructions, OP0x5F) { TestOpcode(0x5F); } 
//
//TEST(Instructions, OP0x60) { TestOpcode(0x60); } 
//TEST(Instructions, OP0x61) { TestOpcode(0x61); } 
//TEST(Instructions, OP0x62) { TestOpcode(0x62); } 
//TEST(Instructions, OP0x63) { TestOpcode(0x63); } 
//TEST(Instructions, OP0x64) { TestOpcode(0x64); } 
//TEST(Instructions, OP0x65) { TestOpcode(0x65); } 
//TEST(Instructions, OP0x66) { TestOpcode(0x66); } 
//TEST(Instructions, OP0x67) { TestOpcode(0x67); } 
//TEST(Instructions, OP0x68) { TestOpcode(0x68); } 
//TEST(Instructions, OP0x69) { TestOpcode(0x69); } 
//TEST(Instructions, OP0x6A) { TestOpcode(0x6A); } 
//TEST(Instructions, OP0x6B) { TestOpcode(0x6B); } 
//TEST(Instructions, OP0x6C) { TestOpcode(0x6C); } 
//TEST(Instructions, OP0x6D) { TestOpcode(0x6D); } 
//TEST(Instructions, OP0x6E) { TestOpcode(0x6E); } 
//TEST(Instructions, OP0x6F) { TestOpcode(0x6F); } 
//
//TEST(Instructions, OP0x80) { TestOpcode(0x80); } 
//TEST(Instructions, OP0x81) { TestOpcode(0x81); } 
//TEST(Instructions, OP0x82) { TestOpcode(0x82); } 
//TEST(Instructions, OP0x83) { TestOpcode(0x83); } 
//TEST(Instructions, OP0x84) { TestOpcode(0x84); } 
//TEST(Instructions, OP0x85) { TestOpcode(0x85); } 
//TEST(Instructions, OP0x86) { TestOpcode(0x86); } 
//TEST(Instructions, OP0x87) { TestOpcode(0x87); } 
//TEST(Instructions, OP0x88) { TestOpcode(0x88); } 
//TEST(Instructions, OP0x89) { TestOpcode(0x89); } 
//TEST(Instructions, OP0x8A) { TestOpcode(0x8A); } 
//TEST(Instructions, OP0x8B) { TestOpcode(0x8B); } 
//TEST(Instructions, OP0x8C) { TestOpcode(0x8C); } 
//TEST(Instructions, OP0x8D) { TestOpcode(0x8D); } 
//TEST(Instructions, OP0x8E) { TestOpcode(0x8E); } 
//TEST(Instructions, OP0x8F) { TestOpcode(0x8F); } 
//
//TEST(Instructions, OP0x90) { TestOpcode(0x90); } 
//TEST(Instructions, OP0x91) { TestOpcode(0x91); } 
//TEST(Instructions, OP0x92) { TestOpcode(0x92); } 
//TEST(Instructions, OP0x93) { TestOpcode(0x93); } 
//TEST(Instructions, OP0x94) { TestOpcode(0x94); } 
//TEST(Instructions, OP0x95) { TestOpcode(0x95); } 
//TEST(Instructions, OP0x96) { TestOpcode(0x96); } 
//TEST(Instructions, OP0x97) { TestOpcode(0x97); } 
//TEST(Instructions, OP0x98) { TestOpcode(0x98); } 
//TEST(Instructions, OP0x99) { TestOpcode(0x99); } 
//TEST(Instructions, OP0x9A) { TestOpcode(0x9A); } 
//TEST(Instructions, OP0x9B) { TestOpcode(0x9B); } 
//TEST(Instructions, OP0x9C) { TestOpcode(0x9C); } 
//TEST(Instructions, OP0x9D) { TestOpcode(0x9D); } 
//TEST(Instructions, OP0x9E) { TestOpcode(0x9E); } 
//TEST(Instructions, OP0x9F) { TestOpcode(0x9F); } 
//
//TEST(Instructions, OP0xA0) { TestOpcode(0xA0); } 
//TEST(Instructions, OP0xA1) { TestOpcode(0xA1); } 
//TEST(Instructions, OP0xA2) { TestOpcode(0xA2); } 
//TEST(Instructions, OP0xA3) { TestOpcode(0xA3); } 
//TEST(Instructions, OP0xA4) { TestOpcode(0xA4); } 
//TEST(Instructions, OP0xA5) { TestOpcode(0xA5); } 
//TEST(Instructions, OP0xA6) { TestOpcode(0xA6); } 
//TEST(Instructions, OP0xA7) { TestOpcode(0xA7); } 
//TEST(Instructions, OP0xA8) { TestOpcode(0xA8); } 
//TEST(Instructions, OP0xA9) { TestOpcode(0xA9); } 
//TEST(Instructions, OP0xAA) { TestOpcode(0xAA); } 
//TEST(Instructions, OP0xAB) { TestOpcode(0xAB); } 
//TEST(Instructions, OP0xAC) { TestOpcode(0xAC); } 
//TEST(Instructions, OP0xAD) { TestOpcode(0xAD); } 
//TEST(Instructions, OP0xAE) { TestOpcode(0xAE); } 
//TEST(Instructions, OP0xAF) { TestOpcode(0xAF); } 
//
//TEST(Instructions, OP0xB0) { TestOpcode(0xB0); } 
//TEST(Instructions, OP0xB1) { TestOpcode(0xB1); } 
//TEST(Instructions, OP0xB2) { TestOpcode(0xB2); } 
//TEST(Instructions, OP0xB3) { TestOpcode(0xB3); } 
//TEST(Instructions, OP0xB4) { TestOpcode(0xB4); } 
//TEST(Instructions, OP0xB5) { TestOpcode(0xB5); } 
//TEST(Instructions, OP0xB6) { TestOpcode(0xB6); } 
//TEST(Instructions, OP0xB7) { TestOpcode(0xB7); } 
//TEST(Instructions, OP0xB8) { TestOpcode(0xB8); } 
//TEST(Instructions, OP0xB9) { TestOpcode(0xB9); } 
//TEST(Instructions, OP0xBA) { TestOpcode(0xBA); } 
//TEST(Instructions, OP0xBB) { TestOpcode(0xBB); } 
//TEST(Instructions, OP0xBC) { TestOpcode(0xBC); } 
//TEST(Instructions, OP0xBD) { TestOpcode(0xBD); } 
//TEST(Instructions, OP0xBE) { TestOpcode(0xBE); } 
//TEST(Instructions, OP0xBF) { TestOpcode(0xBF); } 
//
//TEST(Instructions, OP0xC0) { TestOpcode(0xC0); } 
//TEST(Instructions, OP0xC1) { TestOpcode(0xC1); } 
//TEST(Instructions, OP0xC2) { TestOpcode(0xC2); } 
//TEST(Instructions, OP0xC3) { TestOpcode(0xC3); } 
//TEST(Instructions, OP0xC4) { TestOpcode(0xC4); } 
//TEST(Instructions, OP0xC5) { TestOpcode(0xC5); } 
//TEST(Instructions, OP0xC6) { TestOpcode(0xC6); } 
//TEST(Instructions, OP0xC7) { TestOpcode(0xC7); } 
//TEST(Instructions, OP0xC8) { TestOpcode(0xC8); } 
//TEST(Instructions, OP0xC9) { TestOpcode(0xC9); } 
//TEST(Instructions, OP0xCA) { TestOpcode(0xCA); } 
//// 0xCB - Tested seperately
//TEST(Instructions, OP0xCC) { TestOpcode(0xCC); } 
//TEST(Instructions, OP0xCD) { TestOpcode(0xCD); } 
//TEST(Instructions, OP0xCE) { TestOpcode(0xCE); } 
//TEST(Instructions, OP0xCF) { TestOpcode(0xCF); }
//
//TEST(Instructions, OP0xD0) { TestOpcode(0xD0); } 
//TEST(Instructions, OP0xD1) { TestOpcode(0xD1); } 
//TEST(Instructions, OP0xD2) { TestOpcode(0xD2); } 
//// 0xD3 - Illegal
//TEST(Instructions, OP0xD4) { TestOpcode(0xD4); } 
//TEST(Instructions, OP0xD5) { TestOpcode(0xD5); } 
//TEST(Instructions, OP0xD6) { TestOpcode(0xD6); } 
//TEST(Instructions, OP0xD7) { TestOpcode(0xD7); }
//TEST(Instructions, OP0xD8) { TestOpcode(0xD8); } 
//TEST(Instructions, OP0xD9) { TestOpcode(0xD9); } 
//TEST(Instructions, OP0xDA) { TestOpcode(0xDA); } 
//// 0xDB - Illegal
//TEST(Instructions, OP0xDC) { TestOpcode(0xDC); } 
//// 0xDD - Illegal
//TEST(Instructions, OP0xDE) { TestOpcode(0xDE); } 
//TEST(Instructions, OP0xDF) { TestOpcode(0xDF); }
//
//TEST(Instructions, OP0xE0) { TestOpcode(0xE0); } 
//TEST(Instructions, OP0xE1) { TestOpcode(0xE1); } 
//TEST(Instructions, OP0xE2) { TestOpcode(0xE2); } 
//// 0xE3 - Illegal
//// 0xE4 - Illegal
//TEST(Instructions, OP0xE5) { TestOpcode(0xE5); } 
//TEST(Instructions, OP0xE6) { TestOpcode(0xE6); } 
//TEST(Instructions, OP0xE7) { TestOpcode(0xE7); } 
//TEST(Instructions, OP0xE8) { TestOpcode(0xE8); }
//TEST(Instructions, OP0xE9) { TestOpcode(0xE9); } 
//TEST(Instructions, OP0xEA) { TestOpcode(0xEA); } 
//// 0xEB - Illegal
//// 0xEC - Illegal
//// 0xED - Illegal
//TEST(Instructions, OP0xEE) { TestOpcode(0xEE); } 
//TEST(Instructions, OP0xEF) { TestOpcode(0xEF); } 
//
//TEST(Instructions, OP0xF0) { TestOpcode(0xF0); } 
//TEST(Instructions, OP0xF1) { TestOpcode(0xF1); }
//TEST(Instructions, OP0xF2) { TestOpcode(0xF2); } 
//TEST(Instructions, OP0xF3) { TestOpcode(0xF3); } 
//// 0xF4 - Illegal 
//TEST(Instructions, OP0xF5) { TestOpcode(0xF5); } 
//TEST(Instructions, OP0xF6) { TestOpcode(0xF6); } 
//TEST(Instructions, OP0xF7) { TestOpcode(0xF7); } 
//TEST(Instructions, OP0xF8) { TestOpcode(0xF8); }
//TEST(Instructions, OP0xF9) { TestOpcode(0xF9); } 
//TEST(Instructions, OP0xFA) { TestOpcode(0xFA); } 
//TEST(Instructions, OP0xFB) { TestOpcode(0xFB); } 
//// 0xFC - Illegal 
//// 0xFD - Illegal 
//TEST(Instructions, OP0xFE) { TestOpcode(0xFE); } 
//TEST(Instructions, OP0xFF) { TestOpcode(0xFF); } 
#endif // ENABLED