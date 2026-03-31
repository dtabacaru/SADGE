#pragma once

#include "InterruptController.h"
#include "AudioController.h"
#include "LcdController.h"
#include "JoypadController.h"
#include "SerialController.h"
#include "TimerController.h"

#include "Constants.h"
#include "RomHeader.h"
#include "Status.h"
#include "StopWatch.h"

#include <array>
#include <filesystem>
#include <fstream>

struct Palettes
{
  std::array<Pixel, 4> bg;
  std::array<Pixel, 4> obj0;
  std::array<Pixel, 4> obj1;
};

enum class EdgeType : uint8_t
{
  FALLING = 0,
  RISING = 1
};

enum class LevelType : uint8_t
{
  LOW = 0,
  HIGH = 1
};

struct TestCycle
{
  uint16_t addrBus;
  uint8_t  dataBus;
};

class Cpu
{
public:
#pragma region PUBLIC CONSTANTS
  constexpr static auto KILOBYTES_TO_BYTES = 1024;
  constexpr static auto NUM_BANKS = 512;
  constexpr static uint64_t BANK_SIZE = 16 * KILOBYTES_TO_BYTES;
  constexpr static auto MAX_ROM_SIZE = NUM_BANKS * BANK_SIZE; // 512 banks @ 16 KB
  constexpr static uint8_t DEFAULT_READ = 0xFF;
#pragma endregion

#pragma region PUBLIC FUNCTIONS
  Cpu();
  ~Cpu();

  constexpr static auto DEFAULT_FRAME_T_CYCLES = 70224;
  constexpr static auto DRAG_WINDOW_DETECT_TIME = DEFAULT_FRAME_T_CYCLES / T_RATE;
  constexpr static auto MIN_SLEEP_TIME = 0.003;
  
  Status SetRom(const std::filesystem::path& romPath, std::vector<uint8_t> rom);
  virtual void Main();

  void RunUntil(uint64_t count);
  void Run();
  void Stop();

  AudioController&  GetAudioController()  { return mAudioCtrl;  }
  LcdController&    GetLcdController()    { return mLcdCtrl;    }
  JoypadController& GetJoypadController() { return mJoypadCtrl; }

  Status InsertRom(const std::filesystem::path& rom_path);

  Palettes GetPalettes(std::string title);
  uint8_t  GetTitleHash(std::string title);

  void Fetch();

  void SetState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime, uint8_t ie);
  bool CheckState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime);

  uint64_t mExeCycle{0};
  uint16_t mAddressBus{0};
  uint8_t  mDataBus{0};
  void TickExecution();
#pragma endregion

private:

#pragma region REGISTERS
  union Register
  {
    struct
    {
      uint8_t L;
      uint8_t H;
    };
    uint16_t HL{};
  };

  Register mAF{};
  Register mBC{};
  Register mDE{};
  Register mHL{};

  Register mSP{};
  Register mPC{};

  Register mWZ{};
#pragma endregion

#pragma region FLAGS
  enum class FlagBitMask
  {
    Carry = 0b00010000,
    HalfCarry = 0b00100000,
    Subtract = 0b01000000,
    Zero = 0b10000000
  };

  void SetHalfCarryFlag(uint8_t val1, int val2);

  void SetHalfCarryFlag(uint8_t val1, int val2, int carry);

  void SetHalfCarryFlag(uint16_t val1, int val2);

  void SetCarry16Bit(int result);

  void SetCarry8Bit(int result);

  void SetZeroFlag(uint8_t val);

  void SetSubtractionFlag(int val);

  void SetFlag(FlagBitMask flag);

  void ResetFlag(FlagBitMask flag);

  bool ReadFlag(FlagBitMask flag) const;
#pragma endregion

