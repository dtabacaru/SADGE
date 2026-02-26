#include "Cpu.h"

#include "gtest/gtest.h"
#include "json/json.h"

#include <fstream>

//#define ENABLED

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

void TestCbOpcode(uint8_t opcode)
{
  std::string test_file_path = std::format("sm83-main\\v1\\cb {:02x}.json", opcode);
  std::ifstream file(test_file_path, std::ifstream::binary);

  EXPECT_TRUE(file.is_open());

  // Prepare to parse
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

    uint16_t address = initial["b"].asUInt() << 8 | initial["c"].asUInt();

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
  }
}

#ifdef ENABLED
TEST(ExtendedInstructions, OP0xCB00) { TestCbOpcode(0x00); }
TEST(ExtendedInstructions, OP0xCB01) { TestCbOpcode(0x01); }
TEST(ExtendedInstructions, OP0xCB02) { TestCbOpcode(0x02); }
TEST(ExtendedInstructions, OP0xCB03) { TestCbOpcode(0x03); }
TEST(ExtendedInstructions, OP0xCB04) { TestCbOpcode(0x04); }
TEST(ExtendedInstructions, OP0xCB05) { TestCbOpcode(0x05); }
TEST(ExtendedInstructions, OP0xCB06) { TestCbOpcode(0x06); }
TEST(ExtendedInstructions, OP0xCB07) { TestCbOpcode(0x07); }
TEST(ExtendedInstructions, OP0xCB08) { TestCbOpcode(0x08); }
TEST(ExtendedInstructions, OP0xCB09) { TestCbOpcode(0x09); }
TEST(ExtendedInstructions, OP0xCB0A) { TestCbOpcode(0x0A); }
TEST(ExtendedInstructions, OP0xCB0B) { TestCbOpcode(0x0B); }
TEST(ExtendedInstructions, OP0xCB0C) { TestCbOpcode(0x0C); }
TEST(ExtendedInstructions, OP0xCB0D) { TestCbOpcode(0x0D); }
TEST(ExtendedInstructions, OP0xCB0E) { TestCbOpcode(0x0E); }
TEST(ExtendedInstructions, OP0xCB0F) { TestCbOpcode(0x0F); }

TEST(ExtendedInstructions, OP0xCB10) { TestCbOpcode(0x10); }
TEST(ExtendedInstructions, OP0xCB11) { TestCbOpcode(0x11); }
TEST(ExtendedInstructions, OP0xCB12) { TestCbOpcode(0x12); }
TEST(ExtendedInstructions, OP0xCB13) { TestCbOpcode(0x13); }
TEST(ExtendedInstructions, OP0xCB14) { TestCbOpcode(0x14); }
TEST(ExtendedInstructions, OP0xCB15) { TestCbOpcode(0x15); }
TEST(ExtendedInstructions, OP0xCB16) { TestCbOpcode(0x16); }
TEST(ExtendedInstructions, OP0xCB17) { TestCbOpcode(0x17); }
TEST(ExtendedInstructions, OP0xCB18) { TestCbOpcode(0x18); }
TEST(ExtendedInstructions, OP0xCB19) { TestCbOpcode(0x19); }
TEST(ExtendedInstructions, OP0xCB1A) { TestCbOpcode(0x1A); }
TEST(ExtendedInstructions, OP0xCB1B) { TestCbOpcode(0x1B); }
TEST(ExtendedInstructions, OP0xCB1C) { TestCbOpcode(0x1C); }
TEST(ExtendedInstructions, OP0xCB1D) { TestCbOpcode(0x1D); }
TEST(ExtendedInstructions, OP0xCB1E) { TestCbOpcode(0x1E); }
TEST(ExtendedInstructions, OP0xCB1F) { TestCbOpcode(0x1F); }

