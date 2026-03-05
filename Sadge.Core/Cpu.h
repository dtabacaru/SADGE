#pragma once

#include "InterruptController.h"
#include "AudioController.h"
#include "LcdController.h"
#include "JoypadController.h"
#include "SerialController.h"
#include "TimerController.h"

#include "RomHeader.h"
#include "Status.h"
#include "StopWatch.h"

#include <array>
#include <iostream>
#include <iomanip>
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
  uint16_t address_bus;
  uint8_t  data_bus;
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

  constexpr static auto DRAG_WINDOW_DETECT_TIME = (70224.0 / (1 << 22)) * 1;
  constexpr static auto MACHINE_CLOCK_DIV = 4;
  constexpr static auto INSTRUCTION_COMPLETE = 0;

  // Presumably, cycles start at rising edges, and the system powers on triggering a rising edge
  // Initialize these as falling/low/-1 such that the very first cycle mimics the behaviour
  // of powering on a DMG
  EdgeType  _tEdge{EdgeType::FALLING};
  LevelType _tLevel{LevelType::LOW};
  uint64_t  _tEdgeCount{static_cast<uint64_t>(-1)};

  EdgeType  _mEdge{EdgeType::FALLING};
  LevelType _mLevel{LevelType::LOW};
  uint64_t  _mEdgeCount{static_cast<uint64_t>(-1)};

  inline void TickEdge(EdgeType& edge, LevelType& level, uint64_t& count)
  {
    edge  = edge == EdgeType::FALLING ? EdgeType::RISING : EdgeType::FALLING;
    level = edge == EdgeType::FALLING ? LevelType::LOW : LevelType::HIGH;
    count += 1;
  }

  void RisingTEvent();
  void FallingTEvent();
  void RisingMEvent();
  void FallingMEvent();

  uint64_t _opcycle{0};
  uint16_t _addressBus{0};
  uint8_t  _dataBus{0};

  void TickExecution();
  std::vector<TestCycle> _test_cycles;

  Status SetRom(const std::filesystem::path& rom_path, std::vector<uint8_t> rom);
  void Main();

  void RunUntil(uint64_t cycle_count);
  void Run();

  inline void Stop()
  {
    m_stopped = true;
  }

  StopWatch m_execution_stopwatch;

  inline std::vector<TestCycle> GetTestCycles()
  {
    return _test_cycles;
  }

  inline AudioController&  GetAudioController()  { return m_audio_controller; }
  inline LcdController&    GetLcdController()    { return m_lcd_controller; }
  inline JoypadController& GetJoypadController() { return m_joypad_controller; }

  Status InsertRom(const std::filesystem::path& rom_path)
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

    SetRom(rom_path, std::move(rom));

    return status;
  }

  Palettes GetPalettes(std::string title);
  uint8_t  GetTitleHash(std::string title);

  void TestExecute();
  void SetTestState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime, uint8_t ie);
  bool CheckTestState(uint16_t pc, uint16_t sp, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t h, uint8_t l, bool ime);
#pragma endregion

private:

#pragma region REGISTERS
  union Register
  {
    struct
    {
      uint8_t l;
      uint8_t h;
    };
    uint16_t hl{};
  };

  Register _af{};
  Register _bc{};
  Register _de{};
  Register _hl{};

  Register _sp{};
  Register _pc{};

  Register _wz{};
  uint8_t   _n{};

std::string OutputRegisters()
  {
    std::ostringstream ss;

    ss << std::dec << m_total_cycle_count;
    ss << std::hex << std::uppercase << std::setfill('0');
    ss << " AF: " << std::setw(4) << _af.hl << " BC: " << std::setw(4) << _bc.hl << " DE: " << std::setw(4) << _de.hl << " HL: " << std::setw(4) << _hl.hl << " SP: " << std::setw(4) << _sp.hl << " PC: " << std::setw(4) << _pc.hl << " op: " << std::setw(2) << (int)_opcode;
    ss << " IME: " << std::dec << (int)m_interrupt_controller.m_ime << " IE: " << (int)m_interrupt_controller.m_ie << " IF: " << (int)m_interrupt_controller.m_if;

    return ss.str();
  }

#pragma endregion

#pragma region FLAGS
  enum class FlagBitMask
  {
    Carry = 0b00010000,
    HalfCarry = 0b00100000,
    Subtract = 0b01000000,
    Zero = 0b10000000
  };

  inline void SetHalfCarryFlag(uint8_t val1, int val2)
  {
    bool bit_4_set = val2 < 0 ? ((val1 & 0xF) - (-val2 & 0xF)) & 0x10
                              : ((val1 & 0xF) + (+val2 & 0xF)) & 0x10;

    bit_4_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
  }

  inline void SetHalfCarryFlag(uint8_t val1, int val2, int carry)
  {
    bool bit_4_set = val2 < 0 ? ((val1 & 0xF) - (-val2 & 0xF) - (-carry & 0xF)) & 0x10
                              : ((val1 & 0xF) + (+val2 & 0xF) + (+carry & 0xF)) & 0x10;

    bit_4_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
  }

  inline void SetHalfCarryFlag(uint16_t val1, int val2)
  {
    bool bit_12_set = val2 < 0 ? ((val1 & 0xFFF) - (-val2 & 0xFFF)) & 0x1000
                               : ((val1 & 0xFFF) + (+val2 & 0xFFF)) & 0x1000;

    bit_12_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
  }

  inline void SetCarry16Bit(int result)
  {
    (result > 0xFFFF || result < 0) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  }

  inline void SetCarry8Bit(int result)
  {
    (result > 0xFF || result < 0) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  }

  inline void SetZeroFlag(uint8_t val)
  {
    val ? ResetFlag(FlagBitMask::Zero) : SetFlag(FlagBitMask::Zero);
  }

  inline void SetSubtractionFlag(int val)
  {
    val < 0 ? SetFlag(FlagBitMask::Subtract) : ResetFlag(FlagBitMask::Subtract);
  }

  inline void SetFlag(FlagBitMask flag)
  {
    _af.l |= static_cast<uint8_t>(flag);
  }

  inline void ResetFlag(FlagBitMask flag)
  {
    _af.l &= ~static_cast<uint8_t>(flag);
  }

  inline bool ReadFlag(FlagBitMask flag) const
  {
    return _af.l & static_cast<uint8_t>(flag);
  }
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

  Status ParseHeader();
  
  void InitBanks();
  void WriteEram();
  
  bool m_battery_flag = false;
  std::filesystem::path m_rom_file_path{};
  std::filesystem::path m_save_file_path{};

  bool m_external_ram_enable = true;
  Mbc1Mode m_mbc_mode = Mbc1Mode::SINGLE_RAM;

  uint64_t m_num_rom_banks = 2;
  uint64_t m_num_ram_banks = 1;
  int m_rom_bank = 1;
  int m_ram_bank = 0;

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
  std::vector<std::vector<uint8_t>> m_rom_banks;
  std::vector<uint8_t> m_hram = std::vector<uint8_t>(HRAM_SIZE);
  std::vector<uint8_t> m_wram = std::vector<uint8_t>(WRAM_SIZE);
  std::vector<std::vector<uint8_t>> m_ram_banks;
#pragma endregion

#pragma region EXECUTION
  constexpr static double CLOCK_RATE = (1 << 22);

  enum class ExecutionAddress
  {
    RESET = 0x0100,
    IO = 0xFF00,
    BOOT_COMPLETE = 0xFF50
  };

  RomHeader m_rom_header;

  uint64_t m_total_cycle_count = 0;

  std::atomic<bool> m_stopped = false;
  double m_compensation_time = 0;
  uint64_t m_frame_cycles = 0;

  uint8_t _opcode = 0x00;

  bool m_boot_complete = false;
  bool m_interrupt_enabled_requested = false;
  bool m_stop_requested = false;
  bool m_halt_requested = false;

  virtual uint8_t ReadAddress(uint16_t address);
  virtual void WriteAddress(uint16_t address, uint8_t val);

  uint8_t ReadIo(uint16_t address);
  void WriteIo(uint16_t address, uint8_t val);

  uint16_t ReadNextUint16();

  uint8_t ReadNextUint8();

  inline int8_t ReadNextInt8()
  {
    uint8_t val = ReadAddress(_pc.hl);
    _pc.hl += 1;

    return static_cast<int8_t>(val);
  }

  inline int StopHandler()
  {
    throw std::exception("STOP not implemented.");

    return 4;
  }

  inline int HaltHandler() // TODO: Handle the HALT bug
  {
    if (m_interrupt_controller.InterruptExists())
      m_halt_requested = false;

    return 4;
  }

  inline int InterruptHandler()
  {
    uint16_t isr = m_interrupt_controller.HandleInterrupt();
    _pc.hl -= 1;
    PUSH_RR(_pc.hl);
    _pc.hl = isr;
    Fetch();

    return 20;
  }

  void Fetch()
  {
    _dataBus = ReadNextUint8();
    _opcode  = _dataBus;
  }

  double m_frame_time = 0;

  virtual void WaitFrame();
  void Init();

  int ExecuteCb(uint8_t opcode);
  void Update(int cycle_count);
