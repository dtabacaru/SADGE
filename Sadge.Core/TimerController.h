#pragma once

#include "InterruptProvider.h"

#include <array>

class TimerController : public InterruptProvider
{
public:

  enum class Address : uint16_t
  {
    DIV = 0xFF04,
    TIMA = 0xFF05,
    TMA = 0xFF06,
    TAC = 0xFF07,
    START = DIV,
    END = TAC
  };

  constexpr static uint16_t GetAddress(Address addr)
  {
    return static_cast<uint16_t>(addr);
  }

  TimerController(InterruptReceiver& intRec,
                  uint8_t& dataBus,
                  uint16_t& addrBus);

  void Write();
  void Read() const;

  uint16_t GetClk() const;

  void Tick();

private:
  
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

  constexpr static uint8_t GetTimerBitMask(TimerBitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  constexpr static uint8_t GetTimerClockSelect(TimerClockSelect cs)
  {
    return static_cast<uint8_t>(cs);
  }

  void TickSysClk(uint16_t newClk);
  void TickReload();
  void TickTima();
  void HandleTimaWrite();
  void HandleTmaWrite();
  void HandleTacWrite();

  bool mEnabled{};

  constexpr static uint8_t TIMA_OVERFLOW = 0;

  uint8_t mTima{};
  uint8_t mTma{};
  uint8_t mTac{};

  uint16_t mMask{DIV_BIT_MASK_LUT[GetTimerClockSelect(TimerClockSelect::HZ4096)]};
  uint16_t mClk{};

  uint8_t& mDataBus;
  uint16_t& mAddrBus;

  //
  // Handle timer esoteric behaviour
  //
  // 1. When TIMA overflows from incrementing, TIMA is reloaded on the following M cycle.
  //   -set mReloadCycle = RELOAD_START_CYCLE on overflow
  //   -if mReloadCycle == RELOAD_CYCLE, TIMA is reloaded
  // 2. On cycle 1, if TMA is written to, TIMA is also set to the same value.
  //   -if mReloadCycle == RELOAD_CYCLE on TMA write, TIMA=val
  // 3. On cycle 1, if TIMA is written to, the write is ignored.
  //   -if mReloadCycle != RELOAD_CYCLE on TIMA write, TIMA = val
  // 4. If TIMA is written to during the overflow cycle, the overflow is ignored
  //   -set mReloadCycle = RELOAD_END_CYCLE on TIMA write so reload stops
  //
  constexpr static uint8_t  RELOAD_START_CYCLE = 0;
  constexpr static uint8_t  RELOAD_CYCLE = 1;
  constexpr static uint8_t  RELOAD_END_CYCLE = 2;

  uint8_t mReloadCycle{RELOAD_END_CYCLE};
};