TEST(ExtendedInstructions, OP0xCB20) { TestCbOpcode(0x20); }
TEST(ExtendedInstructions, OP0xCB21) { TestCbOpcode(0x21); }
TEST(ExtendedInstructions, OP0xCB22) { TestCbOpcode(0x22); }
TEST(ExtendedInstructions, OP0xCB23) { TestCbOpcode(0x23); }
TEST(ExtendedInstructions, OP0xCB24) { TestCbOpcode(0x24); }
TEST(ExtendedInstructions, OP0xCB25) { TestCbOpcode(0x25); }
TEST(ExtendedInstructions, OP0xCB26) { TestCbOpcode(0x26); }
TEST(ExtendedInstructions, OP0xCB27) { TestCbOpcode(0x27); }
TEST(ExtendedInstructions, OP0xCB28) { TestCbOpcode(0x28); }
TEST(ExtendedInstructions, OP0xCB29) { TestCbOpcode(0x29); }
TEST(ExtendedInstructions, OP0xCB2A) { TestCbOpcode(0x2A); }
TEST(ExtendedInstructions, OP0xCB2B) { TestCbOpcode(0x2B); }
TEST(ExtendedInstructions, OP0xCB2C) { TestCbOpcode(0x2C); }
TEST(ExtendedInstructions, OP0xCB2D) { TestCbOpcode(0x2D); }
TEST(ExtendedInstructions, OP0xCB2E) { TestCbOpcode(0x2E); }
TEST(ExtendedInstructions, OP0xCB2F) { TestCbOpcode(0x2F); }

TEST(ExtendedInstructions, OP0xCB30) { TestCbOpcode(0x30); }
TEST(ExtendedInstructions, OP0xCB31) { TestCbOpcode(0x31); }
TEST(ExtendedInstructions, OP0xCB32) { TestCbOpcode(0x32); }
TEST(ExtendedInstructions, OP0xCB33) { TestCbOpcode(0x33); }
TEST(ExtendedInstructions, OP0xCB34) { TestCbOpcode(0x34); }
TEST(ExtendedInstructions, OP0xCB35) { TestCbOpcode(0x35); }
TEST(ExtendedInstructions, OP0xCB36) { TestCbOpcode(0x36); }
TEST(ExtendedInstructions, OP0xCB37) { TestCbOpcode(0x37); }
TEST(ExtendedInstructions, OP0xCB38) { TestCbOpcode(0x38); }
TEST(ExtendedInstructions, OP0xCB39) { TestCbOpcode(0x39); }
TEST(ExtendedInstructions, OP0xCB3A) { TestCbOpcode(0x3A); }
TEST(ExtendedInstructions, OP0xCB3B) { TestCbOpcode(0x3B); }
TEST(ExtendedInstructions, OP0xCB3C) { TestCbOpcode(0x3C); }
TEST(ExtendedInstructions, OP0xCB3D) { TestCbOpcode(0x3D); }
TEST(ExtendedInstructions, OP0xCB3E) { TestCbOpcode(0x3E); }
TEST(ExtendedInstructions, OP0xCB3F) { TestCbOpcode(0x3F); }

TEST(ExtendedInstructions, OP0xCB40) { TestCbOpcode(0x40); }
TEST(ExtendedInstructions, OP0xCB41) { TestCbOpcode(0x41); }
TEST(ExtendedInstructions, OP0xCB42) { TestCbOpcode(0x42); }
TEST(ExtendedInstructions, OP0xCB43) { TestCbOpcode(0x43); }
TEST(ExtendedInstructions, OP0xCB44) { TestCbOpcode(0x44); }
TEST(ExtendedInstructions, OP0xCB45) { TestCbOpcode(0x45); }
TEST(ExtendedInstructions, OP0xCB46) { TestCbOpcode(0x46); }
TEST(ExtendedInstructions, OP0xCB47) { TestCbOpcode(0x47); }
TEST(ExtendedInstructions, OP0xCB48) { TestCbOpcode(0x48); }
TEST(ExtendedInstructions, OP0xCB49) { TestCbOpcode(0x49); }
TEST(ExtendedInstructions, OP0xCB4A) { TestCbOpcode(0x4A); }
TEST(ExtendedInstructions, OP0xCB4B) { TestCbOpcode(0x4B); }
TEST(ExtendedInstructions, OP0xCB4C) { TestCbOpcode(0x4C); }
TEST(ExtendedInstructions, OP0xCB4D) { TestCbOpcode(0x4D); }
TEST(ExtendedInstructions, OP0xCB4E) { TestCbOpcode(0x4E); }
TEST(ExtendedInstructions, OP0xCB4F) { TestCbOpcode(0x4F); }

