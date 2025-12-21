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
  
  inline constexpr uint8_t GetTimerBitMask(TimerBitMask timer_bit_mask)
  {
    return static_cast<uint8_t>(timer_bit_mask);
  }

  inline constexpr uint8_t GetTimerClockSelect(TimerClockSelect timer_clock_select)
  {
    return static_cast<uint8_t>(timer_clock_select);
  }

  TimerController(InterruptReceiver& interrupt_receiver) : InterruptProvider(interrupt_receiver, InterruptBitMask::TIMER) {}
  ~TimerController() {}

  bool HandleWrite(uint16_t address, uint8_t val);
  uint8_t HandleRead(uint16_t address) const;

  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  inline bool UpdateSysClock(uint16_t sys_clk)
  {
    uint16_t last_cycle_count = m_sys_clk;
    m_sys_clk = sys_clk;
    UpdateTima(last_cycle_count);
    return FallingEdgeDetect(last_cycle_count, DIV_APU_BIT_MASK);
  }

  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  inline bool Update()
  {
    return UpdateSysClock(m_sys_clk + 4);
  }

private:

  inline bool FallingEdgeDetect(uint16_t last_cycle_count, uint16_t div_bit_mask)
  {
    return (last_cycle_count & div_bit_mask) && !(m_sys_clk & div_bit_mask);
  }

  inline void TickTima()
  {
    m_tima += 1;
    if (m_tima == 0) 
      TimaOverflow();
  }

  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  inline bool HandleDivWrite()
  {
    return UpdateSysClock(0);
  }

  inline void HandleTimaWrite(uint8_t val)
  {
    if (m_tima_reload_cycle != 4)
      m_tima = val;

    m_tima_reload_cycle = 8;
  }

  inline void HandleTmaWrite(uint8_t val)
  {
    if (m_tima_reload_cycle == 4)
      m_tima = val;

    m_tma = val;
  }

  inline void HandleTacWrite(uint8_t val)
  {
    if (m_timer_enabled && !(val & GetTimerBitMask(TimerBitMask::ENABLE)))
    {
      if (m_sys_clk & m_div_bit_mask)
        TickTima();
    }

    m_tac = val;
    m_div_bit_mask = DIV_BIT_MASK_LUT[m_tac & GetTimerBitMask(TimerBitMask::CLOCK_SELECT)];
    m_timer_enabled = m_tac & static_cast<uint8_t>(TimerBitMask::ENABLE);
  }

  inline void TimaOverflow()
  {
    m_tima_reload_cycle = 0;
    m_tima = 0;
  }

  inline void ReloadTima()
  {
    TriggerInterrupt();
    m_tima = m_tma;
  }

  inline void UpdateTima(uint16_t last_cycle_count)
  {
    if (m_tima_reload_cycle < 8)
    {
      m_tima_reload_cycle += 4;

      if (m_tima_reload_cycle == 4) 
        ReloadTima();
    }

    if (m_timer_enabled)
    {
      if (FallingEdgeDetect(last_cycle_count, m_div_bit_mask))
        TickTima();
    }
  }

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