#pragma region CARTRIDGE
  enum class Mbc1Mode
  {
    SINGLE_RAM = 0b0,
    MULTIPLE_RAM = 0b1
  };

  void Mbc1_7FFF(uint8_t val);
  void Mbc1_5FFF(uint8_t val);
  void Mbc1_3FFF(uint8_t val);
  void Mbc1_1FFF(uint8_t val);

  void Mbc3_7FFF(uint8_t val);
  void Mbc3_5FFF(uint8_t val);
  void Mbc3_3FFF(uint8_t val);
  void Mbc3_1FFF(uint8_t val);

  const std::string SAVE_EXTENSION = ".sav";

  RomHeader m_rom_header;
  Status ParseHeader();
  
  void InitBanks();
  void WriteEram();
  
  bool m_battery_flag = false;
  std::filesystem::path m_rom_file_path{};
  std::filesystem::path m_save_file_path{};

  bool mEramEn = true;
  Mbc1Mode m_mbc_mode = Mbc1Mode::SINGLE_RAM;

  uint64_t m_num_rom_banks = 2;
  uint64_t m_num_ram_banks = 1;
  int mRomBank = 1;
  int mRamBank = 0;

  uint8_t m_lower_bank_bits = 1;
  uint8_t m_upper_bank_bits = 0;
#pragma endregion

#pragma region MEMORY
  constexpr static uint32_t WRAM_SIZE = 8 * KILOBYTES_TO_BYTES; // 8 KB Fixed Work RAM
  constexpr static uint32_t ERAM_SIZE = 8 * KILOBYTES_TO_BYTES; // 8 KB Fixed Work RAM
  constexpr static uint32_t HRAM_SIZE = 127;

  //https://github.com/Hacktix/Bootix
  const std::vector<uint8_t> BOOT_ROM = {
    0x31,0xfe,0xff,0x21,0xff,0x9f,0xaf,0x32,0xcb,0x7c,0x20,0xfa,0x0e,0x11,0x21,0x26,
    0xff,0x3e,0x80,0x32,0xe2,0x0c,0x3e,0xf3,0x32,0xe2,0x0c,0x3e,0x77,0x32,0xe2,0x11,
    0x04,0x01,0x21,0x10,0x80,0x1a,0xcd,0xb8,0x00,0x1a,0xcb,0x37,0xcd,0xb8,0x00,0x13,
    0x7b,0xfe,0x34,0x20,0xf0,0x11,0xcc,0x00,0x06,0x08,0x1a,0x13,0x22,0x23,0x05,0x20,
    0xf9,0x21,0x04,0x99,0x01,0x0c,0x01,0xcd,0xb1,0x00,0x3e,0x19,0x77,0x21,0x24,0x99,
    0x0e,0x0c,0xcd,0xb1,0x00,0x3e,0x91,0xe0,0x40,0x06,0x10,0x11,0xd4,0x00,0x78,0xe0,
    0x43,0x05,0x7b,0xfe,0xd8,0x28,0x04,0x1a,0xe0,0x47,0x13,0x0e,0x1c,0xcd,0xa7,0x00,
    0xaf,0x90,0xe0,0x43,0x05,0x0e,0x1c,0xcd,0xa7,0x00,0xaf,0xb0,0x20,0xe0,0xe0,0x43,
    0x3e,0x83,0xcd,0x9f,0x00,0x0e,0x27,0xcd,0xa7,0x00,0x3e,0xc1,0xcd,0x9f,0x00,0x11,
    0x8a,0x01,0xf0,0x44,0xfe,0x90,0x20,0xfa,0x1b,0x7a,0xb3,0x20,0xf5,0x18,0x49,0x0e,
    0x13,0xe2,0x0c,0x3e,0x87,0xe2,0xc9,0xf0,0x44,0xfe,0x90,0x20,0xfa,0x0d,0x20,0xf7,
    0xc9,0x78,0x22,0x04,0x0d,0x20,0xfa,0xc9,0x47,0x0e,0x04,0xaf,0xc5,0xcb,0x10,0x17,
    0xc1,0xcb,0x10,0x17,0x0d,0x20,0xf5,0x22,0x23,0x22,0x23,0xc9,0x3c,0x42,0xb9,0xa5,
    0xb9,0xa5,0x42,0x3c,0x00,0x54,0xa8,0xfc,0x42,0x4f,0x4f,0x54,0x49,0x58,0x2e,0x44,
    0x4d,0x47,0x20,0x76,0x31,0x2e,0x32,0x00,0x3e,0xff,0xc6,0x01,0x0b,0x1e,0xd8,0x21,
    0x4d,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x01,0xe0,0x50,
  };

  std::vector<uint8_t> m_rom;
  std::vector<std::vector<uint8_t>> mRomBanks;
  std::vector<uint8_t> m_hram = std::vector<uint8_t>(HRAM_SIZE);
  std::vector<uint8_t> m_wram = std::vector<uint8_t>(WRAM_SIZE);
  std::vector<std::vector<uint8_t>> mRamBanks;