#pragma endregion

#pragma region I/O CONTROLLERS
  InterruptController m_interrupt_controller;
  AudioController m_audio_controller;
  LcdController m_lcd_controller;
  JoypadController m_joypad_controller;
  SerialController m_serial_controller;
  TimerController m_timer_controller;
#pragma endregion

#pragma region INSTRUCTIONS
  using Instruction = int (Cpu::*)();
  constexpr static uint16_t NUM_INSTRUCTIONS = 256;

  enum class RestartVector
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

  void Op0x00();

  void Op0x01();

  void Op0x02();

  void Op0x03();

  void Op0x04();

  void Op0x05();

  void Op0x06();

  void Op0x07();

  inline int Op0x08()
  {
    LD__nn_rr(_sp.hl);
    return 20;
  }

  void Op0x09();

  void Op0x0A();

  void Op0x0B();

  void Op0x0C();

  void Op0x0D();

  void Op0x0E();

  void Op0x0F();

  void Op0x10();

  void Op0x11();

  void Op0x12();

  void Op0x13();

  void Op0x14();

  void Op0x15();

  void Op0x16();

  void Op0x17();

  inline int Op0x18()
  {
    JR(true);
    return 12;
  }

  void Op0x19();

  void Op0x1A();

  void Op0x1B();

  void Op0x1C();

  void Op0x1D();

  void Op0x1E();

  void Op0x1F();

  inline int Op0x20()
  {
    JR(!ReadFlag(FlagBitMask::Zero));
    return !ReadFlag(FlagBitMask::Zero) ? 12 : 8;
  }

  void Op0x21();

  void Op0x22();

  void Op0x23();

  void Op0x24();

  void Op0x25();

  void Op0x26();

  void Op0x27();

  inline int Op0x28()
  {
    JR(ReadFlag(FlagBitMask::Zero));
    return ReadFlag(FlagBitMask::Zero) ? 12 : 8;
  }

  void Op0x29();

  void Op0x2A();

  void Op0x2B();

  void Op0x2C();

  void Op0x2D();

  void Op0x2E();

  void Op0x2F();

  inline int Op0x30()
  {
    JR(!ReadFlag(FlagBitMask::Carry));
    return !ReadFlag(FlagBitMask::Carry) ? 12 : 8;
  }

  void Op0x31();

  void Op0x32();

  void Op0x33();

  void Op0x34();

  void Op0x35();

  inline int Op0x36()
  {
    WriteAddress(_hl.hl, ReadNextUint8());
    return 12;
  }

  void Op0x37();

  inline int Op0x38()
  {
    JR(ReadFlag(FlagBitMask::Carry));
    return ReadFlag(FlagBitMask::Carry) ? 12 : 8;
  }

  void Op0x39();

  void Op0x3A();

  void Op0x3B();

  void Op0x3C();

  void Op0x3D();

  void Op0x3E();

  void Op0x3F();

  void Op0x40();

  void Op0x41();

  void Op0x42();

  void Op0x43();

  void Op0x44();

  void Op0x45();

  void Op0x46();

  void Op0x47();

  void Op0x48();

  void Op0x49();

  void Op0x4A();

  void Op0x4B();

  void Op0x4C();

  void Op0x4D();

  void Op0x4E();

  void Op0x4F();

  void Op0x50();

  void Op0x51();

  void Op0x52();

  void Op0x53();

  void Op0x54();

  void Op0x55();

  void Op0x56();

  void Op0x57();

  void Op0x58();

  void Op0x59();

  void Op0x5A();

  void Op0x5B();

  void Op0x5C();

  void Op0x5D();

  void Op0x5E();

  void Op0x5F();

  void Op0x60();

  void Op0x61();

  void Op0x62();

  void Op0x63();

  void Op0x64();

  void Op0x65();

  void Op0x66();

  void Op0x67();

  void Op0x68();

  void Op0x69();

  void Op0x6A();

  void Op0x6B();

  void Op0x6C();

  void Op0x6D();

  void Op0x6E();

  void Op0x6F();

  void Op0x70();

  void Op0x71();

  void Op0x72();

  void Op0x73();

  void Op0x74();

  void Op0x75();

  void Op0x76();

  void Op0x77();

  void Op0x78();

  void Op0x79();

  void Op0x7A();

  void Op0x7B();

  void Op0x7C();

  void Op0x7D();

  void Op0x7E();

  void Op0x7F();

  void Op0x80();

  void Op0x81();

  void Op0x82();

  void Op0x83();

  void Op0x84();

  void Op0x85();

  inline int Op0x86()
  {
    ADD_A_X(ReadAddress(_hl.hl));
    return 8;
  }

  void Op0x87();

  void Op0x88();

  void Op0x89();

  void Op0x8A();

  void Op0x8B();

  void Op0x8C();

  void Op0x8D();

  inline int Op0x8E()
  {
    ADC_A_X(ReadAddress(_hl.hl));
    return 8;
  }

  void Op0x8F();

  void Op0x90();

  void Op0x91();

  void Op0x92();

  void Op0x93();

  void Op0x94();

  void Op0x95();

  inline int Op0x96()
  {
    SUB_A_X(ReadAddress(_hl.hl));
    return 8;
  }

  void Op0x97();

  void Op0x98();

  void Op0x99();

  void Op0x9A();

  void Op0x9B();

  void Op0x9C();

  void Op0x9D();

  inline int Op0x9E()
  {
    SBC_A_X(ReadAddress(_hl.hl));
    return 8;
  }

  void Op0x9F();

  inline int Op0xA0()
  {
    AND_A_X(_bc.h);
    return 4;
  }

  inline int Op0xA1()
  {
    AND_A_X(_bc.l);
    return 4;
  }

  inline int Op0xA2()
  {
    AND_A_X(_de.h);
    return 4;
  }

  inline int Op0xA3()
  {
    AND_A_X(_de.l);
    return 4;
  }

  inline int Op0xA4()
  {
    AND_A_X(_hl.h);
    return 4;
  }

  inline int Op0xA5()
  {
    AND_A_X(_hl.l);
    return 4;
  }

  inline int Op0xA6()
  {
    AND_A_X(ReadAddress(_hl.hl));
    return 8;
  }

  inline int Op0xA7()
  {
    AND_A_X(_af.h);
    return 4;
  }

  inline int Op0xA8()
  {
    XOR_A_X(_bc.h);
    return 4;
  }

  inline int Op0xA9()
  {
    XOR_A_X(_bc.l);
    return 4;
  }

  inline int Op0xAA()
  {
    XOR_A_X(_de.h);
    return 4;
  }

  inline int Op0xAB()
  {
    XOR_A_X(_de.l);
    return 4;
  }

  inline int Op0xAC()
  {
    XOR_A_X(_hl.h);
    return 4;
  }

  inline int Op0xAD()
  {
    XOR_A_X(_hl.l);
    return 4;
  }

  inline int Op0xAE()
  {
    XOR_A_X(ReadAddress(_hl.hl));
    return 8;
  }

  inline int Op0xAF()
  {
    XOR_A_X(_af.h);
    return 4;
  }

  inline int Op0xB0()
  {
    OR_A_X(_bc.h);
    return 4;
  }

  inline int Op0xB1()
  {
    OR_A_X(_bc.l);
    return 4;
  }

  inline int Op0xB2()
  {
    OR_A_X(_de.h);
    return 4;
  }

  inline int Op0xB3()
  {
    OR_A_X(_de.l);
    return 4;
  }

  inline int Op0xB4()
  {
    OR_A_X(_hl.h);
    return 4;
  }

  inline int Op0xB5()
  {
    OR_A_X(_hl.l);
    return 4;
  }

  inline int Op0xB6()
  {
    OR_A_X(ReadAddress(_hl.hl));
    return 8;
  }

  inline int Op0xB7()
  {
    OR_A_X(_af.h);
    return 4;
  }

  inline int Op0xB8()
  {
    CP_A_X(_bc.h);
    return 4;
  }

  inline int Op0xB9()
  {
    CP_A_X(_bc.l);
    return 4;
  }

  inline int Op0xBA()
  {
    CP_A_X(_de.h);
    return 4;
  }

  inline int Op0xBB()
  {
    CP_A_X(_de.l);
    return 4;
  }

  inline int Op0xBC()
  {
    CP_A_X(_hl.h);
    return 4;
  }

  inline int Op0xBD()
  {
    CP_A_X(_hl.l);
    return 4;
  }

  inline int Op0xBE()
  {
    CP_A_X(ReadAddress(_hl.hl));
    return 8;
  }

  inline int Op0xBF()
  {
    CP_A_X(_af.h);
    return 4;
  }

  inline int Op0xC0()
  {
    RET(!ReadFlag(FlagBitMask::Zero));
    return !ReadFlag(FlagBitMask::Zero) ? 20 : 8;
  }

  inline int Op0xC1()
  {
    POP_RR(_bc.hl);
    return 12;
  }

  inline int Op0xC2()
  {
    JP(!ReadFlag(FlagBitMask::Zero));
    return !ReadFlag(FlagBitMask::Zero) ? 16 : 12;
  }

  inline int Op0xC3()
  {
    JP(true);
    return 16;
  }

  inline int Op0xC4()
  {
    CALL(!ReadFlag(FlagBitMask::Zero));
    return !ReadFlag(FlagBitMask::Zero) ? 24 : 12;
  }

  inline int Op0xC5()
  {
    PUSH_RR(_bc.hl);
    return 16;
  }

  inline int Op0xC6()
  {
    ADD_A_X(ReadNextUint8());
    return 8;
  }

  inline int Op0xC7()
  {
    RST(RestartVector::RST0);
    return 16;
  }

  inline int Op0xC8()
  {
    RET(ReadFlag(FlagBitMask::Zero));
    return ReadFlag(FlagBitMask::Zero) ? 20 : 8;
  }

  inline int Op0xC9()
  {
    RET(true);
    return 16;
  }

  inline int Op0xCA()
  {
    JP(ReadFlag(FlagBitMask::Zero));
    return ReadFlag(FlagBitMask::Zero) ? 16 : 12;
  }

  inline int Op0xCB()
  {
    uint8_t extended_opcode = ReadNextUint8();
    return ExecuteCb(extended_opcode);
    //return (this->*ExtendedInstructionSet[extended_opcode])();
  }

  inline int Op0xCC()
  {
    CALL(ReadFlag(FlagBitMask::Zero));
    return ReadFlag(FlagBitMask::Zero) ? 24 : 12;
  }

  inline int Op0xCD()
  {
    CALL(true);
    return 24;
  }

  inline int Op0xCE()
  {
    ADC_A_X(ReadNextUint8());
    return 8;
  }

  inline int Op0xCF()
  {
    RST(RestartVector::RST1);
    return 16;
  }

  inline int Op0xD0()
  {
    RET(!ReadFlag(FlagBitMask::Carry));
    return !ReadFlag(FlagBitMask::Carry) ? 20 : 8;
  }

  inline int Op0xD1()
  {
    POP_RR(_de.hl);
    return 12;
  }

  inline int Op0xD2()
  {
    JP(!ReadFlag(FlagBitMask::Carry));
    return !ReadFlag(FlagBitMask::Carry) ? 16 : 12;
  }

  inline int Op0xD3()
  {
    throw std::exception("Invalid Instruction: 0xD3");
  }

  inline int Op0xD4()
  {
    CALL(!ReadFlag(FlagBitMask::Carry));
    return !ReadFlag(FlagBitMask::Carry) ? 24 : 12;
  }

  inline int Op0xD5()
  {
    PUSH_RR(_de.hl);
    return 16;
  }

  inline int Op0xD6()
  {
    SUB_A_X(ReadNextUint8());
    return 8;
  }

  inline int Op0xD7()
  {
    RST(RestartVector::RST2);
    return 16;
  }

  inline int Op0xD8()
  {
    RET(ReadFlag(FlagBitMask::Carry));
    return ReadFlag(FlagBitMask::Carry) ? 20 : 8;
  }

  inline int Op0xD9()
  {
    RETI();
    return 16;
  }

  inline int Op0xDA()
  {
    JP(ReadFlag(FlagBitMask::Carry));
    return ReadFlag(FlagBitMask::Carry) ? 16 : 12;
  }

  inline int Op0xDB()
  {
    throw std::exception("Invalid Instruction: 0xDB");
  }

  inline int Op0xDC()
  {
    CALL(ReadFlag(FlagBitMask::Carry));
    return ReadFlag(FlagBitMask::Carry) ? 24 : 12;
  }

  inline int Op0xDD()
  {
    throw std::exception("Invalid Instruction: 0xDD");
  }

  inline int Op0xDE()
  {
    SBC_A_X(ReadNextUint8());
    return 8;
  }

  inline int Op0xDF()
  {
    RST(RestartVector::RST3);
    return 16;
  }

  inline int Op0xE0()
  {
    LD__R_A_IO(ReadNextUint8());
    return 12;
  }

  inline int Op0xE1()
  {
    POP_RR(_hl.hl);
    return 12;
  }

  inline int Op0xE2()
  {
    LD__R_A_IO(_bc.l);
    return 8;
  }

  inline int Op0xE3()
  {
    throw std::exception("Invalid Instruction: 0xE3");
  }

  inline int Op0xE4()
  {
    throw std::exception("Invalid Instruction: 0xE4");
  }

  inline int Op0xE5()
  {
    PUSH_RR(_hl.hl);
    return 16;
  }

  inline int Op0xE6()
  {
    AND_A_X(ReadNextUint8());
    return 8;
  }

  inline int Op0xE7()
  {
    RST(RestartVector::RST4);
    return 16;
  }

  inline int Op0xE8()
  {
    ADD_SP_s();
    return 16;
  }

  inline int Op0xE9()
  {
    _pc.hl = _hl.hl;
    return 4;
  }

  inline int Op0xEA()
  {
    //LD__RR_A(ReadNextUint16());
    return 16;
  }

  inline int Op0xEB()
  {
    throw std::exception("Invalid Instruction: 0xEB");
  }

  inline int Op0xEC()
  {
    throw std::exception("Invalid Instruction: 0xEC");
  }

  inline int Op0xED()
  {
    throw std::exception("Invalid Instruction: 0xED");
  }

  inline int Op0xEE()
  {
    XOR_A_X(ReadNextUint8());
    return 8;
  }

  inline int Op0xEF()
  {
    RST(RestartVector::RST5);
    return 16;
  }

  inline int Op0xF0()
  {
    LD_A__R_IO(ReadNextUint8());
    return 12;
  }

  inline int Op0xF1()
  {
    POP_AF();
    return 12;
  }

  inline int Op0xF2()
  {
    LD_A__R_IO(_bc.l);
    return 8;
  }

  inline int Op0xF3()
  {
    m_interrupt_controller.DisableInterrupts();
    return 4;
  }

  inline int Op0xF4()
  {
    throw std::exception("Invalid Instruction: 0xF4");
  }

  inline int Op0xF5()
  {
    PUSH_RR(_af.hl);
    return 16;
  }

  inline int Op0xF6()
  {
    OR_A_X(ReadNextUint8());
    return 8;
  }

  inline int Op0xF7()
  {
    RST(RestartVector::RST6);
    return 16;
  }

  inline int Op0xF8()
  {
    LD_HL_SP_s();
    return 12;
  }

  inline int Op0xF9()
  {
    _sp.hl = _hl.hl;
    return 8;
  }

  inline int Op0xFA()
  {
    LD_r_x(_af.h, ReadAddress(ReadNextUint16()));
    return 16;
  }

  inline int Op0xFB()
  {
    m_interrupt_enabled_requested = true;
    return 4;
  }

  inline int Op0xFC()
  {
    throw std::exception("Invalid Instruction: 0xFC");
  }

  inline int Op0xFD()
  {
    throw std::exception("Invalid Instruction: 0xFD");
  }

  inline int Op0xFE()
  {
    CP_A_X(ReadNextUint8());
    return 8;
  }

  inline int Op0xFF()
  {
    RST(RestartVector::RST7);
    return 16;
  }
  //Instruction InstructionSet[NUM_INSTRUCTIONS]
  //{
  //  &Cpu::Op0x00, &Cpu::Op0x01, &Cpu::Op0x02, &Cpu::Op0x03, &Cpu::Op0x04, &Cpu::Op0x05, &Cpu::Op0x06, &Cpu::Op0x07, &Cpu::Op0x08, &Cpu::Op0x09, &Cpu::Op0x0A, &Cpu::Op0x0B, &Cpu::Op0x0C, &Cpu::Op0x0D, &Cpu::Op0x0E, &Cpu::Op0x0F,
  //  &Cpu::Op0x10, &Cpu::Op0x11, &Cpu::Op0x12, &Cpu::Op0x13, &Cpu::Op0x14, &Cpu::Op0x15, &Cpu::Op0x16, &Cpu::Op0x17, &Cpu::Op0x18, &Cpu::Op0x19, &Cpu::Op0x1A, &Cpu::Op0x1B, &Cpu::Op0x1C, &Cpu::Op0x1D, &Cpu::Op0x1E, &Cpu::Op0x1F,
  //  &Cpu::Op0x20, &Cpu::Op0x21, &Cpu::Op0x22, &Cpu::Op0x23, &Cpu::Op0x24, &Cpu::Op0x25, &Cpu::Op0x26, &Cpu::Op0x27, &Cpu::Op0x28, &Cpu::Op0x29, &Cpu::Op0x2A, &Cpu::Op0x2B, &Cpu::Op0x2C, &Cpu::Op0x2D, &Cpu::Op0x2E, &Cpu::Op0x2F,
  //  &Cpu::Op0x30, &Cpu::Op0x31, &Cpu::Op0x32, &Cpu::Op0x33, &Cpu::Op0x34, &Cpu::Op0x35, &Cpu::Op0x36, &Cpu::Op0x37, &Cpu::Op0x38, &Cpu::Op0x39, &Cpu::Op0x3A, &Cpu::Op0x3B, &Cpu::Op0x3C, &Cpu::Op0x3D, &Cpu::Op0x3E, &Cpu::Op0x3F,
  //  &Cpu::Op0x40, &Cpu::Op0x41, &Cpu::Op0x42, &Cpu::Op0x43, &Cpu::Op0x44, &Cpu::Op0x45, &Cpu::Op0x46, &Cpu::Op0x47, &Cpu::Op0x48, &Cpu::Op0x49, &Cpu::Op0x4A, &Cpu::Op0x4B, &Cpu::Op0x4C, &Cpu::Op0x4D, &Cpu::Op0x4E, &Cpu::Op0x4F,
  //  &Cpu::Op0x50, &Cpu::Op0x51, &Cpu::Op0x52, &Cpu::Op0x53, &Cpu::Op0x54, &Cpu::Op0x55, &Cpu::Op0x56, &Cpu::Op0x57, &Cpu::Op0x58, &Cpu::Op0x59, &Cpu::Op0x5A, &Cpu::Op0x5B, &Cpu::Op0x5C, &Cpu::Op0x5D, &Cpu::Op0x5E, &Cpu::Op0x5F,
  //  &Cpu::Op0x60, &Cpu::Op0x61, &Cpu::Op0x62, &Cpu::Op0x63, &Cpu::Op0x64, &Cpu::Op0x65, &Cpu::Op0x66, &Cpu::Op0x67, &Cpu::Op0x68, &Cpu::Op0x69, &Cpu::Op0x6A, &Cpu::Op0x6B, &Cpu::Op0x6C, &Cpu::Op0x6D, &Cpu::Op0x6E, &Cpu::Op0x6F,
  //  &Cpu::Op0x70, &Cpu::Op0x71, &Cpu::Op0x72, &Cpu::Op0x73, &Cpu::Op0x74, &Cpu::Op0x75, &Cpu::Op0x76, &Cpu::Op0x77, &Cpu::Op0x78, &Cpu::Op0x79, &Cpu::Op0x7A, &Cpu::Op0x7B, &Cpu::Op0x7C, &Cpu::Op0x7D, &Cpu::Op0x7E, &Cpu::Op0x7F,
  //  &Cpu::Op0x80, &Cpu::Op0x81, &Cpu::Op0x82, &Cpu::Op0x83, &Cpu::Op0x84, &Cpu::Op0x85, &Cpu::Op0x86, &Cpu::Op0x87, &Cpu::Op0x88, &Cpu::Op0x89, &Cpu::Op0x8A, &Cpu::Op0x8B, &Cpu::Op0x8C, &Cpu::Op0x8D, &Cpu::Op0x8E, &Cpu::Op0x8F,
  //  &Cpu::Op0x90, &Cpu::Op0x91, &Cpu::Op0x92, &Cpu::Op0x93, &Cpu::Op0x94, &Cpu::Op0x95, &Cpu::Op0x96, &Cpu::Op0x97, &Cpu::Op0x98, &Cpu::Op0x99, &Cpu::Op0x9A, &Cpu::Op0x9B, &Cpu::Op0x9C, &Cpu::Op0x9D, &Cpu::Op0x9E, &Cpu::Op0x9F,
  //  &Cpu::Op0xA0, &Cpu::Op0xA1, &Cpu::Op0xA2, &Cpu::Op0xA3, &Cpu::Op0xA4, &Cpu::Op0xA5, &Cpu::Op0xA6, &Cpu::Op0xA7, &Cpu::Op0xA8, &Cpu::Op0xA9, &Cpu::Op0xAA, &Cpu::Op0xAB, &Cpu::Op0xAC, &Cpu::Op0xAD, &Cpu::Op0xAE, &Cpu::Op0xAF,
  //  &Cpu::Op0xB0, &Cpu::Op0xB1, &Cpu::Op0xB2, &Cpu::Op0xB3, &Cpu::Op0xB4, &Cpu::Op0xB5, &Cpu::Op0xB6, &Cpu::Op0xB7, &Cpu::Op0xB8, &Cpu::Op0xB9, &Cpu::Op0xBA, &Cpu::Op0xBB, &Cpu::Op0xBC, &Cpu::Op0xBD, &Cpu::Op0xBE, &Cpu::Op0xBF,
  //  &Cpu::Op0xC0, &Cpu::Op0xC1, &Cpu::Op0xC2, &Cpu::Op0xC3, &Cpu::Op0xC4, &Cpu::Op0xC5, &Cpu::Op0xC6, &Cpu::Op0xC7, &Cpu::Op0xC8, &Cpu::Op0xC9, &Cpu::Op0xCA, &Cpu::Op0xCB, &Cpu::Op0xCC, &Cpu::Op0xCD, &Cpu::Op0xCE, &Cpu::Op0xCF,
  //  &Cpu::Op0xD0, &Cpu::Op0xD1, &Cpu::Op0xD2, &Cpu::Op0xD3, &Cpu::Op0xD4, &Cpu::Op0xD5, &Cpu::Op0xD6, &Cpu::Op0xD7, &Cpu::Op0xD8, &Cpu::Op0xD9, &Cpu::Op0xDA, &Cpu::Op0xDB, &Cpu::Op0xDC, &Cpu::Op0xDD, &Cpu::Op0xDE, &Cpu::Op0xDF,
  //  &Cpu::Op0xE0, &Cpu::Op0xE1, &Cpu::Op0xE2, &Cpu::Op0xE3, &Cpu::Op0xE4, &Cpu::Op0xE5, &Cpu::Op0xE6, &Cpu::Op0xE7, &Cpu::Op0xE8, &Cpu::Op0xE9, &Cpu::Op0xEA, &Cpu::Op0xEB, &Cpu::Op0xEC, &Cpu::Op0xED, &Cpu::Op0xEE, &Cpu::Op0xEF,
  //  &Cpu::Op0xF0, &Cpu::Op0xF1, &Cpu::Op0xF2, &Cpu::Op0xF3, &Cpu::Op0xF4, &Cpu::Op0xF5, &Cpu::Op0xF6, &Cpu::Op0xF7, &Cpu::Op0xF8, &Cpu::Op0xF9, &Cpu::Op0xFA, &Cpu::Op0xFB, &Cpu::Op0xFC, &Cpu::Op0xFD, &Cpu::Op0xFE, &Cpu::Op0xFF
  //};
