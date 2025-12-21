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

#include <iostream>
#include <iomanip>
#include <filesystem>
#include <fstream>

struct Palettes
{
  std::vector<Pixel> bg;
  std::vector<Pixel> obj0;
  std::vector<Pixel> obj1;
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

  Status SetRom(const std::filesystem::path& rom_path, std::vector<uint8_t> rom);
  void Main();

  void RunUntil(uint64_t cycle_count);
  void Run();

  inline void Stop()
  {
    m_stopped = true;
  }

  StopWatch m_execution_stopwatch;

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

  Register m_af{};
  Register m_bc{};
  Register m_de{};
  Register m_hl{};

  uint16_t m_sp{};
  uint16_t m_pc{};

  void OutputRegisters()
  {
    std::cout << std::dec << m_total_cycle_count;
    std::cout << std::hex << std::uppercase << std::setfill('0');
    std::cout << " AF: " << std::setw(4) << m_af.hl << " BC: " << std::setw(4) << m_bc.hl << " DE: " << std::setw(4) << m_de.hl << " HL: " << std::setw(4) << m_hl.hl << " SP: " << std::setw(4) << m_sp << " PC: " << std::setw(4) << m_pc;
    std::cout << " IME: " << std::dec << (int)m_interrupt_controller.m_ime << " IE: " << (int)m_interrupt_controller.m_ie << " IF: " << (int)m_interrupt_controller.m_if;
    std::cout << std::endl;
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

  inline void SetHalfCarryFlagBit3To4(uint8_t val1, int val2)
  {
    bool bit_4_set = val2 < 0 ? ((val1 & 0xF) - (-val2 & 0xF)) & 0x10
      : ((val1 & 0xF) + (+val2 & 0xF)) & 0x10;

    bit_4_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
  }

  inline void SetHalfCarryFlagBit3To4Fc(uint8_t val1, int val2, int carry)
  {
    bool bit_4_set = val2 < 0 || carry < 0 ? ((val1 & 0xF) - (-val2 & 0xF) - (-carry & 0xF)) & 0x10
      : ((val1 & 0xF) + (+val2 & 0xF) + (+carry & 0xF)) & 0x10;

    bit_4_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
  }

  inline void SetHalfCarryFlagBit11To12(uint16_t val1, int val2)
  {
    bool bit_12_set = val2 < 0 ? ((val1 & 0xFFF) - (-val2 & 0xFFF)) & 0x1000
      : ((val1 & 0xFFF) + (+val2 & 0xFFF)) & 0x1000;

    bit_12_set ? SetFlag(FlagBitMask::HalfCarry) : ResetFlag(FlagBitMask::HalfCarry);
  }

  inline void SetCarry16Bit(int val)
  {
    (val > 0xFFFF || val < 0) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
  }

  inline void SetCarry8Bit(int val)
  {
    (val > 0xFF || val < 0) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
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
    m_af.l |= static_cast<uint8_t>(flag);
  }

  inline void ResetFlag(FlagBitMask flag)
  {
    m_af.l &= ~static_cast<uint8_t>(flag);
  }

  inline bool ReadFlag(FlagBitMask flag) const
  {
    return m_af.l & static_cast<uint8_t>(flag);
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

  uint8_t m_opcode = 0x00;

  bool m_boot_complete = false;
  bool m_interrupt_enabled_requested = false;
  bool m_stop_requested = false;
  bool m_halt_requested = false;

  virtual uint8_t ReadAddress(uint16_t address);
  virtual void WriteAddress(uint16_t address, uint8_t val);

  uint8_t ReadIo(uint16_t address);
  void WriteIo(uint16_t address, uint8_t val);

  inline uint16_t ReadNextUint16()
  {
    uint8_t l = ReadAddress(m_pc);
    m_pc += 1;
    uint8_t h = ReadAddress(m_pc);
    m_pc += 1;

    return (h << 8) | l;
  }

  inline uint8_t ReadNextUint8()
  {
    uint8_t val = ReadAddress(m_pc);
    m_pc += 1;

    return val;
  }

  inline int8_t ReadNextInt8()
  {
    uint8_t val = ReadAddress(m_pc);
    m_pc += 1;

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
    m_pc -= 1;
    PUSH_RR(m_pc);
    m_pc = isr;
    m_opcode = Fetch();

    return 20;
  }

  inline int ExecutionHandler()
  {
    int cycle_count = Execute(m_opcode);
    m_opcode = Fetch();

    return cycle_count;
  }

  inline uint8_t Fetch()
  {
    return ReadNextUint8();
  }

  double m_frame_time = 0;

  virtual void WaitFrame();
  void Init();

  int Execute(uint8_t opcode);
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

  inline int Op0x00()
  {
    // Crickets
    return 4;
  }

  inline int Op0x01()
  {
    LD_RR_dd(m_bc.hl);
    return 12;
  }

  inline int Op0x02()
  {
    LD__RR_A(m_bc.hl);
    return 8;
  }

  inline int Op0x03()
  {
    INC_RR(m_bc.hl);
    return 8;
  }

  inline int Op0x04()
  {
    INC_R(m_bc.h);
    return 4;
  }

  inline int Op0x05()
  {
    DEC_R(m_bc.h);
    return 4;
  }

  inline int Op0x06()
  {
    LD_R_d(m_bc.h);
    return 8;
  }

  inline int Op0x07()
  {
    RLCA();
    return 4;
  }

  inline int Op0x08()
  {
    LD__dd_RR(m_sp);
    return 20;
  }

  inline int Op0x09()
  {
    ADD_HL_RR(m_bc.hl);
    return 8;
  }

  inline int Op0x0A()
  {
    LD_A__RR(m_bc.hl);
    return 8;
  }

  inline int Op0x0B()
  {
    DEC_RR(m_bc.hl);
    return 8;
  }

  inline int Op0x0C()
  {
    INC_R(m_bc.l);
    return 4;
  }

  inline int Op0x0D()
  {
    DEC_R(m_bc.l);
    return 4;
  }

  inline int Op0x0E()
  {
    LD_R_d(m_bc.l);
    return 8;
  }

  inline int Op0x0F()
  {
    RRCA();
    return 4;
  }

  inline int Op0x10()
  {
    m_stop_requested = true;
    return 4;
  }

  inline int Op0x11()
  {
    LD_RR_dd(m_de.hl);
    return 12;
  }

  inline int Op0x12()
  {
    LD__RR_A(m_de.hl);
    return 8;
  }

  inline int Op0x13()
  {
    INC_RR(m_de.hl);
    return 8;
  }

  inline int Op0x14()
  {
    INC_R(m_de.h);
    return 4;
  }

  inline int Op0x15()
  {
    DEC_R(m_de.h);
    return 4;
  }

  inline int Op0x16()
  {
    LD_R_d(m_de.h);
    return 8;
  }

  inline int Op0x17()
  {
    RLA();
    return 4;
  }

  inline int Op0x18()
  {
    JR(true);
    return 12;
  }

  inline int Op0x19()
  {
    ADD_HL_RR(m_de.hl);
    return 8;
  }

  inline int Op0x1A()
  {
    LD_A__RR(m_de.hl);
    return 8;
  }

  inline int Op0x1B()
  {
    DEC_RR(m_de.hl);
    return 8;
  }

  inline int Op0x1C()
  {
    INC_R(m_de.l);
    return 4;
  }

  inline int Op0x1D()
  {
    DEC_R(m_de.l);
    return 4;
  }

  inline int Op0x1E()
  {
    LD_R_d(m_de.l);
    return 8;
  }

  inline int Op0x1F()
  {
    RRA();
    return 4;
  }

  inline int Op0x20()
  {
    JR(!ReadFlag(FlagBitMask::Zero));
    return !ReadFlag(FlagBitMask::Zero) ? 12 : 8;
  }

  inline int Op0x21()
  {
    LD_RR_dd(m_hl.hl);
    return 12;
  }

  inline int Op0x22()
  {
    LD__RR_A(m_hl.hl++);
    return 8;
  }

  inline int Op0x23()
  {
    INC_RR(m_hl.hl);
    return 8;
  }

  inline int Op0x24()
  {
    INC_R(m_hl.h);
    return 4;
  }

  inline int Op0x25()
  {
    DEC_R(m_hl.h);
    return 4;
  }

  inline int Op0x26()
  {
    LD_R_d(m_hl.h);
    return 8;
  }

  inline int Op0x27()
  {
    DAA();
    return 4;
  }

  inline int Op0x28()
  {
    JR(ReadFlag(FlagBitMask::Zero));
    return ReadFlag(FlagBitMask::Zero) ? 12 : 8;
  }

  inline int Op0x29()
  {
    ADD_HL_RR(m_hl.hl);
    return 8;
  }

  inline int Op0x2A()
  {
    LD_A__RR(m_hl.hl++);
    return 8;
  }

  inline int Op0x2B()
  {
    DEC_RR(m_hl.hl);
    return 8;
  }

  inline int Op0x2C()
  {
    INC_R(m_hl.l);
    return 4;
  }

  inline int Op0x2D()
  {
    DEC_R(m_hl.l);
    return 4;
  }

  inline int Op0x2E()
  {
    LD_R_d(m_hl.l);
    return 8;
  }

  inline int Op0x2F()
  {
    CPL();
    return 4;
  }

  inline int Op0x30()
  {
    JR(!ReadFlag(FlagBitMask::Carry));
    return !ReadFlag(FlagBitMask::Carry) ? 12 : 8;
  }

  inline int Op0x31()
  {
    LD_RR_dd(m_sp);
    return 12;
  }

  inline int Op0x32()
  {
    LD__RR_A(m_hl.hl--);
    return 8;
  }

  inline int Op0x33()
  {
    INC_RR(m_sp);
    return 8;
  }

  inline int Op0x34()
  {
    INC__RR(m_hl.hl);
    return 12;
  }

  inline int Op0x35()
  {
    DEC__RR(m_hl.hl);
    return 12;
  }

  inline int Op0x36()
  {
    WriteAddress(m_hl.hl, ReadNextUint8());
    return 12;
  }

  inline int Op0x37()
  {
    SCF();
    return 4;
  }

  inline int Op0x38()
  {
    JR(ReadFlag(FlagBitMask::Carry));
    return ReadFlag(FlagBitMask::Carry) ? 12 : 8;
  }

  inline int Op0x39()
  {
    ADD_HL_RR(m_sp);
    return 8;
  }

  inline int Op0x3A()
  {
    LD_A__RR(m_hl.hl--);
    return 8;
  }

  inline int Op0x3B()
  {
    DEC_RR(m_sp);
    return 8;
  }

  inline int Op0x3C()
  {
    INC_R(m_af.h);
    return 4;
  }

  inline int Op0x3D()
  {
    DEC_R(m_af.h);
    return 4;
  }

  inline int Op0x3E()
  {
    LD_R_d(m_af.h);
    return 8;
  }

  inline int Op0x3F()
  {
    CCF();
    return 4;
  }

  inline int Op0x40()
  {
    LD_R_X(m_bc.h, m_bc.h);
    return 4;
  }

  inline int Op0x41()
  {
    LD_R_X(m_bc.h, m_bc.l);
    return 4;
  }

  inline int Op0x42()
  {
    LD_R_X(m_bc.h, m_de.h);
    return 4;
  }

  inline int Op0x43()
  {
    LD_R_X(m_bc.h, m_de.l);
    return 4;
  }

  inline int Op0x44()
  {
    LD_R_X(m_bc.h, m_hl.h);
    return 4;
  }

  inline int Op0x45()
  {
    LD_R_X(m_bc.h, m_hl.l);
    return 4;
  }

  inline int Op0x46()
  {
    LD_R__HL(m_bc.h);
    return 8;
  }

  inline int Op0x47()
  {
    LD_R_X(m_bc.h, m_af.h);
    return 4;
  }

  inline int Op0x48()
  {
    LD_R_X(m_bc.l, m_bc.h);
    return 4;
  }

  inline int Op0x49()
  {
    LD_R_X(m_bc.l, m_bc.l);
    return 4;
  }

  inline int Op0x4A()
  {
    LD_R_X(m_bc.l, m_de.h);
    return 4;
  }

  inline int Op0x4B()
  {
    LD_R_X(m_bc.l, m_de.l);
    return 4;
  }

  inline int Op0x4C()
  {
    LD_R_X(m_bc.l, m_hl.h);
    return 4;
  }

  inline int Op0x4D()
  {
    LD_R_X(m_bc.l, m_hl.l);
    return 4;
  }

  inline int Op0x4E()
  {
    LD_R__HL(m_bc.l);
    return 8;
  }

  inline int Op0x4F()
  {
    LD_R_X(m_bc.l, m_af.h);
    return 4;
  }

  inline int Op0x50()
  {
    LD_R_X(m_de.h, m_bc.h);
    return 4;
  }

  inline int Op0x51()
  {
    LD_R_X(m_de.h, m_bc.l);
    return 4;
  }

  inline int Op0x52()
  {
    LD_R_X(m_de.h, m_de.h);
    return 4;
  }

  inline int Op0x53()
  {
    LD_R_X(m_de.h, m_de.l);
    return 4;
  }

  inline int Op0x54()
  {
    LD_R_X(m_de.h, m_hl.h);
    return 4;
  }

  inline int Op0x55()
  {
    LD_R_X(m_de.h, m_hl.l);
    return 4;
  }

  inline int Op0x56()
  {
    LD_R__HL(m_de.h);
    return 8;
  }

  inline int Op0x57()
  {
    LD_R_X(m_de.h, m_af.h);
    return 4;
  }

  inline int Op0x58()
  {
    LD_R_X(m_de.l, m_bc.h);
    return 4;
  }

  inline int Op0x59()
  {
    LD_R_X(m_de.l, m_bc.l);
    return 4;
  }

  inline int Op0x5A()
  {
    LD_R_X(m_de.l, m_de.h);
    return 4;
  }

  inline int Op0x5B()
  {
    LD_R_X(m_de.l, m_de.l);
    return 4;
  }

  inline int Op0x5C()
  {
    LD_R_X(m_de.l, m_hl.h);
    return 4;
  }
  inline int Op0x5D()
  {
    LD_R_X(m_de.l, m_hl.l);
    return 4;
  }
  inline int Op0x5E()
  {
    LD_R__HL(m_de.l);
    return 8;
  }
  inline int Op0x5F()
  {
    LD_R_X(m_de.l, m_af.h);
    return 4;
  }

  inline int Op0x60()
  {
    LD_R_X(m_hl.h, m_bc.h);
    return 4;
  }

  inline int Op0x61()
  {
    LD_R_X(m_hl.h, m_bc.l);
    return 4;
  }

  inline int Op0x62()
  {
    LD_R_X(m_hl.h, m_de.h);
    return 4;
  }

  inline int Op0x63()
  {
    LD_R_X(m_hl.h, m_de.l);
    return 4;
  }

  inline int Op0x64()
  {
    LD_R_X(m_hl.h, m_hl.h);
    return 4;
  }

  inline int Op0x65()
  {
    LD_R_X(m_hl.h, m_hl.l);
    return 4;
  }

  inline int Op0x66()
  {
    LD_R__HL(m_hl.h);
    return 8;
  }

  inline int Op0x67()
  {
    LD_R_X(m_hl.h, m_af.h);
    return 4;
  }

  inline int Op0x68()
  {
    LD_R_X(m_hl.l, m_bc.h);
    return 4;
  }

  inline int Op0x69()
  {
    LD_R_X(m_hl.l, m_bc.l);
    return 4;
  }

  inline int Op0x6A()
  {
    LD_R_X(m_hl.l, m_de.h);
    return 4;
  }

  inline int Op0x6B()
  {
    LD_R_X(m_hl.l, m_de.l);
    return 4;
  }

  inline int Op0x6C()
  {
    LD_R_X(m_hl.l, m_hl.h);
    return 4;
  }

  inline int Op0x6D()
  {
    LD_R_X(m_hl.l, m_hl.l);
    return 4;
  }

  inline int Op0x6E()
  {
    LD_R__HL(m_hl.l);
    return 8;
  }

  inline int Op0x6F()
  {
    LD_R_X(m_hl.l, m_af.h);
    return 4;
  }

  inline int Op0x70()
  {
    LD__RR_R(m_hl.hl, m_bc.h);
    return 8;
  }

  inline int Op0x71()
  {
    LD__RR_R(m_hl.hl, m_bc.l);
    return 8;
  }

  inline int Op0x72()
  {
    LD__RR_R(m_hl.hl, m_de.h);
    return 8;
  }

  inline int Op0x73()
  {
    LD__RR_R(m_hl.hl, m_de.l);
    return 8;
  }

  inline int Op0x74()
  {
    LD__RR_R(m_hl.hl, m_hl.h);
    return 8;
  }

  inline int Op0x75()
  {
    LD__RR_R(m_hl.hl, m_hl.l);
    return 8;
  }

  inline int Op0x76()
  {
    m_halt_requested = true;
    return 4;
  }

  inline int Op0x77()
  {
    LD__RR_R(m_hl.hl, m_af.h);
    return 8;
  }

  inline int Op0x78()
  {
    LD_R_X(m_af.h, m_bc.h);
    return 4;
  }

  inline int Op0x79()
  {
    LD_R_X(m_af.h, m_bc.l);
    return 4;
  }

  inline int Op0x7A()
  {
    LD_R_X(m_af.h, m_de.h);
    return 4;
  }

  inline int Op0x7B()
  {
    LD_R_X(m_af.h, m_de.l);
    return 4;
  }

  inline int Op0x7C()
  {
    LD_R_X(m_af.h, m_hl.h);
    return 4;
  }

  inline int Op0x7D()
  {
    LD_R_X(m_af.h, m_hl.l);
    return 4;
  }

  inline int Op0x7E()
  {
    LD_R__HL(m_af.h);
    return 8;
  }

  inline int Op0x7F()
  {
    LD_R_X(m_af.h, m_af.h);
    return 4;
  }

  inline int Op0x80()
  {
    ADD_A_X(m_bc.h);
    return 4;
  }

  inline int Op0x81()
  {
    ADD_A_X(m_bc.l);
    return 4;
  }

  inline int Op0x82()
  {
    ADD_A_X(m_de.h);
    return 4;
  }

  inline int Op0x83()
  {
    ADD_A_X(m_de.l);
    return 4;
  }

  inline int Op0x84()
  {
    ADD_A_X(m_hl.h);
    return 4;
  }

  inline int Op0x85()
  {
    ADD_A_X(m_hl.l);
    return 4;
  }

  inline int Op0x86()
  {
    ADD_A_X(ReadAddress(m_hl.hl));
    return 8;
  }

  inline int Op0x87()
  {
    ADD_A_X(m_af.h);
    return 4;
  }

  inline int Op0x88()
  {
    ADC_A_X(m_bc.h);
    return 4;
  }

  inline int Op0x89()
  {
    ADC_A_X(m_bc.l);
    return 4;
  }

  inline int Op0x8A()
  {
    ADC_A_X(m_de.h);
    return 4;
  }

  inline int Op0x8B()
  {
    ADC_A_X(m_de.l);
    return 4;
  }

  inline int Op0x8C()
  {
    ADC_A_X(m_hl.h);
    return 4;
  }

  inline int Op0x8D()
  {
    ADC_A_X(m_hl.l);
    return 4;
  }

  inline int Op0x8E()
  {
    ADC_A_X(ReadAddress(m_hl.hl));
    return 8;
  }

  inline int Op0x8F()
  {
    ADC_A_X(m_af.h);
    return 4;
  }

  inline int Op0x90()
  {
    SUB_A_X(m_bc.h);
    return 4;
  }

  inline int Op0x91()
  {
    SUB_A_X(m_bc.l);
    return 4;
  }

  inline int Op0x92()
  {
    SUB_A_X(m_de.h);
    return 4;
  }

  inline int Op0x93()
  {
    SUB_A_X(m_de.l);
    return 4;
  }

  inline int Op0x94()
  {
    SUB_A_X(m_hl.h);
    return 4;
  }

  inline int Op0x95()
  {
    SUB_A_X(m_hl.l);
    return 4;
  }

  inline int Op0x96()
  {
    SUB_A_X(ReadAddress(m_hl.hl));
    return 8;
  }

  inline int Op0x97()
  {
    SUB_A_X(m_af.h);
    return 4;
  }

  inline int Op0x98()
  {
    SBC_A_X(m_bc.h);
    return 4;
  }

  inline int Op0x99()
  {
    SBC_A_X(m_bc.l);
    return 4;
  }

  inline int Op0x9A()
  {
    SBC_A_X(m_de.h);
    return 4;
  }

  inline int Op0x9B()
  {
    SBC_A_X(m_de.l);
    return 4;
  }

  inline int Op0x9C()
  {
    SBC_A_X(m_hl.h);
    return 4;
  }

  inline int Op0x9D()
  {
    SBC_A_X(m_hl.l);
    return 4;
  }

  inline int Op0x9E()
  {
    SBC_A_X(ReadAddress(m_hl.hl));
    return 8;
  }

  inline int Op0x9F()
  {
    SBC_A_X(m_af.h);
    return 4;
  }

  inline int Op0xA0()
  {
    AND_A_X(m_bc.h);
    return 4;
  }

  inline int Op0xA1()
  {
    AND_A_X(m_bc.l);
    return 4;
  }

  inline int Op0xA2()
  {
    AND_A_X(m_de.h);
    return 4;
  }

  inline int Op0xA3()
  {
    AND_A_X(m_de.l);
    return 4;
  }

  inline int Op0xA4()
  {
    AND_A_X(m_hl.h);
    return 4;
  }

  inline int Op0xA5()
  {
    AND_A_X(m_hl.l);
    return 4;
  }

  inline int Op0xA6()
  {
    AND_A_X(ReadAddress(m_hl.hl));
    return 8;
  }

  inline int Op0xA7()
  {
    AND_A_X(m_af.h);
    return 4;
  }

  inline int Op0xA8()
  {
    XOR_A_X(m_bc.h);
    return 4;
  }

  inline int Op0xA9()
  {
    XOR_A_X(m_bc.l);
    return 4;
  }

  inline int Op0xAA()
  {
    XOR_A_X(m_de.h);
    return 4;
  }

  inline int Op0xAB()
  {
    XOR_A_X(m_de.l);
    return 4;
  }

  inline int Op0xAC()
  {
    XOR_A_X(m_hl.h);
    return 4;
  }

  inline int Op0xAD()
  {
    XOR_A_X(m_hl.l);
    return 4;
  }

  inline int Op0xAE()
  {
    XOR_A_X(ReadAddress(m_hl.hl));
    return 8;
  }

  inline int Op0xAF()
  {
    XOR_A_X(m_af.h);
    return 4;
  }

  inline int Op0xB0()
  {
    OR_A_X(m_bc.h);
    return 4;
  }

  inline int Op0xB1()
  {
    OR_A_X(m_bc.l);
    return 4;
  }

  inline int Op0xB2()
  {
    OR_A_X(m_de.h);
    return 4;
  }

  inline int Op0xB3()
  {
    OR_A_X(m_de.l);
    return 4;
  }

  inline int Op0xB4()
  {
    OR_A_X(m_hl.h);
    return 4;
  }

  inline int Op0xB5()
  {
    OR_A_X(m_hl.l);
    return 4;
  }

  inline int Op0xB6()
  {
    OR_A_X(ReadAddress(m_hl.hl));
    return 8;
  }

  inline int Op0xB7()
  {
    OR_A_X(m_af.h);
    return 4;
  }

  inline int Op0xB8()
  {
    CP_A_X(m_bc.h);
    return 4;
  }

  inline int Op0xB9()
  {
    CP_A_X(m_bc.l);
    return 4;
  }

  inline int Op0xBA()
  {
    CP_A_X(m_de.h);
    return 4;
  }

  inline int Op0xBB()
  {
    CP_A_X(m_de.l);
    return 4;
  }

  inline int Op0xBC()
  {
    CP_A_X(m_hl.h);
    return 4;
  }

  inline int Op0xBD()
  {
    CP_A_X(m_hl.l);
    return 4;
  }

  inline int Op0xBE()
  {
    CP_A_X(ReadAddress(m_hl.hl));
    return 8;
  }

  inline int Op0xBF()
  {
    CP_A_X(m_af.h);
    return 4;
  }

  inline int Op0xC0()
  {
    RET(!ReadFlag(FlagBitMask::Zero));
    return !ReadFlag(FlagBitMask::Zero) ? 20 : 8;
  }

  inline int Op0xC1()
  {
    POP_RR(m_bc.hl);
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
    PUSH_RR(m_bc.hl);
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
    POP_RR(m_de.hl);
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
    PUSH_RR(m_de.hl);
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
    POP_RR(m_hl.hl);
    return 12;
  }

  inline int Op0xE2()
  {
    LD__R_A_IO(m_bc.l);
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
    PUSH_RR(m_hl.hl);
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
    m_pc = m_hl.hl;
    return 4;
  }

  inline int Op0xEA()
  {
    LD__RR_A(ReadNextUint16());
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
    LD_A__R_IO(m_bc.l);
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
    PUSH_RR(m_af.hl);
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
    m_sp = m_hl.hl;
    return 8;
  }

  inline int Op0xFA()
  {
    LD_R_X(m_af.h, ReadAddress(ReadNextUint16()));
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
  //   R denotes an 8-bit register
  //  RR denotes a 16-bit register
  //   d denotes the next 8-bit value
  //  dd denotes the next 16-bit value
  //   s denotes the next signed 8-bit value
  //  ss denotes the next signed 16-bit value
  // 
  //   X denotes wildcard
  //
  //  Some functions are specific to a register.
  //  These functions will denote the register in the name.
  //
  //  e.g. LD__RR_A -> Load the value of A into the address pointed to by RR
  //

// Flags: - - - -
  inline void LD_R_X(uint8_t& R, uint8_t val)
  {
    R = val;
  }

  // Flags: - - - -
  inline void LD_R__HL(uint8_t& R)
  {
    R = ReadAddress(m_hl.hl);
  }

  // Flags: - - - -
  inline void LD_R_d(uint8_t& R)
  {
    R = ReadNextUint8();
  }

  // Flags: - - - -
  inline void LD_A__RR(uint16_t RR)
  {
    m_af.h = ReadAddress(RR);
  }

  // Flags: - - - -
  inline void LD__dd_RR(uint16_t RR)
  {
    uint16_t address = ReadNextUint16();
    WriteAddress(address, RR & 0xFF);
    WriteAddress(address + 1, RR >> 8);
  }

  // Flags: - - - -
  inline void LD_RR_dd(uint16_t& RR)
  {
    RR = ReadNextUint16();
  }

  // Flags: - - - -
  inline void LD__RR_R(uint16_t RR, uint8_t R)
  {
    WriteAddress(RR, R);
  }

  // Flags: - - - -
  inline void LD__RR_A(uint16_t RR)
  {
    LD__RR_R(RR, m_af.h);
  }

  // Flags: - - - -
  inline void LD__R_A_IO(uint8_t R)
  {
    WriteAddress(static_cast<uint16_t>(ExecutionAddress::IO) + R, m_af.h);
  }

  // Flags: - - - -
  inline void LD_A__R_IO(uint8_t R)
  {
    m_af.h = ReadAddress(static_cast<uint16_t>(ExecutionAddress::IO) + R);
  }

  // Flags: 0 0 H C
  inline void LD_HL_SP_s()
  {
    int8_t val = ReadNextInt8();
    int result = m_sp + val;
    ResetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    SetHalfCarryFlagBit3To4(static_cast<uint8_t>(m_sp), static_cast<uint8_t>(val));
    SetCarry8Bit(static_cast<uint8_t>(m_sp) + static_cast<uint8_t>(val));
    m_hl.hl = static_cast<uint16_t>(result);
  }

  // Flags: 0 0 H C
  inline void ADD_SP_s()
  {
    int8_t val = ReadNextInt8();
    int result = m_sp + val;
    ResetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    SetHalfCarryFlagBit3To4(static_cast<uint8_t>(m_sp), static_cast<uint8_t>(val));
    SetCarry8Bit(static_cast<uint8_t>(m_sp) + static_cast<uint8_t>(val));
    m_sp = static_cast<uint16_t>(result);
  }

  // Flags: - - - -
  inline void ADD_RR(uint16_t& RR, int val)
  {
    RR += val;
  }

  // Flags: - - - -
  inline void INC_RR(uint16_t& RR)
  {
    ADD_RR(RR, 1);
  }

  // Flags: - - - -
  inline void DEC_RR(uint16_t& RR)
  {
    ADD_RR(RR, -1);
  }

  // Flags: Z 0 H C
  inline void ADC_A_X(int val)
  {
    int carry = ReadFlag(FlagBitMask::Carry) ? 1 : 0;
    int result = m_af.h + val + carry;
    SetHalfCarryFlagBit3To4Fc(m_af.h, val, carry);
    m_af.h = result;
    SetZeroFlag(m_af.h);
    ResetFlag(FlagBitMask::Subtract);
    SetCarry8Bit(result);
  }

  // Flags: Z 0 H C
  inline void ADD_A_X(int val)
  {
    ADD_X(m_af.h, val, true);
    ResetFlag(FlagBitMask::Subtract);
  }

  // Flags: Z 1 H C
  inline void SBC_A_X(int val)
  {
    int carry = ReadFlag(FlagBitMask::Carry) ? 1 : 0;
    int result = m_af.h - val - carry;
    SetHalfCarryFlagBit3To4Fc(m_af.h, -val, -carry);
    m_af.h = result;
    SetZeroFlag(m_af.h);
    SetFlag(FlagBitMask::Subtract);
    SetCarry8Bit(result);
  }

  // Flags: Z 1 H C
  inline void SUB_A_X(int val)
  {
    ADD_X(m_af.h, -val, true);
    SetFlag(FlagBitMask::Subtract);
  }

  // Flags: Z - H C
  inline void ADD_X(uint8_t& R, int val, bool set_carry = false)
  {
    int result = R + val;
    SetHalfCarryFlagBit3To4(R, val);
    R = result;
    SetZeroFlag(R);
    if (set_carry) SetCarry8Bit(result);
  }

  // Flags: Z N H -
  inline void ADD__RR(uint16_t RR, int val)
  {
    uint8_t curr_val = ReadAddress(RR);
    SetHalfCarryFlagBit3To4(curr_val, val);
    uint8_t result = curr_val + val;
    SetZeroFlag(result);
    SetSubtractionFlag(val);
    WriteAddress(RR, result);
  }

  // Flags: - 0 H C
  inline void ADD_HL_RR(uint16_t RR)
  {
    int result = m_hl.hl + RR;
    ResetFlag(FlagBitMask::Subtract);
    SetHalfCarryFlagBit11To12(m_hl.hl, RR);
    SetCarry16Bit(result);
    m_hl.hl = static_cast<uint16_t>(result);
  }

  // Flags: Z 0 1 0
  inline void AND_A_X(uint8_t val)
  {
    m_af.h &= val;
    SetZeroFlag(m_af.h);
    ResetFlag(FlagBitMask::Subtract);
    SetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
  }

  // Flags: Z 0 0 0
  inline void XOR_A_X(uint8_t val)
  {
    m_af.h ^= val;
    SetZeroFlag(m_af.h);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
  }

  // Flags: Z 0 0 0
  inline void OR_A_X(uint8_t val)
  {
    m_af.h |= val;
    SetZeroFlag(m_af.h);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
  }

  // Flags: Z 1 H C
  inline void CP_A_X(uint8_t val)
  {
    int result = m_af.h - val;
    SetZeroFlag(result);
    SetFlag(FlagBitMask::Subtract);
    SetHalfCarryFlagBit3To4(m_af.h, -val);
    SetCarry8Bit(result);
  }

  // Flags: 0 0 0 A7
  inline void RLCA()
  {
    m_af.h & 0b10000000 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    m_af.h = m_af.h << 1 | m_af.h >> 7;
    ResetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
  }

  // Flags: 0 0 0 A7
  inline void RLA()
  {
    uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
    m_af.h & 0b10000000 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    m_af.h = m_af.h << 1 | carry_bit;
    ResetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
  }

  // Flags: 0 0 0 A0
  inline void RRCA()
  {
    m_af.h & 0b00000001 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    m_af.h = m_af.h >> 1 | m_af.h << 7;
    ResetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
  }

  // Flags: 0 0 0 A0
  inline void RRA()
  {
    uint8_t carry_bit = ReadFlag(FlagBitMask::Carry);
    m_af.h & 0b00000001 ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    m_af.h = m_af.h >> 1 | carry_bit << 7;
    ResetFlag(FlagBitMask::Zero);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
  }

  // Flags: - 1 1 -
  inline void CPL()
  {
    m_af.h = ~m_af.h;
    SetFlag(FlagBitMask::Subtract);
    SetFlag(FlagBitMask::HalfCarry);
  }

  // Flags: Z - 0 C
  // https://forums.nesdev.org/viewtopic.php?t=15944#:~:text=The%20DAA%20instruction%20adjusts%20the,%2C%20lower%20nybble%2C%20or%20both.
  inline void DAA()
  {
    if (!ReadFlag(FlagBitMask::Subtract))
    {
      if (ReadFlag(FlagBitMask::Carry)     ||  m_af.h         > 0x99) { m_af.h += 0x60; SetFlag(FlagBitMask::Carry); }
      if (ReadFlag(FlagBitMask::HalfCarry) || (m_af.h & 0x0f) > 0x09)   m_af.h += 0x06;
    }
    else
    {
      if (ReadFlag(FlagBitMask::Carry))     m_af.h -= 0x60;
      if (ReadFlag(FlagBitMask::HalfCarry)) m_af.h -= 0x06;
    }

    SetZeroFlag(m_af.h);
    ResetFlag(FlagBitMask::HalfCarry);
  }

  // Flags: - 0 0 1
  inline void SCF()
  {
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    SetFlag(FlagBitMask::Carry);
  }

  // Flags: - 0 0 !C
  inline void CCF()
  {
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    ReadFlag(FlagBitMask::Carry) ? ResetFlag(FlagBitMask::Carry) : SetFlag(FlagBitMask::Carry);
  }

  // Flags: Z 0 H -
  inline void INC_R(uint8_t& R)
  {
    ADD_X(R, 1);
    ResetFlag(FlagBitMask::Subtract);
  }

  // Flags: Z 1 H -
  inline void DEC_R(uint8_t& R)
  {
    ADD_X(R, -1);
    SetFlag(FlagBitMask::Subtract);
  }

  // Flags: Z 0 H -
  inline void INC__RR(uint16_t RR)
  {
    ADD__RR(RR, 1);
  }

  // Flags: Z 1 H -
  inline void DEC__RR(uint16_t RR)
  {
    ADD__RR(RR, -1);
  }

  inline void PUSH_RR(uint16_t RR)
  {
    m_sp -= 2;
    WriteAddress(m_sp, RR & 0xFF);
    WriteAddress(m_sp + 1, RR >> 8);
  }

  // Flags: - - - -
  inline void JR(bool jump)
  {
    int8_t steps = ReadNextInt8();
    if (jump) m_pc += steps;
  }

  // Flags: - - - -
  inline void POP_RR(uint16_t& RR)
  {
    uint8_t l = ReadAddress(m_sp);
    m_sp += 1;
    uint8_t h = ReadAddress(m_sp);
    m_sp += 1;

    RR = (h << 8) | l;
  }

  // Flags: - - - -
  inline void POP_AF()
  {
    uint8_t l = ReadAddress(m_sp);
    m_sp += 1;
    uint8_t h = ReadAddress(m_sp);
    m_sp += 1;

    m_af.hl = (h << 8) | l;
    m_af.l &= 0xF0;
  }

  // Flags: - - - -
  inline void RET(bool ret)
  {
    if (ret) POP_RR(m_pc);
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
    if (jump) m_pc = address;
  }

  // Flags: - - - -
  inline void CALL(bool call)
  {
    uint16_t func = ReadNextUint16();
    if (call) { PUSH_RR(m_pc); m_pc = func; }
  }

  // Flags: - - - -
  inline void RST(RestartVector rst_vec)
  {
    PUSH_RR(m_pc);
    m_pc = static_cast<uint16_t>(rst_vec);
  }
#pragma endregion

#pragma region EXTENDED INSTRUCTIONS
  inline int OpCb0x00()
  {
    RLC_R(m_bc.h);
    return 8;
  }

  inline int OpCb0x01()
  {
    RLC_R(m_bc.l);
    return 8;
  }

  inline int OpCb0x02()
  {
    RLC_R(m_de.h);
    return 8;
  }

  inline int OpCb0x03()
  {
    RLC_R(m_de.l);
    return 8;
  }

  inline int OpCb0x04()
  {
    RLC_R(m_hl.h);
    return 8;
  }

  inline int OpCb0x05()
  {
    RLC_R(m_hl.l);
    return 8;
  }

  inline int OpCb0x06()
  {
    RLC__HL();
    return 16;
  }

  inline int OpCb0x07()
  {
    RLC_R(m_af.h);
    return 8;
  }

  inline int OpCb0x08()
  {
    RRC_R(m_bc.h);
    return 8;
  }

  inline int OpCb0x09()
  {
    RRC_R(m_bc.l);
    return 8;
  }

  inline int OpCb0x0A()
  {
    RRC_R(m_de.h);
    return 8;
  }

  inline int OpCb0x0B()
  {
    RRC_R(m_de.l);
    return 8;
  }

  inline int OpCb0x0C()
  {
    RRC_R(m_hl.h);
    return 8;
  }

  inline int OpCb0x0D()
  {
    RRC_R(m_hl.l);
    return 8;
  }

  inline int OpCb0x0E()
  {
    RRC__HL();
    return 16;
  }

  inline int OpCb0x0F()
  {
    RRC_R(m_af.h);
    return 8;
  }

  inline int OpCb0x10()
  {
    RL_R(m_bc.h);
    return 8;
  }

  inline int OpCb0x11()
  {
    RL_R(m_bc.l);
    return 8;
  }

  inline int OpCb0x12()
  {
    RL_R(m_de.h);
    return 8;
  }

  inline int OpCb0x13()
  {
    RL_R(m_de.l);
    return 8;
  }

  inline int OpCb0x14()
  {
    RL_R(m_hl.h);
    return 8;
  }

  inline int OpCb0x15()
  {
    RL_R(m_hl.l);
    return 8;
  }

  inline int OpCb0x16()
  {
    RL__HL();
    return 16;
  }

  inline int OpCb0x17()
  {
    RL_R(m_af.h);
    return 8;
  }

  inline int OpCb0x18()
  {
    RR_R(m_bc.h);
    return 8;
  }

  inline int OpCb0x19()
  {
    RR_R(m_bc.l);
    return 8;
  }

  inline int OpCb0x1A()
  {
    RR_R(m_de.h);
    return 8;
  }

  inline int OpCb0x1B()
  {
    RR_R(m_de.l);
    return 8;
  }

  inline int OpCb0x1C()
  {
    RR_R(m_hl.h);
    return 8;
  }

  inline int OpCb0x1D()
  {
    RR_R(m_hl.l);
    return 8;
  }

  inline int OpCb0x1E()
  {
    RR__HL();
    return 16;
  }

  inline int OpCb0x1F()
  {
    RR_R(m_af.h);
    return 8;
  }

  inline int OpCb0x20()
  {
    SLA_R(m_bc.h);
    return 8;
  }

  inline int OpCb0x21()
  {
    SLA_R(m_bc.l);
    return 8;
  }

  inline int OpCb0x22()
  {
    SLA_R(m_de.h);
    return 8;
  }

  inline int OpCb0x23()
  {
    SLA_R(m_de.l);
    return 8;
  }

  inline int OpCb0x24()
  {
    SLA_R(m_hl.h);
    return 8;
  }

  inline int OpCb0x25()
  {
    SLA_R(m_hl.l);
    return 8;
  }

  inline int OpCb0x26()
  {
    SLA__HL();
    return 16;
  }

  inline int OpCb0x27()
  {
    SLA_R(m_af.h);
    return 8;
  }

  inline int OpCb0x28()
  {
    SRA_R(m_bc.h);
    return 8;
  }

  inline int OpCb0x29()
  {
    SRA_R(m_bc.l);
    return 8;
  }

  inline int OpCb0x2A()
  {
    SRA_R(m_de.h);
    return 8;
  }

  inline int OpCb0x2B()
  {
    SRA_R(m_de.l);
    return 8;
  }

  inline int OpCb0x2C()
  {
    SRA_R(m_hl.h);
    return 8;
  }

  inline int OpCb0x2D()
  {
    SRA_R(m_hl.l);
    return 8;
  }

  inline int OpCb0x2E()
  {
    SRA__HL();
    return 16;
  }

  inline int OpCb0x2F()
  {
    SRA_R(m_af.h);
    return 8;
  }

  inline int OpCb0x30()
  {
    SWAP_R(m_bc.h);
    return 8;
  }

  inline int OpCb0x31()
  {
    SWAP_R(m_bc.l);
    return 8;
  }

  inline int OpCb0x32()
  {
    SWAP_R(m_de.h);
    return 8;
  }

  inline int OpCb0x33()
  {
    SWAP_R(m_de.l);
    return 8;
  }

  inline int OpCb0x34()
  {
    SWAP_R(m_hl.h);
    return 8;
  }

  inline int OpCb0x35()
  {
    SWAP_R(m_hl.l);
    return 8;
  }

  inline int OpCb0x36()
  {
    SWAP__HL();
    return 16;
  }

  inline int OpCb0x37()
  {
    SWAP_R(m_af.h);
    return 8;
  }

  inline int OpCb0x38()
  {
    SRL_R(m_bc.h);
    return 8;
  }

  inline int OpCb0x39()
  {
    SRL_R(m_bc.l);
    return 8;
  }

  inline int OpCb0x3A()
  {
    SRL_R(m_de.h);
    return 8;
  }

  inline int OpCb0x3B()
  {
    SRL_R(m_de.l);
    return 8;
  }

  inline int OpCb0x3C()
  {
    SRL_R(m_hl.h);
    return 8;
  }

  inline int OpCb0x3D()
  {
    SRL_R(m_hl.l);
    return 8;
  }

  inline int OpCb0x3E()
  {
    SRL__HL();
    return 16;
  }

  inline int OpCb0x3F()
  {
    SRL_R(m_af.h);
    return 8;
  }

  inline int OpCb0x40()
  {
    BIT_R(0b00000001, m_bc.h);
    return 8;
  }

  inline int OpCb0x41()
  {
    BIT_R(0b00000001, m_bc.l);
    return 8;
  }

  inline int OpCb0x42()
  {
    BIT_R(0b00000001, m_de.h);
    return 8;
  }

  inline int OpCb0x43()
  {
    BIT_R(0b00000001, m_de.l);
    return 8;
  }

  inline int OpCb0x44()
  {
    BIT_R(0b00000001, m_hl.h);
    return 8;
  }

  inline int OpCb0x45()
  {
    BIT_R(0b00000001, m_hl.l);
    return 8;
  }

  inline int OpCb0x46()
  {
    BIT__HL(0b00000001);
    return 12;
  }

  inline int OpCb0x47()
  {
    BIT_R(0b00000001, m_af.h);
    return 8;
  }

  inline int OpCb0x48()
  {
    BIT_R(0b00000010, m_bc.h);
    return 8;
  }

  inline int OpCb0x49()
  {
    BIT_R(0b00000010, m_bc.l);
    return 8;
  }

  inline int OpCb0x4A()
  {
    BIT_R(0b00000010, m_de.h);
    return 8;
  }

  inline int OpCb0x4B()
  {
    BIT_R(0b00000010, m_de.l);
    return 8;
  }

  inline int OpCb0x4C()
  {
    BIT_R(0b00000010, m_hl.h);
    return 8;
  }

  inline int OpCb0x4D()
  {
    BIT_R(0b00000010, m_hl.l);
    return 8;
  }

  inline int OpCb0x4E()
  {
    BIT__HL(0b00000010);
    return 12;
  }

  inline int OpCb0x4F()
  {
    BIT_R(0b00000010, m_af.h);
    return 8;
  }

  inline int OpCb0x50()
  {
    BIT_R(0b00000100, m_bc.h);
    return 8;
  }

  inline int OpCb0x51()
  {
    BIT_R(0b00000100, m_bc.l);
    return 8;
  }

  inline int OpCb0x52()
  {
    BIT_R(0b00000100, m_de.h);
    return 8;
  }

  inline int OpCb0x53()
  {
    BIT_R(0b00000100, m_de.l);
    return 8;
  }

  inline int OpCb0x54()
  {
    BIT_R(0b00000100, m_hl.h);
    return 8;
  }

  inline int OpCb0x55()
  {
    BIT_R(0b00000100, m_hl.l);
    return 8;
  }

  inline int OpCb0x56()
  {
    BIT__HL(0b00000100);
    return 12;
  }

  inline int OpCb0x57()
  {
    BIT_R(0b00000100, m_af.h);
    return 8;
  }

  inline int OpCb0x58()
  {
    BIT_R(0b00001000, m_bc.h);
    return 8;
  }

  inline int OpCb0x59()
  {
    BIT_R(0b00001000, m_bc.l);
    return 8;
  }

  inline int OpCb0x5A()
  {
    BIT_R(0b00001000, m_de.h);
    return 8;
  }

  inline int OpCb0x5B()
  {
    BIT_R(0b00001000, m_de.l);
    return 8;
  }

  inline int OpCb0x5C()
  {
    BIT_R(0b00001000, m_hl.h);
    return 8;
  }

  inline int OpCb0x5D()
  {
    BIT_R(0b00001000, m_hl.l);
    return 8;
  }

  inline int OpCb0x5E()
  {
    BIT__HL(0b00001000);
    return 12;
  }

  inline int OpCb0x5F()
  {
    BIT_R(0b00001000, m_af.h);
    return 8;
  }

  inline int OpCb0x60()
  {
    BIT_R(0b00010000, m_bc.h);
    return 8;
  }

  inline int OpCb0x61()
  {
    BIT_R(0b00010000, m_bc.l);
    return 8;
  }

  inline int OpCb0x62()
  {
    BIT_R(0b00010000, m_de.h);
    return 8;
  }

  inline int OpCb0x63()
  {
    BIT_R(0b00010000, m_de.l);
    return 8;
  }

  inline int OpCb0x64()
  {
    BIT_R(0b00010000, m_hl.h);
    return 8;
  }

  inline int OpCb0x65()
  {
    BIT_R(0b00010000, m_hl.l);
    return 8;
  }

  inline int OpCb0x66()
  {
    BIT__HL(0b00010000);
    return 12;
  }

  inline int OpCb0x67()
  {
    BIT_R(0b00010000, m_af.h);
    return 8;
  }

  inline int OpCb0x68()
  {
    BIT_R(0b00100000, m_bc.h);
    return 8;
  }

  inline int OpCb0x69()
  {
    BIT_R(0b00100000, m_bc.l);
    return 8;
  }

  inline int OpCb0x6A()
  {
    BIT_R(0b00100000, m_de.h);
    return 8;
  }

  inline int OpCb0x6B()
  {
    BIT_R(0b00100000, m_de.l);
    return 8;
  }

  inline int OpCb0x6C()
  {
    BIT_R(0b00100000, m_hl.h);
    return 8;
  }

  inline int OpCb0x6D()
  {
    BIT_R(0b00100000, m_hl.l);
    return 8;
  }

  inline int OpCb0x6E()
  {
    BIT__HL(0b00100000);
    return 12;
  }

  inline int OpCb0x6F()
  {
    BIT_R(0b00100000, m_af.h);
    return 8;
  }

  inline int OpCb0x70()
  {
    BIT_R(0b01000000, m_bc.h);
    return 8;
  }

  inline int OpCb0x71()
  {
    BIT_R(0b01000000, m_bc.l);
    return 8;
  }

  inline int OpCb0x72()
  {
    BIT_R(0b01000000, m_de.h);
    return 8;
  }

  inline int OpCb0x73()
  {
    BIT_R(0b01000000, m_de.l);
    return 8;
  }

  inline int OpCb0x74()
  {
    BIT_R(0b01000000, m_hl.h);
    return 8;
  }

  inline int OpCb0x75()
  {
    BIT_R(0b01000000, m_hl.l);
    return 8;
  }

  inline int OpCb0x76()
  {
    BIT__HL(0b01000000);
    return 12;
  }

  inline int OpCb0x77()
  {
    BIT_R(0b01000000, m_af.h);
    return 8;
  }

  inline int OpCb0x78()
  {
    BIT_R(0b10000000, m_bc.h);
    return 8;
  }

  inline int OpCb0x79()
  {
    BIT_R(0b10000000, m_bc.l);
    return 8;
  }

  inline int OpCb0x7A()
  {
    BIT_R(0b10000000, m_de.h);
    return 8;
  }

  inline int OpCb0x7B()
  {
    BIT_R(0b10000000, m_de.l);
    return 8;
  }

  inline int OpCb0x7C()
  {
    BIT_R(0b10000000, m_hl.h);
    return 8;
  }

  inline int OpCb0x7D()
  {
    BIT_R(0b10000000, m_hl.l);
    return 8;
  }

  inline int OpCb0x7E()
  {
    BIT__HL(0b10000000);
    return 12;
  }

  inline int OpCb0x7F()
  {
    BIT_R(0b10000000, m_af.h);
    return 8;
  }

  inline int OpCb0x80()
  {
    RES_R(0b00000001, m_bc.h);
    return 8;
  }

  inline int OpCb0x81()
  {
    RES_R(0b00000001, m_bc.l);
    return 8;
  }

  inline int OpCb0x82()
  {
    RES_R(0b00000001, m_de.h);
    return 8;
  }

  inline int OpCb0x83()
  {
    RES_R(0b00000001, m_de.l);
    return 8;
  }

  inline int OpCb0x84()
  {
    RES_R(0b00000001, m_hl.h);
    return 8;
  }

  inline int OpCb0x85()
  {
    RES_R(0b00000001, m_hl.l);
    return 8;
  }

  inline int OpCb0x86()
  {
    RES__HL(0b00000001);
    return 16;
  }

  inline int OpCb0x87()
  {
    RES_R(0b00000001, m_af.h);
    return 8;
  }

  inline int OpCb0x88()
  {
    RES_R(0b00000010, m_bc.h);
    return 8;
  }

  inline int OpCb0x89()
  {
    RES_R(0b00000010, m_bc.l);
    return 8;
  }

  inline int OpCb0x8A()
  {
    RES_R(0b00000010, m_de.h);
    return 8;
  }

  inline int OpCb0x8B()
  {
    RES_R(0b00000010, m_de.l);
    return 8;
  }

  inline int OpCb0x8C()
  {
    RES_R(0b00000010, m_hl.h);
    return 8;
  }

  inline int OpCb0x8D()
  {
    RES_R(0b00000010, m_hl.l);
    return 8;
  }

  inline int OpCb0x8E()
  {
    RES__HL(0b00000010);
    return 16;
  }

  inline int OpCb0x8F()
  {
    RES_R(0b00000010, m_af.h);
    return 8;
  }

  inline int OpCb0x90()
  {
    RES_R(0b00000100, m_bc.h);
    return 8;
  }

  inline int OpCb0x91()
  {
    RES_R(0b00000100, m_bc.l);
    return 8;
  }

  inline int OpCb0x92()
  {
    RES_R(0b00000100, m_de.h);
    return 8;
  }

  inline int OpCb0x93()
  {
    RES_R(0b00000100, m_de.l);
    return 8;
  }

  inline int OpCb0x94()
  {
    RES_R(0b00000100, m_hl.h);
    return 8;
  }

  inline int OpCb0x95()
  {
    RES_R(0b00000100, m_hl.l);
    return 8;
  }

  inline int OpCb0x96()
  {
    RES__HL(0b00000100);
    return 16;
  }

  inline int OpCb0x97()
  {
    RES_R(0b00000100, m_af.h);
    return 8;
  }

  inline int OpCb0x98()
  {
    RES_R(0b00001000, m_bc.h);
    return 8;
  }

  inline int OpCb0x99()
  {
    RES_R(0b00001000, m_bc.l);
    return 8;
  }

  inline int OpCb0x9A()
  {
    RES_R(0b00001000, m_de.h);
    return 8;
  }

  inline int OpCb0x9B()
  {
    RES_R(0b00001000, m_de.l);
    return 8;
  }

  inline int OpCb0x9C()
  {
    RES_R(0b00001000, m_hl.h);
    return 8;
  }

  inline int OpCb0x9D()
  {
    RES_R(0b00001000, m_hl.l);
    return 8;
  }

  inline int OpCb0x9E()
  {
    RES__HL(0b00001000);
    return 16;
  }

  inline int OpCb0x9F()
  {
    RES_R(0b00001000, m_af.h);
    return 8;
  }

  inline int OpCb0xA0()
  {
    RES_R(0b00010000, m_bc.h);
    return 8;
  }

  inline int OpCb0xA1()
  {
    RES_R(0b00010000, m_bc.l);
    return 8;
  }

  inline int OpCb0xA2()
  {
    RES_R(0b00010000, m_de.h);
    return 8;
  }

  inline int OpCb0xA3()
  {
    RES_R(0b00010000, m_de.l);
    return 8;
  }

  inline int OpCb0xA4()
  {
    RES_R(0b00010000, m_hl.h);
    return 8;
  }

  inline int OpCb0xA5()
  {
    RES_R(0b00010000, m_hl.l);
    return 8;
  }

  inline int OpCb0xA6()
  {
    RES__HL(0b00010000);
    return 16;
  }

  inline int OpCb0xA7()
  {
    RES_R(0b00010000, m_af.h);
    return 8;
  }

  inline int OpCb0xA8()
  {
    RES_R(0b00100000, m_bc.h);
    return 8;
  }

  inline int OpCb0xA9()
  {
    RES_R(0b00100000, m_bc.l);
    return 8;
  }

  inline int OpCb0xAA()
  {
    RES_R(0b00100000, m_de.h);
    return 8;
  }

  inline int OpCb0xAB()
  {
    RES_R(0b00100000, m_de.l);
    return 8;
  }

  inline int OpCb0xAC()
  {
    RES_R(0b00100000, m_hl.h);
    return 8;
  }

  inline int OpCb0xAD()
  {
    RES_R(0b00100000, m_hl.l);
    return 8;
  }

  inline int OpCb0xAE()
  {
    RES__HL(0b00100000);
    return 16;
  }

  inline int OpCb0xAF()
  {
    RES_R(0b00100000, m_af.h);
    return 8;
  }

  inline int OpCb0xB0()
  {
    RES_R(0b01000000, m_bc.h);
    return 8;
  }

  inline int OpCb0xB1()
  {
    RES_R(0b01000000, m_bc.l);
    return 8;
  }

  inline int OpCb0xB2()
  {
    RES_R(0b01000000, m_de.h);
    return 8;
  }

  inline int OpCb0xB3()
  {
    RES_R(0b01000000, m_de.l);
    return 8;
  }

  inline int OpCb0xB4()
  {
    RES_R(0b01000000, m_hl.h);
    return 8;
  }

  inline int OpCb0xB5()
  {
    RES_R(0b01000000, m_hl.l);
    return 8;
  }

  inline int OpCb0xB6()
  {
    RES__HL(0b01000000);
    return 16;
  }

  inline int OpCb0xB7()
  {
    RES_R(0b01000000, m_af.h);
    return 8;
  }

  inline int OpCb0xB8()
  {
    RES_R(0b10000000, m_bc.h);
    return 8;
  }

  inline int OpCb0xB9()
  {
    RES_R(0b10000000, m_bc.l);
    return 8;
  }

  inline int OpCb0xBA()
  {
    RES_R(0b10000000, m_de.h);
    return 8;
  }

  inline int OpCb0xBB()
  {
    RES_R(0b10000000, m_de.l);
    return 8;
  }

  inline int OpCb0xBC()
  {
    RES_R(0b10000000, m_hl.h);
    return 8;
  }

  inline int OpCb0xBD()
  {
    RES_R(0b10000000, m_hl.l);
    return 8;
  }

  inline int OpCb0xBE()
  {
    RES__HL(0b10000000);
    return 16;
  }

  inline int OpCb0xBF()
  {
    RES_R(0b10000000, m_af.h);
    return 8;
  }

  inline int OpCb0xC0()
  {
    SET_R(0b00000001, m_bc.h);
    return 8;
  }

  inline int OpCb0xC1()
  {
    SET_R(0b00000001, m_bc.l);
    return 8;
  }

  inline int OpCb0xC2()
  {
    SET_R(0b00000001, m_de.h);
    return 8;
  }

  inline int OpCb0xC3()
  {
    SET_R(0b00000001, m_de.l);
    return 8;
  }

  inline int OpCb0xC4()
  {
    SET_R(0b00000001, m_hl.h);
    return 8;
  }

  inline int OpCb0xC5()
  {
    SET_R(0b00000001, m_hl.l);
    return 8;
  }

  inline int OpCb0xC6()
  {
    SET__HL(0b00000001);
    return 16;
  }

  inline int OpCb0xC7()
  {
    SET_R(0b00000001, m_af.h);
    return 8;
  }

  inline int OpCb0xC8()
  {
    SET_R(0b00000010, m_bc.h);
    return 8;
  }

  inline int OpCb0xC9()
  {
    SET_R(0b00000010, m_bc.l);
    return 8;
  }

  inline int OpCb0xCA()
  {
    SET_R(0b00000010, m_de.h);
    return 8;
  }

  inline int OpCb0xCB()
  {
    SET_R(0b00000010, m_de.l);
    return 8;
  }

  inline int OpCb0xCC()
  {
    SET_R(0b00000010, m_hl.h);
    return 8;
  }

  inline int OpCb0xCD()
  {
    SET_R(0b00000010, m_hl.l);
    return 8;
  }

  inline int OpCb0xCE()
  {
    SET__HL(0b00000010);
    return 16;
  }

  inline int OpCb0xCF()
  {
    SET_R(0b00000010, m_af.h);
    return 8;
  }

  inline int OpCb0xD0()
  {
    SET_R(0b00000100, m_bc.h);
    return 8;
  }

  inline int OpCb0xD1()
  {
    SET_R(0b00000100, m_bc.l);
    return 8;
  }

  inline int OpCb0xD2()
  {
    SET_R(0b00000100, m_de.h);
    return 8;
  }

  inline int OpCb0xD3()
  {
    SET_R(0b00000100, m_de.l);
    return 8;
  }

  inline int OpCb0xD4()
  {
    SET_R(0b00000100, m_hl.h);
    return 8;
  }

  inline int OpCb0xD5()
  {
    SET_R(0b00000100, m_hl.l);
    return 8;
  }

  inline int OpCb0xD6()
  {
    SET__HL(0b00000100);
    return 16;
  }

  inline int OpCb0xD7()
  {
    SET_R(0b00000100, m_af.h);
    return 8;
  }

  inline int OpCb0xD8()
  {
    SET_R(0b00001000, m_bc.h);
    return 8;
  }

  inline int OpCb0xD9()
  {
    SET_R(0b00001000, m_bc.l);
    return 8;
  }

  inline int OpCb0xDA()
  {
    SET_R(0b00001000, m_de.h);
    return 8;
  }

  inline int OpCb0xDB()
  {
    SET_R(0b00001000, m_de.l);
    return 8;
  }

  inline int OpCb0xDC()
  {
    SET_R(0b00001000, m_hl.h);
    return 8;
  }

  inline int OpCb0xDD()
  {
    SET_R(0b00001000, m_hl.l);
    return 8;
  }

  inline int OpCb0xDE()
  {
    SET__HL(0b00001000);
    return 16;
  }

  inline int OpCb0xDF()
  {
    SET_R(0b00001000, m_af.h);
    return 8;
  }

  inline int OpCb0xE0()
  {
    SET_R(0b00010000, m_bc.h);
    return 8;
  }

  inline int OpCb0xE1()
  {
    SET_R(0b00010000, m_bc.l);
    return 8;
  }

  inline int OpCb0xE2()
  {
    SET_R(0b00010000, m_de.h);
    return 8;
  }

  inline int OpCb0xE3()
  {
    SET_R(0b00010000, m_de.l);
    return 8;
  }

  inline int OpCb0xE4()
  {
    SET_R(0b00010000, m_hl.h);
    return 8;
  }

  inline int OpCb0xE5()
  {
    SET_R(0b00010000, m_hl.l);
    return 8;
  }

  inline int OpCb0xE6()
  {
    SET__HL(0b00010000);
    return 16;
  }

  inline int OpCb0xE7()
  {
    SET_R(0b00010000, m_af.h);
    return 8;
  }

  inline int OpCb0xE8()
  {
    SET_R(0b00100000, m_bc.h);
    return 8;
  }

  inline int OpCb0xE9()
  {
    SET_R(0b00100000, m_bc.l);
    return 8;
  }

  inline int OpCb0xEA()
  {
    SET_R(0b00100000, m_de.h);
    return 8;
  }

  inline int OpCb0xEB()
  {
    SET_R(0b00100000, m_de.l);
    return 8;
  }
  inline int OpCb0xEC()
  {
    SET_R(0b00100000, m_hl.h);
    return 8;
  }

  inline int OpCb0xED()
  {
    SET_R(0b00100000, m_hl.l);
    return 8;
  }

  inline int OpCb0xEE()
  {
    SET__HL(0b00100000);
    return 16;
  }

  inline int OpCb0xEF()
  {
    SET_R(0b00100000, m_af.h);
    return 8;
  }

  inline int OpCb0xF0()
  {
    SET_R(0b01000000, m_bc.h);
    return 8;
  }

  inline int OpCb0xF1()
  {
    SET_R(0b01000000, m_bc.l);
    return 8;
  }

  inline int OpCb0xF2()
  {
    SET_R(0b01000000, m_de.h);
    return 8;
  }

  inline int OpCb0xF3()
  {
    SET_R(0b01000000, m_de.l);
    return 8;
  }

  inline int OpCb0xF4()
  {
    SET_R(0b01000000, m_hl.h);
    return 8;
  }

  inline int OpCb0xF5()
  {
    SET_R(0b01000000, m_hl.l);
    return 8;
  }

  inline int OpCb0xF6()
  {
    SET__HL(0b01000000);
    return 16;
  }

  inline int OpCb0xF7()
  {
    SET_R(0b01000000, m_af.h);
    return 8;
  }

  inline int OpCb0xF8()
  {
    SET_R(0b10000000, m_bc.h);
    return 8;
  }

  inline int OpCb0xF9()
  {
    SET_R(0b10000000, m_bc.l);
    return 8;
  }

  inline int OpCb0xFA()
  {
    SET_R(0b10000000, m_de.h);
    return 8;
  }

  inline int OpCb0xFB()
  {
    SET_R(0b10000000, m_de.l);
    return 8;
  }

  inline int OpCb0xFC()
  {
    SET_R(0b10000000, m_hl.h);
    return 8;
  }

  inline int OpCb0xFD()
  {
    SET_R(0b10000000, m_hl.l);
    return 8;
  }

  inline int OpCb0xFE()
  {
    SET__HL(0b10000000);
    return 16;
  }

  inline int OpCb0xFF()
  {
    SET_R(0b10000000, m_af.h);
    return 8;
  }

#pragma endregion

#pragma region GENERALIZED EXTENDED INSTRUCTIONS
  //Flags: Z 0 0 Bit7
  inline void RLC__HL()
  {
    uint8_t val = ReadAddress(m_hl.hl);
    (val & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val << 1 | val >> 7;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(m_hl.hl, result);
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
    uint8_t val = ReadAddress(m_hl.hl);
    (val & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val >> 1 | val << 7;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(m_hl.hl, result);
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
    uint8_t val = ReadAddress(m_hl.hl);
    (val & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val << 1 | carry_bit;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(m_hl.hl, result);
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
    uint8_t val = ReadAddress(m_hl.hl);
    (val & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val >> 1 | (carry_bit << 7);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(m_hl.hl, result);
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
    uint8_t val = ReadAddress(m_hl.hl);
    (val & 0b10000000) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val << 1;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(m_hl.hl, result);
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
    uint8_t val = ReadAddress(m_hl.hl);
    (val & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val >> 1 | (val & 0b10000000);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(m_hl.hl, result);
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
    uint8_t val = ReadAddress(m_hl.hl);
    uint8_t result = (val >> 4) | (val << 4);
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    ResetFlag(FlagBitMask::Carry);
    WriteAddress(m_hl.hl, result);
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
    uint8_t val = ReadAddress(m_hl.hl);
    (val & 0x1) ? SetFlag(FlagBitMask::Carry) : ResetFlag(FlagBitMask::Carry);
    uint8_t result = val >> 1;
    SetZeroFlag(result);
    ResetFlag(FlagBitMask::Subtract);
    ResetFlag(FlagBitMask::HalfCarry);
    WriteAddress(m_hl.hl, result);
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
    uint8_t val = ReadAddress(m_hl.hl);
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
    uint8_t val = ReadAddress(m_hl.hl);
    uint8_t result = val & ~bit_mask;
    WriteAddress(m_hl.hl, result);
  }

  // Flags: - - - -
  inline void RES_R(uint8_t bit_mask, uint8_t& R)
  {
    R &= ~bit_mask;
  }

  // Flags: - - - -
  inline void SET__HL(uint8_t bit_mask)
  {
    uint8_t val = ReadAddress(m_hl.hl);
    uint8_t result = val | bit_mask;
    WriteAddress(m_hl.hl, result);
  }

  // Flags: - - - -
  inline void SET_R(uint8_t bit_mask, uint8_t& R)
  {
    R |= bit_mask;
  }
#pragma endregion
};
