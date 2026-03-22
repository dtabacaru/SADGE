#include "TimerController.h"

#include "Constants.h"
#include "Utils.h"

TimerController::TimerController(InterruptReceiver& intRec,
                                 uint8_t& dataBus,
                                 uint16_t& addrBus) : 
  InterruptProvider(intRec, InterruptBitMask::TIMER),
  mDataBus(dataBus),
  mAddrBus(addrBus)
{
}

void TimerController::Write()
{
  Address addr = static_cast<Address>(mAddrBus);

  switch (addr)
  {
    case Address::DIV:
      TickSysClk(0);
      break;
    case Address::TIMA:
      HandleTimaWrite();
      break;
    case Address::TMA:
      HandleTmaWrite();
      break;
    case Address::TAC:
      HandleTacWrite();
      break;
  }
}

void TimerController::Read() const
{
  Address addr = static_cast<Address>(mAddrBus);

  switch (addr)
  {
    case Address::DIV:
      mDataBus = mClk >> 6;
      break;
    case Address::TIMA:
      mDataBus = mTima;
      break;
    case Address::TMA:
      mDataBus = mTma;
      break;
    case Address::TAC:
      mDataBus = mTac;
      break;
  }
}

uint16_t TimerController::GetClk() const
{
  return mClk;
}

void TimerController::Tick()
{
  TickSysClk(mClk + 1);
}

void TimerController::TickSysClk(uint16_t newClk)
{
  uint16_t oldClk = mClk;
  mClk = newClk;
  
  if (mReloadCycle != RELOAD_END_CYCLE)
    TickReload();

  if (mEnabled)
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
  if (mTima == TIMA_OVERFLOW)
  {
    mReloadCycle = RELOAD_START_CYCLE;
    mTima = 0;
  }
}

void TimerController::HandleTimaWrite()
{
  if (mReloadCycle != RELOAD_CYCLE)
    mTima = mDataBus;

  mReloadCycle = RELOAD_END_CYCLE;
}

void TimerController::HandleTmaWrite()
{
  if (mReloadCycle == RELOAD_CYCLE)
    mTima = mDataBus;

  mTma = mDataBus;
}

void TimerController::HandleTacWrite()
{
  if (mEnabled && !(mDataBus & GetTimerBitMask(TimerBitMask::ENABLE)))
  {
    if (mClk & mMask)
      TickTima();
  }

  mTac = mDataBus;
  mMask = DIV_BIT_MASK_LUT[mTac & GetTimerBitMask(TimerBitMask::CLOCK_SELECT)];
  mEnabled = mTac & GetTimerBitMask(TimerBitMask::ENABLE);
}
