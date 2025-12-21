#pragma once

#include "InterruptProvider.h"

#include <stdint.h>

class TimerController : public InterruptProvider
{
public:
  constexpr static uint8_t  DEFAULT_READ = 0xFF;
  constexpr static uint16_t DIV_APU_BIT_MASK = 0b1000000000000;

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
    CLOCK_SELECT_0  = 0b00000001, // R/W
    CLOCK_SELECT_1  = 0b00000010, // R/W
    CLOCK_SELECT_01 = 0b00000011,
    ENABLE          = 0b00000100  // R/W
  };

  enum class TimerClockSelect : uint8_t
  {
    HZ4096   = 0b00,
    HZ262144 = 0b01,
    HZ65536  = 0b10,
    HZ16384  = 0b11
  };
  
  inline constexpr uint8_t GetBitMask(TimerBitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  TimerController(InterruptReceiver& interrupt_receiver) : InterruptProvider(interrupt_receiver, InterruptBitMask::TIMER) {}
  ~TimerController() {}

  bool HandleWrite(uint16_t address, uint8_t val);
  uint8_t HandleRead(uint16_t address) const;

  inline bool Enabled() const
  {
    return m_tac & static_cast<uint8_t>(TimerBitMask::ENABLE);
  }

  inline bool UpdateApu(uint16_t last_cycle_count)
  {
    return (last_cycle_count & DIV_APU_BIT_MASK) && !(m_sys_clk & DIV_APU_BIT_MASK);
  }

  inline bool Update()
  {
    uint16_t last_cycle_count = m_sys_clk;
    m_sys_clk += 4;
    UpdateTima(last_cycle_count);
    bool apu_tick = UpdateApu(last_cycle_count);
    return apu_tick;
  }

  inline bool HandleDivWrite()
  {
    uint16_t last_cycle_count = m_sys_clk;
    m_sys_clk = 0;
    UpdateTima(last_cycle_count);
    bool apu_tick = UpdateApu(last_cycle_count);
    return apu_tick;
  }

private:
  constexpr uint16_t GetDivBitMask();

  inline void HandleTimaWrite(uint8_t val)
  {
    if (m_tima_reload_cycle_count != 4)
      m_tima = val;

    m_tima_reload_cycle_count = 8;
  }

  inline void HandleTmaWrite(uint8_t val)
  {
    if (m_tima_reload_cycle_count == 4)
      m_tima = val;

    m_tma = val;
  }

  inline void HandleTacWrite(uint8_t val)
  {
    if (Enabled() && !(val & GetBitMask(TimerBitMask::ENABLE)))
    {
      if (m_sys_clk & GetDivBitMask())
      {
        m_tima += 1;
        if (m_tima == 0) TimaOverflow();
      }
    }

    m_tac = val;
  }

  inline void TimaOverflow()
  {
    m_tima_reload_cycle_count = 0;
    m_tima = 0;
  }

  inline void ReloadTima()
  {
    TriggerInterrupt();
    m_tima = static_cast<uint8_t>(m_tma);
  }

  inline void UpdateTima(uint16_t last_cycle_count)
  {
    if (m_tima_reload_cycle_count < 8)
    {
      m_tima_reload_cycle_count += 4;
      if (m_tima_reload_cycle_count == 4) ReloadTima();
    }

    if (Enabled())
    {
      if ((last_cycle_count & GetDivBitMask()) && !(m_sys_clk & GetDivBitMask()))
      {
        m_tima += 1;
        if (m_tima == 0) TimaOverflow();
      }
    }
  }

  uint8_t m_tima{};
  uint8_t m_tma{};
  uint8_t m_tac{};

  uint16_t m_sys_clk{};

  uint8_t m_tima_reload_cycle_count = 8;
};