#pragma endregion

#pragma region EXECUTION
  constexpr static auto M_DIV = 4;
  constexpr static auto EXE_COMPLETE = 0;

  enum class ExecutionAddress : uint16_t
  {
    RESET = 0x0100,
    IO = 0xFF00,
    BOOT_COMPLETE = 0xFF50
  };

  enum class RestartVector : uint16_t
  {
    RST0 = 0x0000,
    RST1 = 0x0008,
    RST2 = 0x0010,
    RST3 = 0x0018,
    RST4 = 0x0020,
    RST5 = 0x0028,
    RST6 = 0x0030,
    RST7 = 0x0038
  };

  enum class ExecutionMode : uint8_t
  {
    INSTRUCTION,
    EXT_INSTRUCTION,
    INTERRUPT,
    HALT,
    STOP
  };

  static constexpr uint16_t GetRestartVectorAddress(RestartVector vec)
  {
    return static_cast<uint16_t>(vec);
  }

  void Init();

  void TickEdge(EdgeType& edge, LevelType& level);

  void TEvent();
  void FallingTEvent();
  void RisingTEvent();
  void MEvent();
  void FallingMEvent();
  void RisingMEvent();

  virtual void InstructionCompleteEvent();
  void InterruptCompleteEvent();
  void ExtInstructionCompleteEvent();
  void HaltCompleteEvent();

  void InstructionHandler();
  void ExtInstructionHandler();
  void InterruptHandler();
  void HaltHandler();

  void ReadNextUint8();

  // Presumably, cycles start at rising edges, and the system powers on triggering a rising edge
  // Initialize these as falling/low/-1 such that the very first cycle mimics the behaviour
  // of powering on a DMG
  EdgeType  mTEdge{EdgeType::FALLING};
  LevelType mTLevel{LevelType::LOW};
  uint64_t  mTLowCount{static_cast<uint64_t>(-1)};
  uint64_t  mTHighCount{static_cast<uint64_t>(-1)};
  uint64_t  mTEdgeCount{static_cast<uint64_t>(-1)};

  EdgeType  mMEdge{EdgeType::FALLING};
  LevelType mMLevel{LevelType::LOW};
  uint64_t  mMLowCount{static_cast<uint64_t>(-1)};
  uint64_t  mMHighCount{static_cast<uint64_t>(-1)};
  uint64_t  mMEdgeCount{static_cast<uint64_t>(-1)};

  ExecutionMode mExecutionMode = ExecutionMode::INSTRUCTION;

  uint8_t  mOpcode{0};

  uint64_t mTFrameCycles = 0;

  StopWatch mExeStopwatch;
  
  double mFrameTime = 0; // seconds
  double mCompensationTime = 0; // seconds
  
  std::atomic<bool> mStopped = false;
  bool mBooted{};
  bool mImeRequest = false;

  virtual void Read();
  virtual void Write();

  void ReadIo();
  void WriteIo();

  virtual void WaitFrame();
#pragma endregion

#pragma region I/O CONTROLLERS
  InterruptController mInterruptCtrl;
  AudioController mAudioCtrl;
  LcdController mLcdCtrl;
  JoypadController mJoypadCtrl;
  SerialController mSerialCtrl;
  TimerController mTimerCtrl;
#pragma endregion

