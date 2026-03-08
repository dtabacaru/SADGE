#pragma once

#include "InterruptProvider.h"

#include <array>
#include <stdint.h>

class TimerController : public InterruptProvider
{
public:
  constexpr static uint8_t  DEFAULT_READ = 0xFF;
  constexpr static uint16_t DIV_APU_BIT_MASK = 0b1000000000000;
  constexpr static std::array<uint16_t, 4> DIV_BIT_MASK_LUT = {
    static_cast<uint16_t>(1 << 9),
    static_cast<uint16_t>(1 << 3),
    static_cast<uint16_t>(1 << 5),
    static_cast<uint16_t>(1 << 7),
  };

  enum class TimerAddress : uint16_t
  {
    DIV   = 0xFF04,
    TIMA  = 0xFF05,
    TMA   = 0xFF06,
    TAC   = 0xFF07,
    START = DIV,
    END   = TAC
  };

  enum class TimerBitMask : uint8_t
  {
    CLOCK_SELECT = 0b00000011,
    ENABLE       = 0b00000100
  };

  enum class TimerClockSelect : uint8_t
  {
    HZ4096   = 0b00,
    HZ262144 = 0b01,
    HZ65536  = 0b10,
    HZ16384  = 0b11
  };
  
  constexpr TimerAddress GetTimerAddress(uint16_t address) const
  {
    return static_cast<TimerAddress>(address);
  }

  constexpr uint8_t GetTimerBitMask(TimerBitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  constexpr uint8_t GetTimerClockSelect(TimerClockSelect cs)
  {
    return static_cast<uint8_t>(cs);
  }

  TimerController(InterruptReceiver& interrupt_receiver);

  bool HandleWrite(uint16_t address, uint8_t val);
  uint8_t HandleRead(uint16_t address) const;

  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  bool Update();

private:
  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  bool UpdateSysClock(uint16_t sys_clk);
  bool FallingEdgeDetect(uint16_t last_cycle_count, uint16_t div_bit_mask);
  void TickTima();
  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  bool HandleDivWrite();
  void HandleTimaWrite(uint8_t val);
  void HandleTmaWrite(uint8_t val);
  void HandleTacWrite(uint8_t val);
  void TimaOverflow();
  void ReloadTima();
  void UpdateTima(uint16_t last_cycle_count);

  bool m_timer_enabled{};

  uint8_t m_tima{};
  uint8_t m_tma{};
  uint8_t m_tac{};

  uint16_t m_div_bit_mask{DIV_BIT_MASK_LUT[GetTimerClockSelect(TimerClockSelect::HZ4096)]};
  uint16_t m_sys_clk{};

  //
  // Handle timer esoteric behaviour
  //
  // 1. When TIMA overflows from incrementing on T cycle 0, TIMA is reloaded on the following M cycle; T cycle 4.
  //   -set m_tima_reload_cycle = 0 on overflow
  //   -if m_tima_reload_cycle == 4, TIMA is reloaded
  // 2. On cycle 4, if TMA is written to, TIMA is also set to the same value.
  //   -if m_tima_reload_cycle == 4 on TMA write, TIMA=val
  // 3. On cycle 4, if TIMA is written to, the write is ignored.
  //   -if m_tima_reload_cycle != 4 on TIMA write, TIMA = val
  // 4. If TIMA is written to during the overflow cycle, the overflow is ignored
  //   -set m_tima_reload_cycle = 8 on TIMA write so reload stops
  //
  // Defaults to 8 = no current reload
  //
  uint8_t m_tima_reload_cycle{8};
};
