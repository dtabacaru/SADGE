#include "SstCpu.h"

#include "gtest/gtest.h"
#include "json/json.h"

#include <fstream>

//#define ENABLED

static CpuInstructionTest cpu_;

static void TestCbOpcode(uint8_t opcode)
{
  std::string test_file_path = std::format("sm83-main\\v1\\cb {:02x}.json", opcode);
  std::ifstream file(test_file_path, std::ifstream::binary);

  EXPECT_TRUE(file.is_open());

  Json::Value root;
  Json::CharReaderBuilder builder;
  builder["collectComments"] = false;
  JSONCPP_STRING errs;

  bool parsed = Json::parseFromStream(builder, file, &root, &errs);
  EXPECT_TRUE(parsed);

  EXPECT_TRUE(root.isArray());

  for (const auto& test : root)
  {
    const auto& initial = test["initial"];

    cpu_.SetState(initial["pc"].asUInt(),
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

    cpu_.Main();

    const auto& final_ = test["final"];

    bool state_passed = cpu_.CheckState(final_["pc"].asUInt(),
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
    auto emulator_test_cycles = cpu_.GetTestCycles();

    EXPECT_EQ(test_cycles.size(), emulator_test_cycles.size());

    if (test_cycles.size() != emulator_test_cycles.size())
      return;

    for (uint32_t cycle_num = 0; cycle_num < test_cycles.size(); cycle_num += 1)
    {
      const auto& test_cycle = test_cycles[cycle_num];

      if (test_cycle[2].asString() == "---")
        continue;

      bool address_matched = test_cycle[0].asUInt() == cpu_.GetTestCycles()[cycle_num].address_bus;
      bool data_matched = test_cycle[1].asUInt() == cpu_.GetTestCycles()[cycle_num].data_bus;

      EXPECT_TRUE(address_matched) << test["name"].asString();
      EXPECT_TRUE(data_matched) << test["name"].asString();
    }
  }
}

#ifdef ENABLED
TEST(ExtendedInstructions, OP0xCB00) { TestCbOpcode(0x00); } // ok
TEST(ExtendedInstructions, OP0xCB01) { TestCbOpcode(0x01); } // ok
TEST(ExtendedInstructions, OP0xCB02) { TestCbOpcode(0x02); } // ok
TEST(ExtendedInstructions, OP0xCB03) { TestCbOpcode(0x03); } // ok
TEST(ExtendedInstructions, OP0xCB04) { TestCbOpcode(0x04); } // ok
TEST(ExtendedInstructions, OP0xCB05) { TestCbOpcode(0x05); } // ok
TEST(ExtendedInstructions, OP0xCB06) { TestCbOpcode(0x06); } // ok
TEST(ExtendedInstructions, OP0xCB07) { TestCbOpcode(0x07); } // ok
TEST(ExtendedInstructions, OP0xCB08) { TestCbOpcode(0x08); } // ok
TEST(ExtendedInstructions, OP0xCB09) { TestCbOpcode(0x09); } // ok
TEST(ExtendedInstructions, OP0xCB0A) { TestCbOpcode(0x0A); } // ok
TEST(ExtendedInstructions, OP0xCB0B) { TestCbOpcode(0x0B); } // ok
TEST(ExtendedInstructions, OP0xCB0C) { TestCbOpcode(0x0C); } // ok
TEST(ExtendedInstructions, OP0xCB0D) { TestCbOpcode(0x0D); } // ok
TEST(ExtendedInstructions, OP0xCB0E) { TestCbOpcode(0x0E); } // ok
TEST(ExtendedInstructions, OP0xCB0F) { TestCbOpcode(0x0F); } // ok

TEST(ExtendedInstructions, OP0xCB10) { TestCbOpcode(0x10); } // ok
TEST(ExtendedInstructions, OP0xCB11) { TestCbOpcode(0x11); } // ok
TEST(ExtendedInstructions, OP0xCB12) { TestCbOpcode(0x12); } // ok
TEST(ExtendedInstructions, OP0xCB13) { TestCbOpcode(0x13); } // ok
TEST(ExtendedInstructions, OP0xCB14) { TestCbOpcode(0x14); } // ok
TEST(ExtendedInstructions, OP0xCB15) { TestCbOpcode(0x15); } // ok
TEST(ExtendedInstructions, OP0xCB16) { TestCbOpcode(0x16); } // ok
TEST(ExtendedInstructions, OP0xCB17) { TestCbOpcode(0x17); } // ok
TEST(ExtendedInstructions, OP0xCB18) { TestCbOpcode(0x18); } // ok
TEST(ExtendedInstructions, OP0xCB19) { TestCbOpcode(0x19); } // ok
TEST(ExtendedInstructions, OP0xCB1A) { TestCbOpcode(0x1A); } // ok
TEST(ExtendedInstructions, OP0xCB1B) { TestCbOpcode(0x1B); } // ok
TEST(ExtendedInstructions, OP0xCB1C) { TestCbOpcode(0x1C); } // ok
TEST(ExtendedInstructions, OP0xCB1D) { TestCbOpcode(0x1D); } // ok
TEST(ExtendedInstructions, OP0xCB1E) { TestCbOpcode(0x1E); } // ok
TEST(ExtendedInstructions, OP0xCB1F) { TestCbOpcode(0x1F); } // ok

TEST(ExtendedInstructions, OP0xCB20) { TestCbOpcode(0x20); } // ok
TEST(ExtendedInstructions, OP0xCB21) { TestCbOpcode(0x21); } // ok
TEST(ExtendedInstructions, OP0xCB22) { TestCbOpcode(0x22); } // ok
TEST(ExtendedInstructions, OP0xCB23) { TestCbOpcode(0x23); } // ok
TEST(ExtendedInstructions, OP0xCB24) { TestCbOpcode(0x24); } // ok
TEST(ExtendedInstructions, OP0xCB25) { TestCbOpcode(0x25); } // ok
TEST(ExtendedInstructions, OP0xCB26) { TestCbOpcode(0x26); } // ok
TEST(ExtendedInstructions, OP0xCB27) { TestCbOpcode(0x27); } // ok
TEST(ExtendedInstructions, OP0xCB28) { TestCbOpcode(0x28); } // ok
TEST(ExtendedInstructions, OP0xCB29) { TestCbOpcode(0x29); } // ok
TEST(ExtendedInstructions, OP0xCB2A) { TestCbOpcode(0x2A); } // ok
TEST(ExtendedInstructions, OP0xCB2B) { TestCbOpcode(0x2B); } // ok
TEST(ExtendedInstructions, OP0xCB2C) { TestCbOpcode(0x2C); } // ok
TEST(ExtendedInstructions, OP0xCB2D) { TestCbOpcode(0x2D); } // ok
TEST(ExtendedInstructions, OP0xCB2E) { TestCbOpcode(0x2E); } // ok
TEST(ExtendedInstructions, OP0xCB2F) { TestCbOpcode(0x2F); } // ok

TEST(ExtendedInstructions, OP0xCB30) { TestCbOpcode(0x30); } // ok
TEST(ExtendedInstructions, OP0xCB31) { TestCbOpcode(0x31); } // ok
TEST(ExtendedInstructions, OP0xCB32) { TestCbOpcode(0x32); } // ok
TEST(ExtendedInstructions, OP0xCB33) { TestCbOpcode(0x33); } // ok
TEST(ExtendedInstructions, OP0xCB34) { TestCbOpcode(0x34); } // ok
TEST(ExtendedInstructions, OP0xCB35) { TestCbOpcode(0x35); } // ok
TEST(ExtendedInstructions, OP0xCB36) { TestCbOpcode(0x36); } // ok
TEST(ExtendedInstructions, OP0xCB37) { TestCbOpcode(0x37); } // ok
TEST(ExtendedInstructions, OP0xCB38) { TestCbOpcode(0x38); } // ok
TEST(ExtendedInstructions, OP0xCB39) { TestCbOpcode(0x39); } // ok
TEST(ExtendedInstructions, OP0xCB3A) { TestCbOpcode(0x3A); } // ok
TEST(ExtendedInstructions, OP0xCB3B) { TestCbOpcode(0x3B); } // ok
TEST(ExtendedInstructions, OP0xCB3C) { TestCbOpcode(0x3C); } // ok
TEST(ExtendedInstructions, OP0xCB3D) { TestCbOpcode(0x3D); } // ok
TEST(ExtendedInstructions, OP0xCB3E) { TestCbOpcode(0x3E); } // ok
TEST(ExtendedInstructions, OP0xCB3F) { TestCbOpcode(0x3F); } // ok

TEST(ExtendedInstructions, OP0xCB40) { TestCbOpcode(0x40); } // ok
TEST(ExtendedInstructions, OP0xCB41) { TestCbOpcode(0x41); } // ok
TEST(ExtendedInstructions, OP0xCB42) { TestCbOpcode(0x42); } // ok
TEST(ExtendedInstructions, OP0xCB43) { TestCbOpcode(0x43); } // ok
TEST(ExtendedInstructions, OP0xCB44) { TestCbOpcode(0x44); } // ok
TEST(ExtendedInstructions, OP0xCB45) { TestCbOpcode(0x45); } // ok
TEST(ExtendedInstructions, OP0xCB46) { TestCbOpcode(0x46); } // ok
TEST(ExtendedInstructions, OP0xCB47) { TestCbOpcode(0x47); } // ok
TEST(ExtendedInstructions, OP0xCB48) { TestCbOpcode(0x48); } // ok
TEST(ExtendedInstructions, OP0xCB49) { TestCbOpcode(0x49); } // ok
TEST(ExtendedInstructions, OP0xCB4A) { TestCbOpcode(0x4A); } // ok
TEST(ExtendedInstructions, OP0xCB4B) { TestCbOpcode(0x4B); } // ok
TEST(ExtendedInstructions, OP0xCB4C) { TestCbOpcode(0x4C); } // ok
TEST(ExtendedInstructions, OP0xCB4D) { TestCbOpcode(0x4D); } // ok
TEST(ExtendedInstructions, OP0xCB4E) { TestCbOpcode(0x4E); } // ok
TEST(ExtendedInstructions, OP0xCB4F) { TestCbOpcode(0x4F); } // ok

TEST(ExtendedInstructions, OP0xCB50) { TestCbOpcode(0x50); } // ok
TEST(ExtendedInstructions, OP0xCB51) { TestCbOpcode(0x51); } // ok
TEST(ExtendedInstructions, OP0xCB52) { TestCbOpcode(0x52); } // ok
TEST(ExtendedInstructions, OP0xCB53) { TestCbOpcode(0x53); } // ok
TEST(ExtendedInstructions, OP0xCB54) { TestCbOpcode(0x54); } // ok
TEST(ExtendedInstructions, OP0xCB55) { TestCbOpcode(0x55); } // ok
TEST(ExtendedInstructions, OP0xCB56) { TestCbOpcode(0x56); } // ok
TEST(ExtendedInstructions, OP0xCB57) { TestCbOpcode(0x57); } // ok
TEST(ExtendedInstructions, OP0xCB58) { TestCbOpcode(0x58); } // ok
TEST(ExtendedInstructions, OP0xCB59) { TestCbOpcode(0x59); } // ok
TEST(ExtendedInstructions, OP0xCB5A) { TestCbOpcode(0x5A); } // ok
TEST(ExtendedInstructions, OP0xCB5B) { TestCbOpcode(0x5B); } // ok
TEST(ExtendedInstructions, OP0xCB5C) { TestCbOpcode(0x5C); } // ok
TEST(ExtendedInstructions, OP0xCB5D) { TestCbOpcode(0x5D); } // ok
TEST(ExtendedInstructions, OP0xCB5E) { TestCbOpcode(0x5E); } // ok
TEST(ExtendedInstructions, OP0xCB5F) { TestCbOpcode(0x5F); } // ok

TEST(ExtendedInstructions, OP0xCB60) { TestCbOpcode(0x60); } // ok
TEST(ExtendedInstructions, OP0xCB61) { TestCbOpcode(0x61); } // ok
TEST(ExtendedInstructions, OP0xCB62) { TestCbOpcode(0x62); } // ok
TEST(ExtendedInstructions, OP0xCB63) { TestCbOpcode(0x63); } // ok
TEST(ExtendedInstructions, OP0xCB64) { TestCbOpcode(0x64); } // ok
TEST(ExtendedInstructions, OP0xCB65) { TestCbOpcode(0x65); } // ok
TEST(ExtendedInstructions, OP0xCB66) { TestCbOpcode(0x66); } // ok
TEST(ExtendedInstructions, OP0xCB67) { TestCbOpcode(0x67); } // ok
TEST(ExtendedInstructions, OP0xCB68) { TestCbOpcode(0x68); } // ok
TEST(ExtendedInstructions, OP0xCB69) { TestCbOpcode(0x69); } // ok
TEST(ExtendedInstructions, OP0xCB6A) { TestCbOpcode(0x6A); } // ok
TEST(ExtendedInstructions, OP0xCB6B) { TestCbOpcode(0x6B); } // ok
TEST(ExtendedInstructions, OP0xCB6C) { TestCbOpcode(0x6C); } // ok
TEST(ExtendedInstructions, OP0xCB6D) { TestCbOpcode(0x6D); } // ok
TEST(ExtendedInstructions, OP0xCB6E) { TestCbOpcode(0x6E); } // ok
TEST(ExtendedInstructions, OP0xCB6F) { TestCbOpcode(0x6F); } // ok

TEST(ExtendedInstructions, OP0xCB70) { TestCbOpcode(0x70); } // ok
TEST(ExtendedInstructions, OP0xCB71) { TestCbOpcode(0x71); } // ok
TEST(ExtendedInstructions, OP0xCB72) { TestCbOpcode(0x72); } // ok
TEST(ExtendedInstructions, OP0xCB73) { TestCbOpcode(0x73); } // ok
TEST(ExtendedInstructions, OP0xCB74) { TestCbOpcode(0x74); } // ok
TEST(ExtendedInstructions, OP0xCB75) { TestCbOpcode(0x75); } // ok
TEST(ExtendedInstructions, OP0xCB76) { TestCbOpcode(0x76); } // ok
TEST(ExtendedInstructions, OP0xCB77) { TestCbOpcode(0x77); } // ok
TEST(ExtendedInstructions, OP0xCB78) { TestCbOpcode(0x78); } // ok
TEST(ExtendedInstructions, OP0xCB79) { TestCbOpcode(0x79); } // ok
TEST(ExtendedInstructions, OP0xCB7A) { TestCbOpcode(0x7A); } // ok
TEST(ExtendedInstructions, OP0xCB7B) { TestCbOpcode(0x7B); } // ok
TEST(ExtendedInstructions, OP0xCB7C) { TestCbOpcode(0x7C); } // ok
TEST(ExtendedInstructions, OP0xCB7D) { TestCbOpcode(0x7D); } // ok
TEST(ExtendedInstructions, OP0xCB7E) { TestCbOpcode(0x7E); } // ok
TEST(ExtendedInstructions, OP0xCB7F) { TestCbOpcode(0x7F); } // ok

TEST(ExtendedInstructions, OP0xCB80) { TestCbOpcode(0x80); } // ok
TEST(ExtendedInstructions, OP0xCB81) { TestCbOpcode(0x81); } // ok
TEST(ExtendedInstructions, OP0xCB82) { TestCbOpcode(0x82); } // ok
TEST(ExtendedInstructions, OP0xCB83) { TestCbOpcode(0x83); } // ok
TEST(ExtendedInstructions, OP0xCB84) { TestCbOpcode(0x84); } // ok
TEST(ExtendedInstructions, OP0xCB85) { TestCbOpcode(0x85); } // ok
TEST(ExtendedInstructions, OP0xCB86) { TestCbOpcode(0x86); } // ok
TEST(ExtendedInstructions, OP0xCB87) { TestCbOpcode(0x87); } // ok
TEST(ExtendedInstructions, OP0xCB88) { TestCbOpcode(0x88); } // ok
TEST(ExtendedInstructions, OP0xCB89) { TestCbOpcode(0x89); } // ok
TEST(ExtendedInstructions, OP0xCB8A) { TestCbOpcode(0x8A); } // ok
TEST(ExtendedInstructions, OP0xCB8B) { TestCbOpcode(0x8B); } // ok
TEST(ExtendedInstructions, OP0xCB8C) { TestCbOpcode(0x8C); } // ok
TEST(ExtendedInstructions, OP0xCB8D) { TestCbOpcode(0x8D); } // ok
TEST(ExtendedInstructions, OP0xCB8E) { TestCbOpcode(0x8E); } // ok
TEST(ExtendedInstructions, OP0xCB8F) { TestCbOpcode(0x8F); } // ok

TEST(ExtendedInstructions, OP0xCB90) { TestCbOpcode(0x90); } // ok
TEST(ExtendedInstructions, OP0xCB91) { TestCbOpcode(0x91); } // ok
TEST(ExtendedInstructions, OP0xCB92) { TestCbOpcode(0x92); } // ok
TEST(ExtendedInstructions, OP0xCB93) { TestCbOpcode(0x93); } // ok
TEST(ExtendedInstructions, OP0xCB94) { TestCbOpcode(0x94); } // ok
TEST(ExtendedInstructions, OP0xCB95) { TestCbOpcode(0x95); } // ok
TEST(ExtendedInstructions, OP0xCB96) { TestCbOpcode(0x96); } // ok
TEST(ExtendedInstructions, OP0xCB97) { TestCbOpcode(0x97); } // ok
TEST(ExtendedInstructions, OP0xCB98) { TestCbOpcode(0x98); } // ok
TEST(ExtendedInstructions, OP0xCB99) { TestCbOpcode(0x99); } // ok
TEST(ExtendedInstructions, OP0xCB9A) { TestCbOpcode(0x9A); } // ok
TEST(ExtendedInstructions, OP0xCB9B) { TestCbOpcode(0x9B); } // ok
TEST(ExtendedInstructions, OP0xCB9C) { TestCbOpcode(0x9C); } // ok
TEST(ExtendedInstructions, OP0xCB9D) { TestCbOpcode(0x9D); } // ok
TEST(ExtendedInstructions, OP0xCB9E) { TestCbOpcode(0x9E); } // ok
TEST(ExtendedInstructions, OP0xCB9F) { TestCbOpcode(0x9F); } // ok

TEST(ExtendedInstructions, OP0xCBA0) { TestCbOpcode(0xA0); } // ok
TEST(ExtendedInstructions, OP0xCBA1) { TestCbOpcode(0xA1); } // ok
TEST(ExtendedInstructions, OP0xCBA2) { TestCbOpcode(0xA2); } // ok
TEST(ExtendedInstructions, OP0xCBA3) { TestCbOpcode(0xA3); } // ok
TEST(ExtendedInstructions, OP0xCBA4) { TestCbOpcode(0xA4); } // ok
TEST(ExtendedInstructions, OP0xCBA5) { TestCbOpcode(0xA5); } // ok
TEST(ExtendedInstructions, OP0xCBA6) { TestCbOpcode(0xA6); } // ok
TEST(ExtendedInstructions, OP0xCBA7) { TestCbOpcode(0xA7); } // ok
TEST(ExtendedInstructions, OP0xCBA8) { TestCbOpcode(0xA8); } // ok
TEST(ExtendedInstructions, OP0xCBA9) { TestCbOpcode(0xA9); } // ok
TEST(ExtendedInstructions, OP0xCBAA) { TestCbOpcode(0xAA); } // ok
TEST(ExtendedInstructions, OP0xCBAB) { TestCbOpcode(0xAB); } // ok
TEST(ExtendedInstructions, OP0xCBAC) { TestCbOpcode(0xAC); } // ok
TEST(ExtendedInstructions, OP0xCBAD) { TestCbOpcode(0xAD); } // ok
TEST(ExtendedInstructions, OP0xCBAE) { TestCbOpcode(0xAE); } // ok
TEST(ExtendedInstructions, OP0xCBAF) { TestCbOpcode(0xAF); } // ok

TEST(ExtendedInstructions, OP0xCBB0) { TestCbOpcode(0xB0); } // ok
TEST(ExtendedInstructions, OP0xCBB1) { TestCbOpcode(0xB1); } // ok
TEST(ExtendedInstructions, OP0xCBB2) { TestCbOpcode(0xB2); } // ok
TEST(ExtendedInstructions, OP0xCBB3) { TestCbOpcode(0xB3); } // ok
TEST(ExtendedInstructions, OP0xCBB4) { TestCbOpcode(0xB4); } // ok
TEST(ExtendedInstructions, OP0xCBB5) { TestCbOpcode(0xB5); } // ok
TEST(ExtendedInstructions, OP0xCBB6) { TestCbOpcode(0xB6); } // ok
TEST(ExtendedInstructions, OP0xCBB7) { TestCbOpcode(0xB7); } // ok
TEST(ExtendedInstructions, OP0xCBB8) { TestCbOpcode(0xB8); } // ok
TEST(ExtendedInstructions, OP0xCBB9) { TestCbOpcode(0xB9); } // ok
TEST(ExtendedInstructions, OP0xCBBA) { TestCbOpcode(0xBA); } // ok
TEST(ExtendedInstructions, OP0xCBBB) { TestCbOpcode(0xBB); } // ok
TEST(ExtendedInstructions, OP0xCBBC) { TestCbOpcode(0xBC); } // ok
TEST(ExtendedInstructions, OP0xCBBD) { TestCbOpcode(0xBD); } // ok
TEST(ExtendedInstructions, OP0xCBBE) { TestCbOpcode(0xBE); } // ok
TEST(ExtendedInstructions, OP0xCBBF) { TestCbOpcode(0xBF); } // ok

TEST(ExtendedInstructions, OP0xCBC0) { TestCbOpcode(0xC0); } // ok
TEST(ExtendedInstructions, OP0xCBC1) { TestCbOpcode(0xC1); } // ok
TEST(ExtendedInstructions, OP0xCBC2) { TestCbOpcode(0xC2); } // ok
TEST(ExtendedInstructions, OP0xCBC3) { TestCbOpcode(0xC3); } // ok
TEST(ExtendedInstructions, OP0xCBC4) { TestCbOpcode(0xC4); } // ok
TEST(ExtendedInstructions, OP0xCBC5) { TestCbOpcode(0xC5); } // ok
TEST(ExtendedInstructions, OP0xCBC6) { TestCbOpcode(0xC6); } // ok
TEST(ExtendedInstructions, OP0xCBC7) { TestCbOpcode(0xC7); } // ok
TEST(ExtendedInstructions, OP0xCBC8) { TestCbOpcode(0xC8); } // ok
TEST(ExtendedInstructions, OP0xCBC9) { TestCbOpcode(0xC9); } // ok
TEST(ExtendedInstructions, OP0xCBCA) { TestCbOpcode(0xCA); } // ok
TEST(ExtendedInstructions, OP0xCBCB) { TestCbOpcode(0xCB); } // ok
TEST(ExtendedInstructions, OP0xCBCC) { TestCbOpcode(0xCC); } // ok
TEST(ExtendedInstructions, OP0xCBCD) { TestCbOpcode(0xCD); } // ok
TEST(ExtendedInstructions, OP0xCBCE) { TestCbOpcode(0xCE); } // ok
TEST(ExtendedInstructions, OP0xCBCF) { TestCbOpcode(0xCF); } // ok

TEST(ExtendedInstructions, OP0xCBD0) { TestCbOpcode(0xD0); } // ok
TEST(ExtendedInstructions, OP0xCBD1) { TestCbOpcode(0xD1); } // ok
TEST(ExtendedInstructions, OP0xCBD2) { TestCbOpcode(0xD2); } // ok
TEST(ExtendedInstructions, OP0xCBD3) { TestCbOpcode(0xD3); } // ok
TEST(ExtendedInstructions, OP0xCBD4) { TestCbOpcode(0xD4); } // ok
TEST(ExtendedInstructions, OP0xCBD5) { TestCbOpcode(0xD5); } // ok
TEST(ExtendedInstructions, OP0xCBD6) { TestCbOpcode(0xD6); } // ok
TEST(ExtendedInstructions, OP0xCBD7) { TestCbOpcode(0xD7); } // ok
TEST(ExtendedInstructions, OP0xCBD8) { TestCbOpcode(0xD8); } // ok
TEST(ExtendedInstructions, OP0xCBD9) { TestCbOpcode(0xD9); } // ok
TEST(ExtendedInstructions, OP0xCBDA) { TestCbOpcode(0xDA); } // ok
TEST(ExtendedInstructions, OP0xCBDB) { TestCbOpcode(0xDB); } // ok
TEST(ExtendedInstructions, OP0xCBDC) { TestCbOpcode(0xDC); } // ok
TEST(ExtendedInstructions, OP0xCBDD) { TestCbOpcode(0xDD); } // ok
TEST(ExtendedInstructions, OP0xCBDE) { TestCbOpcode(0xDE); } // ok
TEST(ExtendedInstructions, OP0xCBDF) { TestCbOpcode(0xDF); } // ok

TEST(ExtendedInstructions, OP0xCBE0) { TestCbOpcode(0xE0); } // ok
TEST(ExtendedInstructions, OP0xCBE1) { TestCbOpcode(0xE1); } // ok
TEST(ExtendedInstructions, OP0xCBE2) { TestCbOpcode(0xE2); } // ok
TEST(ExtendedInstructions, OP0xCBE3) { TestCbOpcode(0xE3); } // ok
TEST(ExtendedInstructions, OP0xCBE4) { TestCbOpcode(0xE4); } // ok
TEST(ExtendedInstructions, OP0xCBE5) { TestCbOpcode(0xE5); } // ok
TEST(ExtendedInstructions, OP0xCBE6) { TestCbOpcode(0xE6); } // ok
TEST(ExtendedInstructions, OP0xCBE7) { TestCbOpcode(0xE7); } // ok
TEST(ExtendedInstructions, OP0xCBE8) { TestCbOpcode(0xE8); } // ok
TEST(ExtendedInstructions, OP0xCBE9) { TestCbOpcode(0xE9); } // ok
TEST(ExtendedInstructions, OP0xCBEA) { TestCbOpcode(0xEA); } // ok
TEST(ExtendedInstructions, OP0xCBEB) { TestCbOpcode(0xEB); } // ok
TEST(ExtendedInstructions, OP0xCBEC) { TestCbOpcode(0xEC); } // ok
TEST(ExtendedInstructions, OP0xCBED) { TestCbOpcode(0xED); } // ok
TEST(ExtendedInstructions, OP0xCBEE) { TestCbOpcode(0xEE); } // ok
TEST(ExtendedInstructions, OP0xCBEF) { TestCbOpcode(0xEF); } // ok

TEST(ExtendedInstructions, OP0xCBF0) { TestCbOpcode(0xF0); } // ok
TEST(ExtendedInstructions, OP0xCBF1) { TestCbOpcode(0xF1); } // ok
TEST(ExtendedInstructions, OP0xCBF2) { TestCbOpcode(0xF2); } // ok
TEST(ExtendedInstructions, OP0xCBF3) { TestCbOpcode(0xF3); } // ok
TEST(ExtendedInstructions, OP0xCBF4) { TestCbOpcode(0xF4); } // ok
TEST(ExtendedInstructions, OP0xCBF5) { TestCbOpcode(0xF5); } // ok
TEST(ExtendedInstructions, OP0xCBF6) { TestCbOpcode(0xF6); } // ok
TEST(ExtendedInstructions, OP0xCBF7) { TestCbOpcode(0xF7); } // ok
TEST(ExtendedInstructions, OP0xCBF8) { TestCbOpcode(0xF8); } // ok
TEST(ExtendedInstructions, OP0xCBF9) { TestCbOpcode(0xF9); } // ok
TEST(ExtendedInstructions, OP0xCBFA) { TestCbOpcode(0xFA); } // ok
TEST(ExtendedInstructions, OP0xCBFB) { TestCbOpcode(0xFB); } // ok
TEST(ExtendedInstructions, OP0xCBFC) { TestCbOpcode(0xFC); } // ok
TEST(ExtendedInstructions, OP0xCBFD) { TestCbOpcode(0xFD); } // ok
TEST(ExtendedInstructions, OP0xCBFE) { TestCbOpcode(0xFE); } // ok
TEST(ExtendedInstructions, OP0xCBFF) { TestCbOpcode(0xFF); } // ok
#endif // ENABLED