TEST(ExtendedInstructions, OP0xCB50) { TestCbOpcode(0x50); }
TEST(ExtendedInstructions, OP0xCB51) { TestCbOpcode(0x51); }
TEST(ExtendedInstructions, OP0xCB52) { TestCbOpcode(0x52); }
TEST(ExtendedInstructions, OP0xCB53) { TestCbOpcode(0x53); }
TEST(ExtendedInstructions, OP0xCB54) { TestCbOpcode(0x54); }
TEST(ExtendedInstructions, OP0xCB55) { TestCbOpcode(0x55); }
TEST(ExtendedInstructions, OP0xCB56) { TestCbOpcode(0x56); }
TEST(ExtendedInstructions, OP0xCB57) { TestCbOpcode(0x57); }
TEST(ExtendedInstructions, OP0xCB58) { TestCbOpcode(0x58); }
TEST(ExtendedInstructions, OP0xCB59) { TestCbOpcode(0x59); }
TEST(ExtendedInstructions, OP0xCB5A) { TestCbOpcode(0x5A); }
TEST(ExtendedInstructions, OP0xCB5B) { TestCbOpcode(0x5B); }
TEST(ExtendedInstructions, OP0xCB5C) { TestCbOpcode(0x5C); }
TEST(ExtendedInstructions, OP0xCB5D) { TestCbOpcode(0x5D); }
TEST(ExtendedInstructions, OP0xCB5E) { TestCbOpcode(0x5E); }
TEST(ExtendedInstructions, OP0xCB5F) { TestCbOpcode(0x5F); }

TEST(ExtendedInstructions, OP0xCB60) { TestCbOpcode(0x60); }
TEST(ExtendedInstructions, OP0xCB61) { TestCbOpcode(0x61); }
TEST(ExtendedInstructions, OP0xCB62) { TestCbOpcode(0x62); }
TEST(ExtendedInstructions, OP0xCB63) { TestCbOpcode(0x63); }
TEST(ExtendedInstructions, OP0xCB64) { TestCbOpcode(0x64); }
TEST(ExtendedInstructions, OP0xCB65) { TestCbOpcode(0x65); }
TEST(ExtendedInstructions, OP0xCB66) { TestCbOpcode(0x66); }
TEST(ExtendedInstructions, OP0xCB67) { TestCbOpcode(0x67); }
TEST(ExtendedInstructions, OP0xCB68) { TestCbOpcode(0x68); }
TEST(ExtendedInstructions, OP0xCB69) { TestCbOpcode(0x69); }
TEST(ExtendedInstructions, OP0xCB6A) { TestCbOpcode(0x6A); }
TEST(ExtendedInstructions, OP0xCB6B) { TestCbOpcode(0x6B); }
TEST(ExtendedInstructions, OP0xCB6C) { TestCbOpcode(0x6C); }
TEST(ExtendedInstructions, OP0xCB6D) { TestCbOpcode(0x6D); }
TEST(ExtendedInstructions, OP0xCB6E) { TestCbOpcode(0x6E); }
TEST(ExtendedInstructions, OP0xCB6F) { TestCbOpcode(0x6F); }

TEST(ExtendedInstructions, OP0xCB70) { TestCbOpcode(0x70); }
TEST(ExtendedInstructions, OP0xCB71) { TestCbOpcode(0x71); }
TEST(ExtendedInstructions, OP0xCB72) { TestCbOpcode(0x72); }
TEST(ExtendedInstructions, OP0xCB73) { TestCbOpcode(0x73); }
TEST(ExtendedInstructions, OP0xCB74) { TestCbOpcode(0x74); }
TEST(ExtendedInstructions, OP0xCB75) { TestCbOpcode(0x75); }
TEST(ExtendedInstructions, OP0xCB76) { TestCbOpcode(0x76); }
TEST(ExtendedInstructions, OP0xCB77) { TestCbOpcode(0x77); }
TEST(ExtendedInstructions, OP0xCB78) { TestCbOpcode(0x78); }
TEST(ExtendedInstructions, OP0xCB79) { TestCbOpcode(0x79); }
TEST(ExtendedInstructions, OP0xCB7A) { TestCbOpcode(0x7A); }
TEST(ExtendedInstructions, OP0xCB7B) { TestCbOpcode(0x7B); }
TEST(ExtendedInstructions, OP0xCB7C) { TestCbOpcode(0x7C); }
TEST(ExtendedInstructions, OP0xCB7D) { TestCbOpcode(0x7D); }
TEST(ExtendedInstructions, OP0xCB7E) { TestCbOpcode(0x7E); }
TEST(ExtendedInstructions, OP0xCB7F) { TestCbOpcode(0x7F); }

