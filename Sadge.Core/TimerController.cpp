#include "TimerController.h"

TimerController::TimerController(InterruptReceiver& interrupt_receiver) : InterruptProvider(interrupt_receiver, InterruptBitMask::TIMER) 
{
}

bool TimerController::HandleWrite(uint16_t address, uint8_t val)
{
  TimerAddress timer_address = GetTimerAddress(address);

  switch (timer_address)
  {
    case TimerAddress::DIV:
      return HandleDivWrite();
    case TimerAddress::TIMA:
      HandleTimaWrite(val);
      break;
    case TimerAddress::TMA:
      HandleTmaWrite(val);
      break;
    case TimerAddress::TAC:
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

bool TimerController::UpdateSysClock(uint16_t newClk)
{
  uint16_t oldClk = mClk;
  mClk = newClk;
  UpdateTima(oldClk);
  return FallingEdgeDetect(oldClk, DIV_APU_BIT_MASK);
}

bool TimerController::Update()
{
  return UpdateSysClock(mClk + 1);
}

bool TimerController::FallingEdgeDetect(uint16_t oldClk, uint16_t mask)
{
  return (oldClk & mask) && !(mClk & mask);
}

void TimerController::TickTima()
{
  mTima += 1;
  if (mTima == 0)
    TimaOverflow();
}

bool TimerController::HandleDivWrite()
{
  return UpdateSysClock(0);
}

void TimerController::HandleTimaWrite(uint8_t val)
{
  if (mReloadCycle != 1)
    mTima = val;

  mReloadCycle = 2;
}

void TimerController::HandleTmaWrite(uint8_t val)
{
  if (mReloadCycle == 1)
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

void TimerController::TimaOverflow()
{
  mReloadCycle = 0;
  mTima = 0;
}

void TimerController::ReloadTima()
{
  TriggerInterrupt();
  mTima = mTma;
}

void TimerController::UpdateTima(uint16_t oldClk)
{
  if (mReloadCycle != NO_RELOAD)
  {
    mReloadCycle += 1;

    if (mReloadCycle == 1)
      ReloadTima();
  }

  if (mTimerEnabled)
  {
    if (FallingEdgeDetect(oldClk, mMask))
      TickTima();
  }
}