#pragma endregion

#pragma region GENERALIZED INSTRUCTIONS
  //
  // Naming convention:
  //   _ denotes a register
  //  __ denotes an address
  //   r denotes an 8-bit register
  //  rr denotes a 16-bit register
  //   n denotes the next 8-bit value
  //  nn denotes the next 16-bit value
  //   s denotes the next signed 8-bit value
  //  ss denotes the next signed 16-bit value
  // 
  //   x denotes wildcard
  //
  //  Some functions are specific to a register.
  //  These functions will denote the register in the name.
  //
  //  e.g. LD__rr_a -> Load the value of A into the address pointed to by RR
  //

  // Flags: - - - -
  void NOP();

  // Flags: - - - -
  void LD_r_x(uint8_t& r, uint8_t val);

  // Flags: - - - -
  void LD_r__HL(uint8_t& r);

  // Flags: - - - -
  void LD_r_n(uint8_t& r);

  // Flags: - - - -
  void LD_A__RR(Register rr);

  // Flags: - - - -
  void LD_A__HLx(int val);

  // Flags: - - - -
  inline void LD__nn_rr(uint16_t RR)
  {
    uint16_t address = ReadNextUint16();
    WriteAddress(address, RR & 0xFF);
    WriteAddress(address + 1, RR >> 8);
  }

  // Flags: - - - -
  void LD_rr_nn(Register& rr);

  // Flags: - - - -
  void LD__rr_r(Register rr, uint8_t r);

  // Flags: - - - -
  void LD__rr_A(Register rr);

  void LD__HLx_A(int val);

  // Flags: - - - -
  inline void LD__R_A_IO(uint8_t R)
  {
    WriteAddress(static_cast<uint16_t>(ExecutionAddress::IO) + R, _af.h);
  }

  // Flags: - - - -
  inline void LD_A__R_IO(uint8_t R)
  {
    _af.h = ReadAddress(static_cast<uint16_t>(ExecutionAddress::IO) + R);
  }

  // Flags: 0 0 H C
  inline void LD_HL_SP_s()
  {
    int8_t val = ReadNextInt8();
    int result = _sp.hl + val;
    ResetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    SetHalfCarryFlag(static_cast<uint8_t>(_sp.hl), static_cast<uint8_t>(val));
    SetCarry8Bit(static_cast<uint8_t>(_sp.hl) + static_cast<uint8_t>(val));
    _hl.hl = static_cast<uint16_t>(result);
  }

  // Flags: 0 0 H C
  inline void ADD_SP_s()
  {
    int8_t val = ReadNextInt8();
    int result = _sp.hl + val;
    ResetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    SetHalfCarryFlag(static_cast<uint8_t>(_sp.hl), static_cast<uint8_t>(val));
    SetCarry8Bit(static_cast<uint8_t>(_sp.hl) + static_cast<uint8_t>(val));
    _sp.hl = static_cast<uint16_t>(result);
  }

  // Flags: - - - -
  void ADD_rr(Register& RR, int val);

  // Flags: Z 0 H C
  void ADC_A_X(int val);

  // Flags: Z 0 H C
  void ADD_A_X(int val);

  // Flags: Z 1 H C
  void SBC_A_X(int val);

  // Flags: Z 1 H C
  void SUB_A_X(int val);

  // Flags: Z - H C
  void ADD_x(uint8_t& R, int val, bool set_carry = false);

  // Flags: Z N H -
  void ADD__HL(int val);

  // Flags: - 0 H C
  void ADD_HL_rr(Register RR);

  // Flags: - 0 H C
  void ADD_HL_rr(uint16_t RR);

  // Flags: Z 0 1 0
  inline void AND_A_X(uint8_t val)
  {
    _af.h &= val;
    SetZeroFlag(_af.h);
    ResetFlag(FlagBitMask::Subtract);
    SetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
  }

  // Flags: Z 0 0 0
  inline void XOR_A_X(uint8_t val)
  {
    _af.h ^= val;
    SetZeroFlag(_af.h);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
  }

  // Flags: Z 0 0 0
  inline void OR_A_X(uint8_t val)
  {
    _af.h |= val;
    SetZeroFlag(_af.h);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
  }

  // Flags: Z 1 H C
  inline void CP_A_X(uint8_t val)
  {
    int result = _af.h - val;
    SetZeroFlag(result);
    SetFlag(FlagBitMask::Subtract);
    SetHalfCarryFlag(_af.h, -val);
    SetCarry8Bit(result);
  }

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

  // Flags: Z - 0 C
  // https://forums.nesdev.org/viewtopic.php?t=15944#:~:text=The%20DAA%20instruction%20adjusts%20the,%2C%20lower%20nybble%2C%20or%20both.
  inline void DAA();

  // Flags: - 0 0 1
  void SCF();

  // Flags: - 0 0 !C
  void CCF();

  // Flags: Z 0 H -
  void INC_R(uint8_t& R);

  // Flags: Z 1 H -
  void DEC_R(uint8_t& R);

  inline void PUSH_RR(uint16_t RR)
  {
    _sp.hl -= 2;
    WriteAddress(_sp.hl, RR & 0xFF);
    WriteAddress(_sp.hl + 1, RR >> 8);
  }

  // Flags: - - - -
  inline void JR(bool jump)
  {
    int8_t steps = ReadNextInt8();
    if (jump) _pc.hl += steps;
  }

  // Flags: - - - -
  inline void POP_RR(uint16_t& RR)
  {
    uint8_t l = ReadAddress(_sp.hl);
    _sp.hl += 1;
    uint8_t h = ReadAddress(_sp.hl);
    _sp.hl += 1;

    RR = (h << 8) | l;
  }

  // Flags: - - - -
  inline void POP_AF()
  {
    uint8_t l = ReadAddress(_sp.hl);
    _sp.hl += 1;
    uint8_t h = ReadAddress(_sp.hl);
    _sp.hl += 1;

    _af.hl = (h << 8) | l;
    _af.l &= 0xF0;
  }

  // Flags: - - - -
  inline void RET(bool ret)
  {
    if (ret) POP_RR(_pc.hl);
  }

  // Flags: - - - -
  inline void RETI()
  {
    m_interrupt_controller.EnableInterrupts();
    RET(true);
  }

  // Flags: - - - -
  inline void JP(bool jump)
  {
    uint16_t address = ReadNextUint16();
    if (jump) _pc.hl = address;
  }

  // Flags: - - - -
  inline void CALL(bool call)
  {
    uint16_t func = ReadNextUint16();
    if (call) { PUSH_RR(_pc.hl); _pc.hl = func; }
  }

  // Flags: - - - -
  inline void RST(RestartVector rst_vec)
  {
    PUSH_RR(_pc.hl);
    _pc.hl = static_cast<uint16_t>(rst_vec);
  }
