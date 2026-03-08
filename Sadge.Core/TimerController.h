#pragma once

#include "InterruptProvider.h"

#include <array>

enum class TimerAddress : uint16_t
{
  DIV = 0xFF04,
  TIMA = 0xFF05,
  TMA = 0xFF06,
  TAC = 0xFF07,
  START = DIV,
  END = TAC
};

class TimerController : public InterruptProvider
{
public:
  
  constexpr TimerAddress GetTimerAddress(uint16_t address) const
  {
    return static_cast<TimerAddress>(address);
  }

  TimerController(InterruptReceiver& interrupt_receiver);

  bool HandleWrite(uint16_t address, uint8_t val);
  uint8_t HandleRead(uint16_t address) const;

  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  bool Update();

private:
  constexpr static uint8_t  NO_RELOAD = 2;
  constexpr static uint8_t  DEFAULT_READ = 0xFF;
  constexpr static uint16_t DIV_APU_BIT_MASK = 0b10000000000;
  constexpr static std::array<uint16_t, 4> DIV_BIT_MASK_LUT = {
    0b10000000,
    0b00000010,
    0b00001000,
    0b00100000
  };

  enum class TimerBitMask : uint8_t
  {
    CLOCK_SELECT = 0b00000011,
    ENABLE = 0b00000100
  };

  enum class TimerClockSelect : uint8_t
  {
    HZ4096   = 0b00,
    HZ262144 = 0b01,
    HZ65536  = 0b10,
    HZ16384  = 0b11
  };

  constexpr uint8_t GetTimerBitMask(TimerBitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  constexpr uint8_t GetTimerClockSelect(TimerClockSelect cs)
  {
    return static_cast<uint8_t>(cs);
  }

  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  bool UpdateSysClock(uint16_t newClk);
  bool FallingEdgeDetect(uint16_t oldClk, uint16_t mask);
  void TickTima();
  /// <returns>Flag indicating whether an apu DIV event was triggered this cycle</returns>
  bool HandleDivWrite();
  void HandleTimaWrite(uint8_t val);
  void HandleTmaWrite(uint8_t val);
  void HandleTacWrite(uint8_t val);
  void TimaOverflow();
  void ReloadTima();
  void UpdateTima(uint16_t oldClk);

  bool mTimerEnabled{};

  uint8_t mTima{};
  uint8_t mTma{};
  uint8_t mTac{};

  uint16_t mMask{DIV_BIT_MASK_LUT[GetTimerClockSelect(TimerClockSelect::HZ4096)]};
  uint16_t mClk{};

  //
  // Handle timer esoteric behaviour
  //
  // 1. When TIMA overflows from incrementing on T cycle 0, TIMA is reloaded on the following M cycle.
  //   -set m_tima_reload_cycle = 0 on overflow
  //   -if m_tima_reload_cycle == 1, TIMA is reloaded
  // 2. On cycle 1, if TMA is written to, TIMA is also set to the same value.
  //   -if m_tima_reload_cycle == 1 on TMA write, TIMA=val
  // 3. On cycle 1, if TIMA is written to, the write is ignored.
  //   -if m_tima_reload_cycle != 1 on TIMA write, TIMA = val
  // 4. If TIMA is written to during the overflow cycle, the overflow is ignored
  //   -set m_tima_reload_cycle = 2 on TIMA write so reload stops
  //
  uint8_t mReloadCycle{NO_RELOAD};
};