#pragma region GENERALIZED INSTRUCTIONS

  //-------------------------------------------------------------------------
  // Naming convention:
  //   _ denotes a value
  //  __ denotes an address
  //   r denotes an 8-bit register
  //  rr denotes a 16-bit register
  //   n denotes the next 8-bit value
  //  nn denotes the next 16-bit value
  //   e denotes the next signed 8-bit value
  //  cc denotes conditional flag
  // 
  //   x denotes wildcard
  // 
  //  Registers are capitalized
  // 
  //  e.g. HL, C
  // 
  //  Operations are capitalized
  // 
  //  e.g. LD, JP
  //
  //  Some functions are specific to a register.
  //  These functions will denote the register in the name.
  // 
  //  e.g. JP_HL -> Jump to HL
  //  e.g. LD__rr_A -> Load the value of A into the address pointed to by RR
  //-------------------------------------------------------------------------

  // Flags: - - - -
  void NOP();

  // Flags: - - - -
  void CB();

  // Flags: - - - -
  void JP_HL();

  // Flags: - - - -
  void LD_r_x(uint8_t& r, uint8_t val);

  // Flags: - - - -
  void LD_r__HL(uint8_t& r);

  // Flags: - - - -
  void LD_r_n(uint8_t& r);

  // Flags: - - - -
  void LD_A__rr(Register rr);

  // Flags: - - - -
  void LD_A__nn();

  // Flags: - - - -
  void LD_A__HLx(int val);

  // Flags: - - - -
  void LD_SP_HL();

  // Flags: - - - -
  void LD__nn_SP();

  // Flags: - - - -
  void LD_rr_nn(Register& rr);

  // Flags: - - - -
  void LD__rr_r(Register rr, uint8_t r);

  // Flags: - - - -
  void LD__rr_A(Register rr);

  // Flags: - - - -
  void LD__nn_A();

  // Flags: - - - -
  void LD__HLx_A(int val);

  // Flags: - - - -
  void LD__HL_n();

  // Flags: - - - -
  void LDH__n_A();

  // Flags: - - - -
  void LDH__C_A();

  // Flags: - - - -
  void LDH_A__C();

  // Flags: - - - -
  void LDH_A__n();

  // Flags: 0 0 H C
  void LD_HL_SP_e();

  // Flags: 0 0 H C
  void ADD_SP_e();

  // Flags: - - - -
  void ADD_rr(Register& rr, int val);

  // Flags: Z 0 H C
  void ADC_A_x(int val);

  // Flags: Z 0 H C
  void ADC_A__HL();

  // Flags: Z 0 H C
  void ADC_A_n();

  // Flags: Z 0 H C
  void ADD_A_x(int val);

  // Flags: Z 0 H C
  void ADD_A__HL();

  // Flags: Z 0 H C
  void ADD_A_n();

  // Flags: Z 1 H C
  void SBC_A_x(int val);

  // Flags: Z 1 H C
  void SBC_A__HL();

  // Flags: Z 1 H C
  void SBC_A_n();

  // Flags: Z 1 H C
  void SUB_A_x(int val);

  // Flags: Z 1 H C
  void SUB_A__HL();

  // Flags: Z 1 H C
  void SUB_A_n();

  // Flags: Z - H C
  void ADD_x(uint8_t& r, int val, bool set_carry = false);

  // Flags: Z N H -
  void ADD__HL(int val);

  // Flags: - 0 H C
  void ADD_HL_rr(Register rr);

  // Flags: Z 0 1 0
  void AND_A_x(uint8_t val);

  // Flags: Z 0 1 0
  void AND_A__HL();

  // Flags: Z 0 1 0
  void AND_A_n();

  // Flags: Z 0 0 0
  void XOR_A_x(uint8_t val);

  // Flags: Z 0 0 0
  void XOR_A__HL();

  // Flags: Z 0 0 0
  void XOR_A_n();

  // Flags: Z 0 0 0
  void OR_A_X(uint8_t val);

  // Flags: Z 0 0 0
  void OR_A__HL();

  // Flags: Z 0 0 0
  void OR_A_n();

  // Flags: Z 1 H C
  void CP_A_x(uint8_t val);

  // Flags: Z 1 H C
  void CP_A__HL();

  // Flags: Z 1 H C
  void CP_A_n();

  // Flags: 0 0 0 A7
  void RLCA();

  // Flags: 0 0 0 A7
  void RLA();

  // Flags: 0 0 0 A0
  void RRCA();

  // Flags: 0 0 0 A0
  void RRA();

  // Flags: - 1 1 -
  void CPL();

  // https://forums.nesdev.org/viewtopic.php?t=15944#:~:text=The%20DAA%20instruction%20adjusts%20the,%2C%20lower%20nybble%2C%20or%20both.
  // Flags: Z - 0 C
  void DAA();

  // Flags: - 0 0 1
  void SCF();

  // Flags: - 0 0 !C
  void CCF();

  // Flags: Z 0 H -
  void INC_r(uint8_t& r);

  // Flags: Z 1 H -
  void DEC_r(uint8_t& r);

  // Flags - - - -
  void PUSH_rr(Register rr);

  // Flags: - - - -
  void JR_cc_e(bool jump);

  // Flags: - - - -
  void POP_rr(Register& rr);

  // Flags: - - - -
  void POP_AF();

  // Flags: - - - -
  void RET(bool set_ime = false);

  // Flags: - - - -
  void RET_cc(bool ret);

  // Flags: - - - -
  void JP_cc_nn(bool jump);

  // Flags: - - - -
  void CALL(bool call);

  // Flags: - - - -
  void RST(RestartVector vec);