#pragma endregion

#pragma region EXTENDED INSTRUCTIONS
  inline int OpCb0x00()
  {
    RLC_R(_bc.h);
    return 8;
  }

  inline int OpCb0x01()
  {
    RLC_R(_bc.l);
    return 8;
  }

  inline int OpCb0x02()
  {
    RLC_R(_de.h);
    return 8;
  }

  inline int OpCb0x03()
  {
    RLC_R(_de.l);
    return 8;
  }

  inline int OpCb0x04()
  {
    RLC_R(_hl.h);
    return 8;
  }

  inline int OpCb0x05()
  {
    RLC_R(_hl.l);
    return 8;
  }

  inline int OpCb0x06()
  {
    RLC__HL();
    return 16;
  }

  inline int OpCb0x07()
  {
    RLC_R(_af.h);
    return 8;
  }

  inline int OpCb0x08()
  {
    RRC_R(_bc.h);
    return 8;
  }

  inline int OpCb0x09()
  {
    RRC_R(_bc.l);
    return 8;
  }

  inline int OpCb0x0A()
  {
    RRC_R(_de.h);
    return 8;
  }

  inline int OpCb0x0B()
  {
    RRC_R(_de.l);
    return 8;
  }

  inline int OpCb0x0C()
  {
    RRC_R(_hl.h);
    return 8;
  }

  inline int OpCb0x0D()
  {
    RRC_R(_hl.l);
    return 8;
  }

  inline int OpCb0x0E()
  {
    RRC__HL();
    return 16;
  }

  inline int OpCb0x0F()
  {
    RRC_R(_af.h);
    return 8;
  }

  inline int OpCb0x10()
  {
    RL_R(_bc.h);
    return 8;
  }

  inline int OpCb0x11()
  {
    RL_R(_bc.l);
    return 8;
  }

  inline int OpCb0x12()
  {
    RL_R(_de.h);
    return 8;
  }

  inline int OpCb0x13()
  {
    RL_R(_de.l);
    return 8;
  }

  inline int OpCb0x14()
  {
    RL_R(_hl.h);
    return 8;
  }

  inline int OpCb0x15()
  {
    RL_R(_hl.l);
    return 8;
  }

  inline int OpCb0x16()
  {
    RL__HL();
    return 16;
  }

  inline int OpCb0x17()
  {
    RL_R(_af.h);
    return 8;
  }

  inline int OpCb0x18()
  {
    RR_R(_bc.h);
    return 8;
  }

  inline int OpCb0x19()
  {
    RR_R(_bc.l);
    return 8;
  }

  inline int OpCb0x1A()
  {
    RR_R(_de.h);
    return 8;
  }

  inline int OpCb0x1B()
  {
    RR_R(_de.l);
    return 8;
  }

  inline int OpCb0x1C()
  {
    RR_R(_hl.h);
    return 8;
  }

  inline int OpCb0x1D()
  {
    RR_R(_hl.l);
    return 8;
  }

  inline int OpCb0x1E()
  {
    RR__HL();
    return 16;
  }

  inline int OpCb0x1F()
  {
    RR_R(_af.h);
    return 8;
  }

  inline int OpCb0x20()
  {
    SLA_R(_bc.h);
    return 8;
  }

  inline int OpCb0x21()
  {
    SLA_R(_bc.l);
    return 8;
  }

  inline int OpCb0x22()
  {
    SLA_R(_de.h);
    return 8;
  }

  inline int OpCb0x23()
  {
    SLA_R(_de.l);
    return 8;
  }

  inline int OpCb0x24()
  {
    SLA_R(_hl.h);
    return 8;
  }

  inline int OpCb0x25()
  {
    SLA_R(_hl.l);
    return 8;
  }

  inline int OpCb0x26()
  {
    SLA__HL();
    return 16;
  }

  inline int OpCb0x27()
  {
    SLA_R(_af.h);
    return 8;
  }

  inline int OpCb0x28()
  {
    SRA_R(_bc.h);
    return 8;
  }

  inline int OpCb0x29()
  {
    SRA_R(_bc.l);
    return 8;
  }

  inline int OpCb0x2A()
  {
    SRA_R(_de.h);
    return 8;
  }

  inline int OpCb0x2B()
  {
    SRA_R(_de.l);
    return 8;
  }

  inline int OpCb0x2C()
  {
    SRA_R(_hl.h);
    return 8;
  }

  inline int OpCb0x2D()
  {
    SRA_R(_hl.l);
    return 8;
  }

  inline int OpCb0x2E()
  {
    SRA__HL();
    return 16;
  }

  inline int OpCb0x2F()
  {
    SRA_R(_af.h);
    return 8;
  }

  inline int OpCb0x30()
  {
    SWAP_R(_bc.h);
    return 8;
  }

  inline int OpCb0x31()
  {
    SWAP_R(_bc.l);
    return 8;
  }

  inline int OpCb0x32()
  {
    SWAP_R(_de.h);
    return 8;
  }

  inline int OpCb0x33()
  {
    SWAP_R(_de.l);
    return 8;
  }

  inline int OpCb0x34()
  {
    SWAP_R(_hl.h);
    return 8;
  }

  inline int OpCb0x35()
  {
    SWAP_R(_hl.l);
    return 8;
  }

  inline int OpCb0x36()
  {
    SWAP__HL();
    return 16;
  }

  inline int OpCb0x37()
  {
    SWAP_R(_af.h);
    return 8;
  }

  inline int OpCb0x38()
  {
    SRL_R(_bc.h);
    return 8;
  }

  inline int OpCb0x39()
  {
    SRL_R(_bc.l);
    return 8;
  }

  inline int OpCb0x3A()
  {
    SRL_R(_de.h);
    return 8;
  }

  inline int OpCb0x3B()
  {
    SRL_R(_de.l);
    return 8;
  }

  inline int OpCb0x3C()
  {
    SRL_R(_hl.h);
    return 8;
  }

  inline int OpCb0x3D()
  {
    SRL_R(_hl.l);
    return 8;
  }

  inline int OpCb0x3E()
  {
    SRL__HL();
    return 16;
  }

  inline int OpCb0x3F()
  {
    SRL_R(_af.h);
    return 8;
  }

  inline int OpCb0x40()
  {
    BIT_R(0b00000001, _bc.h);
    return 8;
  }

  inline int OpCb0x41()
  {
    BIT_R(0b00000001, _bc.l);
    return 8;
  }

  inline int OpCb0x42()
  {
    BIT_R(0b00000001, _de.h);
    return 8;
  }

  inline int OpCb0x43()
  {
    BIT_R(0b00000001, _de.l);
    return 8;
  }

  inline int OpCb0x44()
  {
    BIT_R(0b00000001, _hl.h);
    return 8;
  }

  inline int OpCb0x45()
  {
    BIT_R(0b00000001, _hl.l);
    return 8;
  }

  inline int OpCb0x46()
  {
    BIT__HL(0b00000001);
    return 12;
  }

  inline int OpCb0x47()
  {
    BIT_R(0b00000001, _af.h);
    return 8;
  }

  inline int OpCb0x48()
  {
    BIT_R(0b00000010, _bc.h);
    return 8;
  }

  inline int OpCb0x49()
  {
    BIT_R(0b00000010, _bc.l);
    return 8;
  }

  inline int OpCb0x4A()
  {
    BIT_R(0b00000010, _de.h);
    return 8;
  }

  inline int OpCb0x4B()
  {
    BIT_R(0b00000010, _de.l);
    return 8;
  }

  inline int OpCb0x4C()
  {
    BIT_R(0b00000010, _hl.h);
    return 8;
  }

  inline int OpCb0x4D()
  {
    BIT_R(0b00000010, _hl.l);
    return 8;
  }

  inline int OpCb0x4E()
  {
    BIT__HL(0b00000010);
    return 12;
  }

  inline int OpCb0x4F()
  {
    BIT_R(0b00000010, _af.h);
    return 8;
  }

  inline int OpCb0x50()
  {
    BIT_R(0b00000100, _bc.h);
    return 8;
  }

  inline int OpCb0x51()
  {
    BIT_R(0b00000100, _bc.l);
    return 8;
  }

  inline int OpCb0x52()
  {
    BIT_R(0b00000100, _de.h);
    return 8;
  }

  inline int OpCb0x53()
  {
    BIT_R(0b00000100, _de.l);
    return 8;
  }

  inline int OpCb0x54()
  {
    BIT_R(0b00000100, _hl.h);
    return 8;
  }

  inline int OpCb0x55()
  {
    BIT_R(0b00000100, _hl.l);
    return 8;
  }

  inline int OpCb0x56()
  {
    BIT__HL(0b00000100);
    return 12;
  }

  inline int OpCb0x57()
  {
    BIT_R(0b00000100, _af.h);
    return 8;
  }

  inline int OpCb0x58()
  {
    BIT_R(0b00001000, _bc.h);
    return 8;
  }

  inline int OpCb0x59()
  {
    BIT_R(0b00001000, _bc.l);
    return 8;
  }

  inline int OpCb0x5A()
  {
    BIT_R(0b00001000, _de.h);
    return 8;
  }

  inline int OpCb0x5B()
  {
    BIT_R(0b00001000, _de.l);
    return 8;
  }

  inline int OpCb0x5C()
  {
    BIT_R(0b00001000, _hl.h);
    return 8;
  }

  inline int OpCb0x5D()
  {
    BIT_R(0b00001000, _hl.l);
    return 8;
  }

  inline int OpCb0x5E()
  {
    BIT__HL(0b00001000);
    return 12;
  }

  inline int OpCb0x5F()
  {
    BIT_R(0b00001000, _af.h);
    return 8;
  }

  inline int OpCb0x60()
  {
    BIT_R(0b00010000, _bc.h);
    return 8;
  }

  inline int OpCb0x61()
  {
    BIT_R(0b00010000, _bc.l);
    return 8;
  }

  inline int OpCb0x62()
  {
    BIT_R(0b00010000, _de.h);
    return 8;
  }

  inline int OpCb0x63()
  {
    BIT_R(0b00010000, _de.l);
    return 8;
  }

  inline int OpCb0x64()
  {
    BIT_R(0b00010000, _hl.h);
    return 8;
  }

  inline int OpCb0x65()
  {
    BIT_R(0b00010000, _hl.l);
    return 8;
  }

  inline int OpCb0x66()
  {
    BIT__HL(0b00010000);
    return 12;
  }

  inline int OpCb0x67()
  {
    BIT_R(0b00010000, _af.h);
    return 8;
  }

  inline int OpCb0x68()
  {
    BIT_R(0b00100000, _bc.h);
    return 8;
  }

  inline int OpCb0x69()
  {
    BIT_R(0b00100000, _bc.l);
    return 8;
  }

  inline int OpCb0x6A()
  {
    BIT_R(0b00100000, _de.h);
    return 8;
  }

  inline int OpCb0x6B()
  {
    BIT_R(0b00100000, _de.l);
    return 8;
  }

  inline int OpCb0x6C()
  {
    BIT_R(0b00100000, _hl.h);
    return 8;
  }

  inline int OpCb0x6D()
  {
    BIT_R(0b00100000, _hl.l);
    return 8;
  }

  inline int OpCb0x6E()
  {
    BIT__HL(0b00100000);
    return 12;
  }

  inline int OpCb0x6F()
  {
    BIT_R(0b00100000, _af.h);
    return 8;
  }

  inline int OpCb0x70()
  {
    BIT_R(0b01000000, _bc.h);
    return 8;
  }

  inline int OpCb0x71()
  {
    BIT_R(0b01000000, _bc.l);
    return 8;
  }

  inline int OpCb0x72()
  {
    BIT_R(0b01000000, _de.h);
    return 8;
  }

  inline int OpCb0x73()
  {
    BIT_R(0b01000000, _de.l);
    return 8;
  }

  inline int OpCb0x74()
  {
    BIT_R(0b01000000, _hl.h);
    return 8;
  }

  inline int OpCb0x75()
  {
    BIT_R(0b01000000, _hl.l);
    return 8;
  }

  inline int OpCb0x76()
  {
    BIT__HL(0b01000000);
    return 12;
  }

  inline int OpCb0x77()
  {
    BIT_R(0b01000000, _af.h);
    return 8;
  }

  inline int OpCb0x78()
  {
    BIT_R(0b10000000, _bc.h);
    return 8;
  }

  inline int OpCb0x79()
  {
    BIT_R(0b10000000, _bc.l);
    return 8;
  }

  inline int OpCb0x7A()
  {
    BIT_R(0b10000000, _de.h);
    return 8;
  }

  inline int OpCb0x7B()
  {
    BIT_R(0b10000000, _de.l);
    return 8;
  }

  inline int OpCb0x7C()
  {
    BIT_R(0b10000000, _hl.h);
    return 8;
  }

  inline int OpCb0x7D()
  {
    BIT_R(0b10000000, _hl.l);
    return 8;
  }

  inline int OpCb0x7E()
  {
    BIT__HL(0b10000000);
    return 12;
  }

  inline int OpCb0x7F()
  {
    BIT_R(0b10000000, _af.h);
    return 8;
  }

  inline int OpCb0x80()
  {
    RES_R(0b00000001, _bc.h);
    return 8;
  }

  inline int OpCb0x81()
  {
    RES_R(0b00000001, _bc.l);
    return 8;
  }

  inline int OpCb0x82()
  {
    RES_R(0b00000001, _de.h);
    return 8;
  }

  inline int OpCb0x83()
  {
    RES_R(0b00000001, _de.l);
    return 8;
  }

  inline int OpCb0x84()
  {
    RES_R(0b00000001, _hl.h);
    return 8;
  }

  inline int OpCb0x85()
  {
    RES_R(0b00000001, _hl.l);
    return 8;
  }

  inline int OpCb0x86()
  {
    RES__HL(0b00000001);
    return 16;
  }

  inline int OpCb0x87()
  {
    RES_R(0b00000001, _af.h);
    return 8;
  }

  inline int OpCb0x88()
  {
    RES_R(0b00000010, _bc.h);
    return 8;
  }

  inline int OpCb0x89()
  {
    RES_R(0b00000010, _bc.l);
    return 8;
  }

  inline int OpCb0x8A()
  {
    RES_R(0b00000010, _de.h);
    return 8;
  }

  inline int OpCb0x8B()
  {
    RES_R(0b00000010, _de.l);
    return 8;
  }

  inline int OpCb0x8C()
  {
    RES_R(0b00000010, _hl.h);
    return 8;
  }

  inline int OpCb0x8D()
  {
    RES_R(0b00000010, _hl.l);
    return 8;
  }

  inline int OpCb0x8E()
  {
    RES__HL(0b00000010);
    return 16;
  }

  inline int OpCb0x8F()
  {
    RES_R(0b00000010, _af.h);
    return 8;
  }

  inline int OpCb0x90()
  {
    RES_R(0b00000100, _bc.h);
    return 8;
  }

  inline int OpCb0x91()
  {
    RES_R(0b00000100, _bc.l);
    return 8;
  }

  inline int OpCb0x92()
  {
    RES_R(0b00000100, _de.h);
    return 8;
  }

  inline int OpCb0x93()
  {
    RES_R(0b00000100, _de.l);
    return 8;
  }

  inline int OpCb0x94()
  {
    RES_R(0b00000100, _hl.h);
    return 8;
  }

  inline int OpCb0x95()
  {
    RES_R(0b00000100, _hl.l);
    return 8;
  }

  inline int OpCb0x96()
  {
    RES__HL(0b00000100);
    return 16;
  }

  inline int OpCb0x97()
  {
    RES_R(0b00000100, _af.h);
    return 8;
  }

  inline int OpCb0x98()
  {
    RES_R(0b00001000, _bc.h);
    return 8;
  }

  inline int OpCb0x99()
  {
    RES_R(0b00001000, _bc.l);
    return 8;
  }

  inline int OpCb0x9A()
  {
    RES_R(0b00001000, _de.h);
    return 8;
  }

  inline int OpCb0x9B()
  {
    RES_R(0b00001000, _de.l);
    return 8;
  }

  inline int OpCb0x9C()
  {
    RES_R(0b00001000, _hl.h);
    return 8;
  }

  inline int OpCb0x9D()
  {
    RES_R(0b00001000, _hl.l);
    return 8;
  }

  inline int OpCb0x9E()
  {
    RES__HL(0b00001000);
    return 16;
  }

  inline int OpCb0x9F()
  {
    RES_R(0b00001000, _af.h);
    return 8;
  }

  inline int OpCb0xA0()
  {
    RES_R(0b00010000, _bc.h);
    return 8;
  }

  inline int OpCb0xA1()
  {
    RES_R(0b00010000, _bc.l);
    return 8;
  }

  inline int OpCb0xA2()
  {
    RES_R(0b00010000, _de.h);
    return 8;
  }

  inline int OpCb0xA3()
  {
    RES_R(0b00010000, _de.l);
    return 8;
  }

  inline int OpCb0xA4()
  {
    RES_R(0b00010000, _hl.h);
    return 8;
  }

  inline int OpCb0xA5()
  {
    RES_R(0b00010000, _hl.l);
    return 8;
  }

  inline int OpCb0xA6()
  {
    RES__HL(0b00010000);
    return 16;
  }

  inline int OpCb0xA7()
  {
    RES_R(0b00010000, _af.h);
    return 8;
  }

  inline int OpCb0xA8()
  {
    RES_R(0b00100000, _bc.h);
    return 8;
  }

  inline int OpCb0xA9()
  {
    RES_R(0b00100000, _bc.l);
    return 8;
  }

  inline int OpCb0xAA()
  {
    RES_R(0b00100000, _de.h);
    return 8;
  }

  inline int OpCb0xAB()
  {
    RES_R(0b00100000, _de.l);
    return 8;
  }

  inline int OpCb0xAC()
  {
    RES_R(0b00100000, _hl.h);
    return 8;
  }

  inline int OpCb0xAD()
  {
    RES_R(0b00100000, _hl.l);
    return 8;
  }

  inline int OpCb0xAE()
  {
    RES__HL(0b00100000);
    return 16;
  }

  inline int OpCb0xAF()
  {
    RES_R(0b00100000, _af.h);
    return 8;
  }

  inline int OpCb0xB0()
  {
    RES_R(0b01000000, _bc.h);
    return 8;
  }

  inline int OpCb0xB1()
  {
    RES_R(0b01000000, _bc.l);
    return 8;
  }

  inline int OpCb0xB2()
  {
    RES_R(0b01000000, _de.h);
    return 8;
  }

  inline int OpCb0xB3()
  {
    RES_R(0b01000000, _de.l);
    return 8;
  }

  inline int OpCb0xB4()
  {
    RES_R(0b01000000, _hl.h);
    return 8;
  }

  inline int OpCb0xB5()
  {
    RES_R(0b01000000, _hl.l);
    return 8;
  }

  inline int OpCb0xB6()
  {
    RES__HL(0b01000000);
    return 16;
  }

  inline int OpCb0xB7()
  {
    RES_R(0b01000000, _af.h);
    return 8;
  }

  inline int OpCb0xB8()
  {
    RES_R(0b10000000, _bc.h);
    return 8;
  }

  inline int OpCb0xB9()
  {
    RES_R(0b10000000, _bc.l);
    return 8;
  }

  inline int OpCb0xBA()
  {
    RES_R(0b10000000, _de.h);
    return 8;
  }

  inline int OpCb0xBB()
  {
    RES_R(0b10000000, _de.l);
    return 8;
  }

  inline int OpCb0xBC()
  {
    RES_R(0b10000000, _hl.h);
    return 8;
  }

  inline int OpCb0xBD()
  {
    RES_R(0b10000000, _hl.l);
    return 8;
  }

  inline int OpCb0xBE()
  {
    RES__HL(0b10000000);
    return 16;
  }

  inline int OpCb0xBF()
  {
    RES_R(0b10000000, _af.h);
    return 8;
  }

  inline int OpCb0xC0()
  {
    SET_R(0b00000001, _bc.h);
    return 8;
  }

  inline int OpCb0xC1()
  {
    SET_R(0b00000001, _bc.l);
    return 8;
  }

  inline int OpCb0xC2()
  {
    SET_R(0b00000001, _de.h);
    return 8;
  }

  inline int OpCb0xC3()
  {
    SET_R(0b00000001, _de.l);
    return 8;
  }

  inline int OpCb0xC4()
  {
    SET_R(0b00000001, _hl.h);
    return 8;
  }

  inline int OpCb0xC5()
  {
    SET_R(0b00000001, _hl.l);
    return 8;
  }

  inline int OpCb0xC6()
  {
    SET__HL(0b00000001);
    return 16;
  }

  inline int OpCb0xC7()
  {
    SET_R(0b00000001, _af.h);
    return 8;
  }

  inline int OpCb0xC8()
  {
    SET_R(0b00000010, _bc.h);
    return 8;
  }

  inline int OpCb0xC9()
  {
    SET_R(0b00000010, _bc.l);
    return 8;
  }

  inline int OpCb0xCA()
  {
    SET_R(0b00000010, _de.h);
    return 8;
  }

  inline int OpCb0xCB()
  {
    SET_R(0b00000010, _de.l);
    return 8;
  }

  inline int OpCb0xCC()
  {
    SET_R(0b00000010, _hl.h);
    return 8;
  }

  inline int OpCb0xCD()
  {
    SET_R(0b00000010, _hl.l);
    return 8;
  }

  inline int OpCb0xCE()
  {
    SET__HL(0b00000010);
    return 16;
  }

  inline int OpCb0xCF()
  {
    SET_R(0b00000010, _af.h);
    return 8;
  }

  inline int OpCb0xD0()
  {
    SET_R(0b00000100, _bc.h);
    return 8;
  }

  inline int OpCb0xD1()
  {
    SET_R(0b00000100, _bc.l);
    return 8;
  }

  inline int OpCb0xD2()
  {
    SET_R(0b00000100, _de.h);
    return 8;
  }

  inline int OpCb0xD3()
  {
    SET_R(0b00000100, _de.l);
    return 8;
  }

  inline int OpCb0xD4()
  {
    SET_R(0b00000100, _hl.h);
    return 8;
  }

  inline int OpCb0xD5()
  {
    SET_R(0b00000100, _hl.l);
    return 8;
  }

  inline int OpCb0xD6()
  {
    SET__HL(0b00000100);
    return 16;
  }

  inline int OpCb0xD7()
  {
    SET_R(0b00000100, _af.h);
    return 8;
  }

  inline int OpCb0xD8()
  {
    SET_R(0b00001000, _bc.h);
    return 8;
  }

  inline int OpCb0xD9()
  {
    SET_R(0b00001000, _bc.l);
    return 8;
  }

  inline int OpCb0xDA()
  {
    SET_R(0b00001000, _de.h);
    return 8;
  }

  inline int OpCb0xDB()
  {
    SET_R(0b00001000, _de.l);
    return 8;
  }

  inline int OpCb0xDC()
  {
    SET_R(0b00001000, _hl.h);
    return 8;
  }

  inline int OpCb0xDD()
  {
    SET_R(0b00001000, _hl.l);
    return 8;
  }

  inline int OpCb0xDE()
  {
    SET__HL(0b00001000);
    return 16;
  }

  inline int OpCb0xDF()
  {
    SET_R(0b00001000, _af.h);
    return 8;
  }

  inline int OpCb0xE0()
  {
    SET_R(0b00010000, _bc.h);
    return 8;
  }

  inline int OpCb0xE1()
  {
    SET_R(0b00010000, _bc.l);
    return 8;
  }

  inline int OpCb0xE2()
  {
    SET_R(0b00010000, _de.h);
    return 8;
  }

  inline int OpCb0xE3()
  {
    SET_R(0b00010000, _de.l);
    return 8;
  }

  inline int OpCb0xE4()
  {
    SET_R(0b00010000, _hl.h);
    return 8;
  }

  inline int OpCb0xE5()
  {
    SET_R(0b00010000, _hl.l);
    return 8;
  }

  inline int OpCb0xE6()
  {
    SET__HL(0b00010000);
    return 16;
  }

  inline int OpCb0xE7()
  {
    SET_R(0b00010000, _af.h);
    return 8;
  }

  inline int OpCb0xE8()
  {
    SET_R(0b00100000, _bc.h);
    return 8;
  }

  inline int OpCb0xE9()
  {
    SET_R(0b00100000, _bc.l);
    return 8;
  }

  inline int OpCb0xEA()
  {
    SET_R(0b00100000, _de.h);
    return 8;
  }

  inline int OpCb0xEB()
  {
    SET_R(0b00100000, _de.l);
    return 8;
  }
  inline int OpCb0xEC()
  {
    SET_R(0b00100000, _hl.h);
    return 8;
  }

  inline int OpCb0xED()
  {
    SET_R(0b00100000, _hl.l);
    return 8;
  }

  inline int OpCb0xEE()
  {
    SET__HL(0b00100000);
    return 16;
  }

  inline int OpCb0xEF()
  {
    SET_R(0b00100000, _af.h);
    return 8;
  }

  inline int OpCb0xF0()
  {
    SET_R(0b01000000, _bc.h);
    return 8;
  }

  inline int OpCb0xF1()
  {
    SET_R(0b01000000, _bc.l);
    return 8;
  }

  inline int OpCb0xF2()
  {
    SET_R(0b01000000, _de.h);
    return 8;
  }

  inline int OpCb0xF3()
  {
    SET_R(0b01000000, _de.l);
    return 8;
  }

  inline int OpCb0xF4()
  {
    SET_R(0b01000000, _hl.h);
    return 8;
  }

  inline int OpCb0xF5()
  {
    SET_R(0b01000000, _hl.l);
    return 8;
  }

  inline int OpCb0xF6()
  {
    SET__HL(0b01000000);
    return 16;
  }

  inline int OpCb0xF7()
  {
    SET_R(0b01000000, _af.h);
    return 8;
  }

  inline int OpCb0xF8()
  {
    SET_R(0b10000000, _bc.h);
    return 8;
  }

  inline int OpCb0xF9()
  {
    SET_R(0b10000000, _bc.l);
    return 8;
  }

  inline int OpCb0xFA()
  {
    SET_R(0b10000000, _de.h);
    return 8;
  }

  inline int OpCb0xFB()
  {
    SET_R(0b10000000, _de.l);
    return 8;
  }

  inline int OpCb0xFC()
  {
    SET_R(0b10000000, _hl.h);
    return 8;
  }

  inline int OpCb0xFD()
  {
    SET_R(0b10000000, _hl.l);
    return 8;
  }

  inline int OpCb0xFE()
  {
    SET__HL(0b10000000);
    return 16;
  }

  inline int OpCb0xFF()
  {
    SET_R(0b10000000, _af.h);
    return 8;
  }

