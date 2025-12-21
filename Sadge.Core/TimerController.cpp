#include "TimerController.h"

bool TimerController::HandleWrite(uint16_t address, uint8_t val)
{
  TimerAddress timer_address = static_cast<TimerAddress>(address);

  switch (timer_address)
  {
    case TimerController::TimerAddress::DIV:
      return HandleDivWrite();
    case TimerController::TimerAddress::TIMA:
      HandleTimaWrite(val);
      break;
    case TimerController::TimerAddress::TMA:
      HandleTmaWrite(val);
      break;
    case TimerController::TimerAddress::TAC:
      HandleTacWrite(val);
      break;
    default:
      (void)val;
      break;
  }

  return false;
}

uint8_t TimerController::HandleRead(uint16_t address) const
{
  TimerAddress timer_address = static_cast<TimerAddress>(address);

  switch (timer_address)
  {
    case TimerController::TimerAddress::DIV:
      return m_sys_clk >> 8;
    case TimerController::TimerAddress::TIMA:
      return m_tima;
    case TimerController::TimerAddress::TMA:
      return m_tma;
    case TimerController::TimerAddress::TAC:
      return m_tac;
    default:
      return DEFAULT_READ;
  }
}

constexpr uint16_t TimerController::GetDivBitMask()
{
  TimerClockSelect clock_select = static_cast<TimerClockSelect>(m_tac & GetBitMask(TimerBitMask::CLOCK_SELECT_01));

  switch (clock_select)
  {
    case TimerClockSelect::HZ4096:
      return 1 << 9;
    case TimerClockSelect::HZ262144:
      return 1 << 3;
    case TimerClockSelect::HZ65536:
      return 1 << 5;
    case TimerClockSelect::HZ16384:
      return 1 << 7;
    default:
      return 1 << 0;
  }
}