#pragma endregion

#pragma region INSTRUCTIONS
  void Op0x00(); void Op0x01(); void Op0x02(); void Op0x03(); void Op0x04(); void Op0x05(); void Op0x06(); void Op0x07(); void Op0x08(); void Op0x09(); void Op0x0A(); void Op0x0B(); void Op0x0C(); void Op0x0D(); void Op0x0E(); void Op0x0F();
  void Op0x10(); void Op0x11(); void Op0x12(); void Op0x13(); void Op0x14(); void Op0x15(); void Op0x16(); void Op0x17(); void Op0x18(); void Op0x19(); void Op0x1A(); void Op0x1B(); void Op0x1C(); void Op0x1D(); void Op0x1E(); void Op0x1F();
  void Op0x20(); void Op0x21(); void Op0x22(); void Op0x23(); void Op0x24(); void Op0x25(); void Op0x26(); void Op0x27(); void Op0x28(); void Op0x29(); void Op0x2A(); void Op0x2B(); void Op0x2C(); void Op0x2D(); void Op0x2E(); void Op0x2F();
  void Op0x30(); void Op0x31(); void Op0x32(); void Op0x33(); void Op0x34(); void Op0x35(); void Op0x36(); void Op0x37(); void Op0x38(); void Op0x39(); void Op0x3A(); void Op0x3B(); void Op0x3C(); void Op0x3D(); void Op0x3E(); void Op0x3F();
  void Op0x40(); void Op0x41(); void Op0x42(); void Op0x43(); void Op0x44(); void Op0x45(); void Op0x46(); void Op0x47(); void Op0x48(); void Op0x49(); void Op0x4A(); void Op0x4B(); void Op0x4C(); void Op0x4D(); void Op0x4E(); void Op0x4F();
  void Op0x50(); void Op0x51(); void Op0x52(); void Op0x53(); void Op0x54(); void Op0x55(); void Op0x56(); void Op0x57(); void Op0x58(); void Op0x59(); void Op0x5A(); void Op0x5B(); void Op0x5C(); void Op0x5D(); void Op0x5E(); void Op0x5F();
  void Op0x60(); void Op0x61(); void Op0x62(); void Op0x63(); void Op0x64(); void Op0x65(); void Op0x66(); void Op0x67(); void Op0x68(); void Op0x69(); void Op0x6A(); void Op0x6B(); void Op0x6C(); void Op0x6D(); void Op0x6E(); void Op0x6F();
  void Op0x70(); void Op0x71(); void Op0x72(); void Op0x73(); void Op0x74(); void Op0x75(); void Op0x76(); void Op0x77(); void Op0x78(); void Op0x79(); void Op0x7A(); void Op0x7B(); void Op0x7C(); void Op0x7D(); void Op0x7E(); void Op0x7F();
  void Op0x80(); void Op0x81(); void Op0x82(); void Op0x83(); void Op0x84(); void Op0x85(); void Op0x86(); void Op0x87(); void Op0x88(); void Op0x89(); void Op0x8A(); void Op0x8B(); void Op0x8C(); void Op0x8D(); void Op0x8E(); void Op0x8F();
  void Op0x90(); void Op0x91(); void Op0x92(); void Op0x93(); void Op0x94(); void Op0x95(); void Op0x96(); void Op0x97(); void Op0x98(); void Op0x99(); void Op0x9A(); void Op0x9B(); void Op0x9C(); void Op0x9D(); void Op0x9E(); void Op0x9F();
  void Op0xA0(); void Op0xA1(); void Op0xA2(); void Op0xA3(); void Op0xA4(); void Op0xA5(); void Op0xA6(); void Op0xA7(); void Op0xA8(); void Op0xA9(); void Op0xAA(); void Op0xAB(); void Op0xAC(); void Op0xAD(); void Op0xAE(); void Op0xAF();
  void Op0xB0(); void Op0xB1(); void Op0xB2(); void Op0xB3(); void Op0xB4(); void Op0xB5(); void Op0xB6(); void Op0xB7(); void Op0xB8(); void Op0xB9(); void Op0xBA(); void Op0xBB(); void Op0xBC(); void Op0xBD(); void Op0xBE(); void Op0xBF();
  void Op0xC0(); void Op0xC1(); void Op0xC2(); void Op0xC3(); void Op0xC4(); void Op0xC5(); void Op0xC6(); void Op0xC7(); void Op0xC8(); void Op0xC9(); void Op0xCA(); void Op0xCB(); void Op0xCC(); void Op0xCD(); void Op0xCE(); void Op0xCF();
  void Op0xD0(); void Op0xD1(); void Op0xD2(); void Op0xD3(); void Op0xD4(); void Op0xD5(); void Op0xD6(); void Op0xD7(); void Op0xD8(); void Op0xD9(); void Op0xDA(); void Op0xDB(); void Op0xDC(); void Op0xDD(); void Op0xDE(); void Op0xDF();
  void Op0xE0(); void Op0xE1(); void Op0xE2(); void Op0xE3(); void Op0xE4(); void Op0xE5(); void Op0xE6(); void Op0xE7(); void Op0xE8(); void Op0xE9(); void Op0xEA(); void Op0xEB(); void Op0xEC(); void Op0xED(); void Op0xEE(); void Op0xEF();
  void Op0xF0(); void Op0xF1(); void Op0xF2(); void Op0xF3(); void Op0xF4(); void Op0xF5(); void Op0xF6(); void Op0xF7(); void Op0xF8(); void Op0xF9(); void Op0xFA(); void Op0xFB(); void Op0xFC(); void Op0xFD(); void Op0xFE(); void Op0xFF();