#pragma endregion

#pragma region GENERALIZED EXTENDED INSTRUCTIONS
  //Flags: Z 0 0 Bit7
  inline void RLC__HL()
  {
    uint8_t val = ReadAddress(_hl.hl);
    (val & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val << 1 | val >> 7;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(_hl.hl, result);
  }

  //Flags: Z 0 0 Bit7
  inline void RLC_R(uint8_t& R)
  {
    (R & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = R << 1 | R >> 7;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    R = result;
  }

  //Flags: Z 0 0 Bit0
  inline void RRC__HL()
  {
    uint8_t val = ReadAddress(_hl.hl);
    (val & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val >> 1 | val << 7;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(_hl.hl, result);
  }

  //Flags: Z 0 0 Bit0
  inline void RRC_R(uint8_t& R)
  {
    (R & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = R >> 1 | R << 7;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    R = result;
  }

  //Flags: Z 0 0 Bit7
  inline void RL__HL()
  {
    uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
    uint8_t val = ReadAddress(_hl.hl);
    (val & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val << 1 | carry_bit;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(_hl.hl, result);
  }

  //Flags: Z 0 0 Bit7
  inline void RL_R(uint8_t& R)
  {
    uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
    (R & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = R << 1 | carry_bit;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    R = result;
  }

  //Flags: Z 0 0 Bit0
  inline void RR__HL()
  {
    uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
    uint8_t val = ReadAddress(_hl.hl);
    (val & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val >> 1 | (carry_bit << 7);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(_hl.hl, result);
  }

  //Flags: Z 0 0 Bit0
  inline void RR_R(uint8_t& R)
  {
    uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
    (R & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = R >> 1 | (carry_bit << 7);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    R = result;
  }

  //Flags: Z 0 0 Bit7
  inline void SLA__HL()
  {
    uint8_t val = ReadAddress(_hl.hl);
    (val & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val << 1;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(_hl.hl, result);
  }

  //Flags: Z 0 0 Bit7
  inline void SLA_R(uint8_t& R)
  {
    (R & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = R << 1;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    R = result;
  }

  //Flags: Z 0 0 Bit0
  inline void SRA__HL()
  {
    uint8_t val = ReadAddress(_hl.hl);
    (val & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val >> 1 | (val & 0b10000000);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(_hl.hl, result);
  }

  //Flags: Z 0 0 Bit0
  inline void SRA_R(uint8_t& R)
  {
    (R & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = R >> 1 | (R & 0b10000000);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    R = result;
  }

  // Flags: Z 0 0 0
  inline void SWAP__HL()
  {
    uint8_t val = ReadAddress(_hl.hl);
    uint8_t result = (val >> 4) | (val << 4);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
    WriteAddress(_hl.hl, result);
  }

  // Flags: Z 0 0 0
  inline void SWAP_R(uint8_t& R)
  {
    uint8_t result = (R >> 4) | (R << 4);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
    R = result;
  }

  // Flags: Z 0 0 bit0
  inline void SRL__HL()
  {
    uint8_t val = ReadAddress(_hl.hl);
    (val & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val >> 1;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(_hl.hl, result);
  }

  // Flags: Z 0 0 bit0
  inline void SRL_R(uint8_t& R)
  {
    (R & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = R >> 1;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    R = result;
  }

  // Flags: !bit 0 1 -
  inline void BIT__HL(uint8_t bit_mask)
  {
    uint8_t val = ReadAddress(_hl.hl);
    (val & bit_mask) ? ResetFlag(FlagBitMask::Zero) : SetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    SetFlag(FlagBitMask::HalfCarry);
  }

  // Flags: !bit 0 1 -
  inline void BIT_R(uint8_t bit_mask, uint8_t R)
  {
    (R & bit_mask) ? ResetFlag(FlagBitMask::Zero) : SetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    SetFlag(FlagBitMask::HalfCarry);
  }

  // Flags: - - - -
  inline void RES__HL(uint8_t bit_mask)
  {
    uint8_t val = ReadAddress(_hl.hl);
    uint8_t result = val & ~bit_mask;
    WriteAddress(_hl.hl, result);
  }

  // Flags: - - - -
  inline void RES_R(uint8_t bit_mask, uint8_t& R)
  {
    R &= ~bit_mask;
  }

  // Flags: - - - -
  inline void SET__HL(uint8_t bit_mask)
  {
    uint8_t val = ReadAddress(_hl.hl);
    uint8_t result = val | bit_mask;
    WriteAddress(_hl.hl, result);
  }

  // Flags: - - - -
  inline void SET_R(uint8_t bit_mask, uint8_t& R)
  {
    R |= bit_mask;
  }
#pragma endregion
};