TEST(ExtendedInstructions, OP0xCB80) { TestCbOpcode(0x80); }
TEST(ExtendedInstructions, OP0xCB81) { TestCbOpcode(0x81); }
TEST(ExtendedInstructions, OP0xCB82) { TestCbOpcode(0x82); }
TEST(ExtendedInstructions, OP0xCB83) { TestCbOpcode(0x83); }
TEST(ExtendedInstructions, OP0xCB84) { TestCbOpcode(0x84); }
TEST(ExtendedInstructions, OP0xCB85) { TestCbOpcode(0x85); }
TEST(ExtendedInstructions, OP0xCB86) { TestCbOpcode(0x86); }
TEST(ExtendedInstructions, OP0xCB87) { TestCbOpcode(0x87); }
TEST(ExtendedInstructions, OP0xCB88) { TestCbOpcode(0x88); }
TEST(ExtendedInstructions, OP0xCB89) { TestCbOpcode(0x89); }
TEST(ExtendedInstructions, OP0xCB8A) { TestCbOpcode(0x8A); }
TEST(ExtendedInstructions, OP0xCB8B) { TestCbOpcode(0x8B); }
TEST(ExtendedInstructions, OP0xCB8C) { TestCbOpcode(0x8C); }
TEST(ExtendedInstructions, OP0xCB8D) { TestCbOpcode(0x8D); }
TEST(ExtendedInstructions, OP0xCB8E) { TestCbOpcode(0x8E); }
TEST(ExtendedInstructions, OP0xCB8F) { TestCbOpcode(0x8F); }

TEST(ExtendedInstructions, OP0xCB90) { TestCbOpcode(0x90); }
TEST(ExtendedInstructions, OP0xCB91) { TestCbOpcode(0x91); }
TEST(ExtendedInstructions, OP0xCB92) { TestCbOpcode(0x92); }
TEST(ExtendedInstructions, OP0xCB93) { TestCbOpcode(0x93); }
TEST(ExtendedInstructions, OP0xCB94) { TestCbOpcode(0x94); }
TEST(ExtendedInstructions, OP0xCB95) { TestCbOpcode(0x95); }
TEST(ExtendedInstructions, OP0xCB96) { TestCbOpcode(0x96); }
TEST(ExtendedInstructions, OP0xCB97) { TestCbOpcode(0x97); }
TEST(ExtendedInstructions, OP0xCB98) { TestCbOpcode(0x98); }
TEST(ExtendedInstructions, OP0xCB99) { TestCbOpcode(0x99); }
TEST(ExtendedInstructions, OP0xCB9A) { TestCbOpcode(0x9A); }
TEST(ExtendedInstructions, OP0xCB9B) { TestCbOpcode(0x9B); }
TEST(ExtendedInstructions, OP0xCB9C) { TestCbOpcode(0x9C); }
TEST(ExtendedInstructions, OP0xCB9D) { TestCbOpcode(0x9D); }
TEST(ExtendedInstructions, OP0xCB9E) { TestCbOpcode(0x9E); }
TEST(ExtendedInstructions, OP0xCB9F) { TestCbOpcode(0x9F); }

TEST(ExtendedInstructions, OP0xCBA0) { TestCbOpcode(0xA0); }
TEST(ExtendedInstructions, OP0xCBA1) { TestCbOpcode(0xA1); }
TEST(ExtendedInstructions, OP0xCBA2) { TestCbOpcode(0xA2); }
TEST(ExtendedInstructions, OP0xCBA3) { TestCbOpcode(0xA3); }
TEST(ExtendedInstructions, OP0xCBA4) { TestCbOpcode(0xA4); }
TEST(ExtendedInstructions, OP0xCBA5) { TestCbOpcode(0xA5); }
TEST(ExtendedInstructions, OP0xCBA6) { TestCbOpcode(0xA6); }
TEST(ExtendedInstructions, OP0xCBA7) { TestCbOpcode(0xA7); }
TEST(ExtendedInstructions, OP0xCBA8) { TestCbOpcode(0xA8); }
TEST(ExtendedInstructions, OP0xCBA9) { TestCbOpcode(0xA9); }
TEST(ExtendedInstructions, OP0xCBAA) { TestCbOpcode(0xAA); }
TEST(ExtendedInstructions, OP0xCBAB) { TestCbOpcode(0xAB); }
TEST(ExtendedInstructions, OP0xCBAC) { TestCbOpcode(0xAC); }
TEST(ExtendedInstructions, OP0xCBAD) { TestCbOpcode(0xAD); }
TEST(ExtendedInstructions, OP0xCBAE) { TestCbOpcode(0xAE); }
TEST(ExtendedInstructions, OP0xCBAF) { TestCbOpcode(0xAF); }