#pragma endregion

#pragma region GENERALIZED EXTENDED INSTRUCTIONS
  //Flags: Z 0 0 Bit7
  void RLC_R(uint8_t& R);

  //Flags: Z 0 0 Bit7
  void RLC__HL();

  // Flags: Z 0 0 Bit0
  void RRC_R(uint8_t& R);

  // Flags: Z 0 0 Bit0
  void RRC__HL();

  // Flags: Z 0 0 Bit7
  void RL_R(uint8_t& R);

  // Flags: Z 0 0 Bit7
  void RL__HL();

  // Flags: Z 0 0 Bit0
  void RR_R(uint8_t& R);

  // Flags: Z 0 0 Bit0
  void RR__HL();

  // Flags: Z 0 0 Bit7
  void SLA_R(uint8_t& R);

  // Flags: Z 0 0 Bit7
  void SLA__HL();

  // Flags: Z 0 0 Bit0
  void SRA_R(uint8_t& R);

  // Flags: Z 0 0 Bit0
  void SRA__HL();

  // Flags: Z 0 0 0
  void SWAP_R(uint8_t& R);

  // Flags: Z 0 0 0
  void SWAP__HL();

  // Flags: Z 0 0 bit0
  void SRL_R(uint8_t& R);

  // Flags: Z 0 0 bit0
  void SRL__HL();

  // Flags: !bit 0 1 -
  void BIT_R(uint8_t mask, uint8_t R);

  // Flags: !bit 0 1 -
  void BIT__HL(uint8_t mask);

  // Flags: - - - -
  void RES_R(uint8_t mask, uint8_t& R);

  // Flags: - - - -
  void RES__HL(uint8_t mask);

  // Flags: - - - -
  void SET_R(uint8_t mask, uint8_t& R);

  // Flags: - - - -
  void SET__HL(uint8_t mask);
