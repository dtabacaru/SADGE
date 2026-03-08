#include "TimerController.h"

TimerController::TimerController(InterruptReceiver& interrupt_receiver) : InterruptProvider(interrupt_receiver, InterruptBitMask::TIMER) 
{
}

bool TimerController::HandleWrite(uint16_t address, uint8_t val)
{
  TimerAddress timer_address = GetTimerAddress(address);

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
  TimerAddress timer_address = GetTimerAddress(address);

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

bool TimerController::UpdateSysClock(uint16_t sys_clk)
{
  uint16_t last_cycle_count = m_sys_clk;
  m_sys_clk = sys_clk;
  UpdateTima(last_cycle_count);
  return FallingEdgeDetect(last_cycle_count, DIV_APU_BIT_MASK);
}

bool TimerController::Update()
{
  return UpdateSysClock(m_sys_clk + 4);
}

bool TimerController::FallingEdgeDetect(uint16_t last_cycle_count, uint16_t div_bit_mask)
{
  return (last_cycle_count & div_bit_mask) && !(m_sys_clk & div_bit_mask);
}

void TimerController::TickTima()
{
  m_tima += 1;
  if (m_tima == 0)
    TimaOverflow();
}

bool TimerController::HandleDivWrite()
{
  return UpdateSysClock(0);
}

void TimerController::HandleTimaWrite(uint8_t val)
{
  if (m_tima_reload_cycle != 4)
    m_tima = val;

  m_tima_reload_cycle = 8;
}

void TimerController::HandleTmaWrite(uint8_t val)
{
  if (m_tima_reload_cycle == 4)
    m_tima = val;

  m_tma = val;
}

void TimerController::HandleTacWrite(uint8_t val)
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

void TimerController::TimaOverflow()
{
  m_tima_reload_cycle = 0;
  m_tima = 0;
}

void TimerController::ReloadTima()
{
  TriggerInterrupt();
  m_tima = m_tma;
}

void TimerController::UpdateTima(uint16_t last_cycle_count)
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