TEST(ExtendedInstructions, OP0xCBB0) { TestCbOpcode(0xB0); }
TEST(ExtendedInstructions, OP0xCBB1) { TestCbOpcode(0xB1); }
TEST(ExtendedInstructions, OP0xCBB2) { TestCbOpcode(0xB2); }
TEST(ExtendedInstructions, OP0xCBB3) { TestCbOpcode(0xB3); }
TEST(ExtendedInstructions, OP0xCBB4) { TestCbOpcode(0xB4); }
TEST(ExtendedInstructions, OP0xCBB5) { TestCbOpcode(0xB5); }
TEST(ExtendedInstructions, OP0xCBB6) { TestCbOpcode(0xB6); }
TEST(ExtendedInstructions, OP0xCBB7) { TestCbOpcode(0xB7); }
TEST(ExtendedInstructions, OP0xCBB8) { TestCbOpcode(0xB8); }
TEST(ExtendedInstructions, OP0xCBB9) { TestCbOpcode(0xB9); }
TEST(ExtendedInstructions, OP0xCBBA) { TestCbOpcode(0xBA); }
TEST(ExtendedInstructions, OP0xCBBB) { TestCbOpcode(0xBB); }
TEST(ExtendedInstructions, OP0xCBBC) { TestCbOpcode(0xBC); }
TEST(ExtendedInstructions, OP0xCBBD) { TestCbOpcode(0xBD); }
TEST(ExtendedInstructions, OP0xCBBE) { TestCbOpcode(0xBE); }
TEST(ExtendedInstructions, OP0xCBBF) { TestCbOpcode(0xBF); }

TEST(ExtendedInstructions, OP0xCBC0) { TestCbOpcode(0xC0); }
TEST(ExtendedInstructions, OP0xCBC1) { TestCbOpcode(0xC1); }
TEST(ExtendedInstructions, OP0xCBC2) { TestCbOpcode(0xC2); }
TEST(ExtendedInstructions, OP0xCBC3) { TestCbOpcode(0xC3); }
TEST(ExtendedInstructions, OP0xCBC4) { TestCbOpcode(0xC4); }
TEST(ExtendedInstructions, OP0xCBC5) { TestCbOpcode(0xC5); }
TEST(ExtendedInstructions, OP0xCBC6) { TestCbOpcode(0xC6); }
TEST(ExtendedInstructions, OP0xCBC7) { TestCbOpcode(0xC7); }
TEST(ExtendedInstructions, OP0xCBC8) { TestCbOpcode(0xC8); }
TEST(ExtendedInstructions, OP0xCBC9) { TestCbOpcode(0xC9); }
TEST(ExtendedInstructions, OP0xCBCA) { TestCbOpcode(0xCA); }
TEST(ExtendedInstructions, OP0xCBCB) { TestCbOpcode(0xCB); }
TEST(ExtendedInstructions, OP0xCBCC) { TestCbOpcode(0xCC); }
TEST(ExtendedInstructions, OP0xCBCD) { TestCbOpcode(0xCD); }
TEST(ExtendedInstructions, OP0xCBCE) { TestCbOpcode(0xCE); }
TEST(ExtendedInstructions, OP0xCBCF) { TestCbOpcode(0xCF); }

