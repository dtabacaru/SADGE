#include "TimerController.h"

#include "Constants.h"
#include "Utils.h"

TimerController::TimerController(InterruptReceiver& interrupt_receiver) : InterruptProvider(interrupt_receiver, InterruptBitMask::TIMER) 
{
}

void TimerController::HandleWrite(uint16_t address, uint8_t val)
{
  TimerAddress timer_address = static_cast<TimerAddress>(address);

  switch (timer_address)
  {
    case TimerAddress::DIV:
      HandleDivWrite();
      break;
    case TimerAddress::TIMA:
      HandleTimaWrite(val);
      break;
    case TimerAddress::TMA:
      HandleTmaWrite(val);
      break;
    case TimerAddress::TAC:
      HandleTacWrite(val);
      break;
  }
}

uint8_t TimerController::HandleRead(uint16_t address) const
{
  TimerAddress timer_address = static_cast<TimerAddress>(address);

  switch (timer_address)
  {
    case TimerAddress::DIV:
      return mClk >> 6;
    case TimerAddress::TIMA:
      return mTima;
    case TimerAddress::TMA:
      return mTma;
    case TimerAddress::TAC:
      return mTac;
    default:
      return DEFAULT_READ;
  }
}

uint16_t TimerController::GetClk() const
{
  return mClk;
}

void TimerController::Tick()
{
  return TickSysClk(mClk + 1);
}

constexpr uint8_t TimerController::GetTimerBitMask(TimerBitMask mask) const
{
  return static_cast<uint8_t>(mask);
}

constexpr uint8_t TimerController::GetTimerClockSelect(TimerClockSelect cs) const
{
  return static_cast<uint8_t>(cs);
}

void TimerController::TickSysClk(uint16_t newClk)
{
  uint16_t oldClk = mClk;
  mClk = newClk;
  
  if (mReloadCycle != RELOAD_END_CYCLE)
    TickReload();

  if (mTimerEnabled)
  {
    if (Utils::FallingEdgeDetect(oldClk, mClk, mMask))
      TickTima();
  }
}

void TimerController::TickReload()
{
  mReloadCycle += 1;
  if (mReloadCycle == RELOAD_CYCLE)
  {
    TriggerInterrupt();
    mTima = mTma;
  }
}

void TimerController::TickTima()
{
  mTima += 1;
  if (mTima == 0)
  {
    mReloadCycle = RELOAD_START_CYCLE;
    mTima = 0;
  }
}

void TimerController::HandleDivWrite()
{
  TickSysClk(0);
}

void TimerController::HandleTimaWrite(uint8_t val)
{
  if (mReloadCycle != RELOAD_CYCLE)
    mTima = val;

  mReloadCycle = RELOAD_END_CYCLE;
}

void TimerController::HandleTmaWrite(uint8_t val)
{
  if (mReloadCycle == RELOAD_CYCLE)
    mTima = val;

  mTma = val;
}

void TimerController::HandleTacWrite(uint8_t val)
{
  if (mTimerEnabled && !(val & GetTimerBitMask(TimerBitMask::ENABLE)))
  {
    if (mClk & mMask)
      TickTima();
  }

  mTac = val;
  mMask = DIV_BIT_MASK_LUT[mTac & GetTimerBitMask(TimerBitMask::CLOCK_SELECT)];
  mTimerEnabled = mTac & GetTimerBitMask(TimerBitMask::ENABLE);
}