#pragma endregion

#pragma region EXTENDED INSTRUCTIONS
   void OpCb0x00(); void OpCb0x01(); void OpCb0x02(); void OpCb0x03(); void OpCb0x04(); void OpCb0x05(); void OpCb0x06(); void OpCb0x07(); void OpCb0x08(); void OpCb0x09(); void OpCb0x0A(); void OpCb0x0B(); void OpCb0x0C(); void OpCb0x0D(); void OpCb0x0E(); void OpCb0x0F();
   void OpCb0x10(); void OpCb0x11(); void OpCb0x12(); void OpCb0x13(); void OpCb0x14(); void OpCb0x15(); void OpCb0x16(); void OpCb0x17(); void OpCb0x18(); void OpCb0x19(); void OpCb0x1A(); void OpCb0x1B(); void OpCb0x1C(); void OpCb0x1D(); void OpCb0x1E(); void OpCb0x1F();
   void OpCb0x20(); void OpCb0x21(); void OpCb0x22(); void OpCb0x23(); void OpCb0x24(); void OpCb0x25(); void OpCb0x26(); void OpCb0x27(); void OpCb0x28(); void OpCb0x29(); void OpCb0x2A(); void OpCb0x2B(); void OpCb0x2C(); void OpCb0x2D(); void OpCb0x2E(); void OpCb0x2F();
   void OpCb0x30(); void OpCb0x31(); void OpCb0x32(); void OpCb0x33(); void OpCb0x34(); void OpCb0x35(); void OpCb0x36(); void OpCb0x37(); void OpCb0x38(); void OpCb0x39(); void OpCb0x3A(); void OpCb0x3B(); void OpCb0x3C(); void OpCb0x3D(); void OpCb0x3E(); void OpCb0x3F();
   void OpCb0x40(); void OpCb0x41(); void OpCb0x42(); void OpCb0x43(); void OpCb0x44(); void OpCb0x45(); void OpCb0x46(); void OpCb0x47(); void OpCb0x48(); void OpCb0x49(); void OpCb0x4A(); void OpCb0x4B(); void OpCb0x4C(); void OpCb0x4D(); void OpCb0x4E(); void OpCb0x4F();
   void OpCb0x50(); void OpCb0x51(); void OpCb0x52(); void OpCb0x53(); void OpCb0x54(); void OpCb0x55(); void OpCb0x56(); void OpCb0x57(); void OpCb0x58(); void OpCb0x59(); void OpCb0x5A(); void OpCb0x5B(); void OpCb0x5C(); void OpCb0x5D(); void OpCb0x5E(); void OpCb0x5F();
   void OpCb0x60(); void OpCb0x61(); void OpCb0x62(); void OpCb0x63(); void OpCb0x64(); void OpCb0x65(); void OpCb0x66(); void OpCb0x67(); void OpCb0x68(); void OpCb0x69(); void OpCb0x6A(); void OpCb0x6B(); void OpCb0x6C(); void OpCb0x6D(); void OpCb0x6E(); void OpCb0x6F();
   void OpCb0x70(); void OpCb0x71(); void OpCb0x72(); void OpCb0x73(); void OpCb0x74(); void OpCb0x75(); void OpCb0x76(); void OpCb0x77(); void OpCb0x78(); void OpCb0x79(); void OpCb0x7A(); void OpCb0x7B(); void OpCb0x7C(); void OpCb0x7D(); void OpCb0x7E(); void OpCb0x7F();
   void OpCb0x80(); void OpCb0x81(); void OpCb0x82(); void OpCb0x83(); void OpCb0x84(); void OpCb0x85(); void OpCb0x86(); void OpCb0x87(); void OpCb0x88(); void OpCb0x89(); void OpCb0x8A(); void OpCb0x8B(); void OpCb0x8C(); void OpCb0x8D(); void OpCb0x8E(); void OpCb0x8F();
   void OpCb0x90(); void OpCb0x91(); void OpCb0x92(); void OpCb0x93(); void OpCb0x94(); void OpCb0x95(); void OpCb0x96(); void OpCb0x97(); void OpCb0x98(); void OpCb0x99(); void OpCb0x9A(); void OpCb0x9B(); void OpCb0x9C(); void OpCb0x9D(); void OpCb0x9E(); void OpCb0x9F();
   void OpCb0xA0(); void OpCb0xA1(); void OpCb0xA2(); void OpCb0xA3(); void OpCb0xA4(); void OpCb0xA5(); void OpCb0xA6(); void OpCb0xA7(); void OpCb0xA8(); void OpCb0xA9(); void OpCb0xAA(); void OpCb0xAB(); void OpCb0xAC(); void OpCb0xAD(); void OpCb0xAE(); void OpCb0xAF();
   void OpCb0xB0(); void OpCb0xB1(); void OpCb0xB2(); void OpCb0xB3(); void OpCb0xB4(); void OpCb0xB5(); void OpCb0xB6(); void OpCb0xB7(); void OpCb0xB8(); void OpCb0xB9(); void OpCb0xBA(); void OpCb0xBB(); void OpCb0xBC(); void OpCb0xBD(); void OpCb0xBE(); void OpCb0xBF();
   void OpCb0xC0(); void OpCb0xC1(); void OpCb0xC2(); void OpCb0xC3(); void OpCb0xC4(); void OpCb0xC5(); void OpCb0xC6(); void OpCb0xC7(); void OpCb0xC8(); void OpCb0xC9(); void OpCb0xCA(); void OpCb0xCB(); void OpCb0xCC(); void OpCb0xCD(); void OpCb0xCE(); void OpCb0xCF();
   void OpCb0xD0(); void OpCb0xD1(); void OpCb0xD2(); void OpCb0xD3(); void OpCb0xD4(); void OpCb0xD5(); void OpCb0xD6(); void OpCb0xD7(); void OpCb0xD8(); void OpCb0xD9(); void OpCb0xDA(); void OpCb0xDB(); void OpCb0xDC(); void OpCb0xDD(); void OpCb0xDE(); void OpCb0xDF();
   void OpCb0xE0(); void OpCb0xE1(); void OpCb0xE2(); void OpCb0xE3(); void OpCb0xE4(); void OpCb0xE5(); void OpCb0xE6(); void OpCb0xE7(); void OpCb0xE8(); void OpCb0xE9(); void OpCb0xEA(); void OpCb0xEB(); void OpCb0xEC(); void OpCb0xED(); void OpCb0xEE(); void OpCb0xEF();
   void OpCb0xF0(); void OpCb0xF1(); void OpCb0xF2(); void OpCb0xF3(); void OpCb0xF4(); void OpCb0xF5(); void OpCb0xF6(); void OpCb0xF7(); void OpCb0xF8(); void OpCb0xF9(); void OpCb0xFA(); void OpCb0xFB(); void OpCb0xFC(); void OpCb0xFD(); void OpCb0xFE(); void OpCb0xFF();
#pragma endregion
};