TEST(ExtendedInstructions, OP0xCBD0) { TestCbOpcode(0xD0); }
TEST(ExtendedInstructions, OP0xCBD1) { TestCbOpcode(0xD1); }
TEST(ExtendedInstructions, OP0xCBD2) { TestCbOpcode(0xD2); }
TEST(ExtendedInstructions, OP0xCBD3) { TestCbOpcode(0xD3); }
TEST(ExtendedInstructions, OP0xCBD4) { TestCbOpcode(0xD4); }
TEST(ExtendedInstructions, OP0xCBD5) { TestCbOpcode(0xD5); }
TEST(ExtendedInstructions, OP0xCBD6) { TestCbOpcode(0xD6); }
TEST(ExtendedInstructions, OP0xCBD7) { TestCbOpcode(0xD7); }
TEST(ExtendedInstructions, OP0xCBD8) { TestCbOpcode(0xD8); }
TEST(ExtendedInstructions, OP0xCBD9) { TestCbOpcode(0xD9); }
TEST(ExtendedInstructions, OP0xCBDA) { TestCbOpcode(0xDA); }
TEST(ExtendedInstructions, OP0xCBDB) { TestCbOpcode(0xDB); }
TEST(ExtendedInstructions, OP0xCBDC) { TestCbOpcode(0xDC); }
TEST(ExtendedInstructions, OP0xCBDD) { TestCbOpcode(0xDD); }
TEST(ExtendedInstructions, OP0xCBDE) { TestCbOpcode(0xDE); }
TEST(ExtendedInstructions, OP0xCBDF) { TestCbOpcode(0xDF); }

TEST(ExtendedInstructions, OP0xCBE0) { TestCbOpcode(0xE0); }
TEST(ExtendedInstructions, OP0xCBE1) { TestCbOpcode(0xE1); }
TEST(ExtendedInstructions, OP0xCBE2) { TestCbOpcode(0xE2); }
TEST(ExtendedInstructions, OP0xCBE3) { TestCbOpcode(0xE3); }
TEST(ExtendedInstructions, OP0xCBE4) { TestCbOpcode(0xE4); }
TEST(ExtendedInstructions, OP0xCBE5) { TestCbOpcode(0xE5); }
TEST(ExtendedInstructions, OP0xCBE6) { TestCbOpcode(0xE6); }
TEST(ExtendedInstructions, OP0xCBE7) { TestCbOpcode(0xE7); }
TEST(ExtendedInstructions, OP0xCBE8) { TestCbOpcode(0xE8); }
TEST(ExtendedInstructions, OP0xCBE9) { TestCbOpcode(0xE9); }
TEST(ExtendedInstructions, OP0xCBEA) { TestCbOpcode(0xEA); }
TEST(ExtendedInstructions, OP0xCBEB) { TestCbOpcode(0xEB); }
TEST(ExtendedInstructions, OP0xCBEC) { TestCbOpcode(0xEC); }
TEST(ExtendedInstructions, OP0xCBED) { TestCbOpcode(0xED); }
TEST(ExtendedInstructions, OP0xCBEE) { TestCbOpcode(0xEE); }
TEST(ExtendedInstructions, OP0xCBEF) { TestCbOpcode(0xEF); }

TEST(ExtendedInstructions, OP0xCBF0) { TestCbOpcode(0xF0); }
TEST(ExtendedInstructions, OP0xCBF1) { TestCbOpcode(0xF1); }
TEST(ExtendedInstructions, OP0xCBF2) { TestCbOpcode(0xF2); }
TEST(ExtendedInstructions, OP0xCBF3) { TestCbOpcode(0xF3); }
TEST(ExtendedInstructions, OP0xCBF4) { TestCbOpcode(0xF4); }
TEST(ExtendedInstructions, OP0xCBF5) { TestCbOpcode(0xF5); }
TEST(ExtendedInstructions, OP0xCBF6) { TestCbOpcode(0xF6); }
TEST(ExtendedInstructions, OP0xCBF7) { TestCbOpcode(0xF7); }
TEST(ExtendedInstructions, OP0xCBF8) { TestCbOpcode(0xF8); }
TEST(ExtendedInstructions, OP0xCBF9) { TestCbOpcode(0xF9); }
TEST(ExtendedInstructions, OP0xCBFA) { TestCbOpcode(0xFA); }
TEST(ExtendedInstructions, OP0xCBFB) { TestCbOpcode(0xFB); }
TEST(ExtendedInstructions, OP0xCBFC) { TestCbOpcode(0xFC); }
TEST(ExtendedInstructions, OP0xCBFD) { TestCbOpcode(0xFD); }
TEST(ExtendedInstructions, OP0xCBFE) { TestCbOpcode(0xFE); }
TEST(ExtendedInstructions, OP0xCBFF) { TestCbOpcode(0xFF); }
#endif // ENABLED