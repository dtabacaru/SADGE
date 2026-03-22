#include "InterruptController.h"

#include "Constants.h"

uint16_t InterruptController::GetInterruptVector(InterruptBitMask mask) const
{
  switch (mask)
  {
    case InterruptBitMask::VBLANK:
      return 0x0040;
    case InterruptBitMask::LCD:
      return 0x0048;
    case InterruptBitMask::TIMER:
      return 0x0050;
    case InterruptBitMask::SERIAL:
      return 0x0058;
    case InterruptBitMask::JOYPAD:
      return 0x0060;
    default:
      return 0x0000;
  }
}

InterruptController::InterruptController(uint8_t& dataBus,
                                         uint16_t& addrBus) :
  mDataBus(dataBus),
  mAddressBus(addrBus)
{
};

bool InterruptController::InterruptRequested() const
{
  return mIME && InterruptExists();
}

bool InterruptController::InterruptExists() const
{
  return mIF & mIE;
}

void InterruptController::EnableInterrupts()
{
  mIME = true;
}

void InterruptController::DisableInterrupts()
{
  mIME = false;
}

void InterruptController::Update(bool interrupt_enable_request)
{
  if (interrupt_enable_request)
  {
    mEnableNext = true;
  }
  else if (mEnableNext)
  {
    mIME = true;
    mEnableNext = false;
  }
}

void InterruptController::Read() const
{
  InterruptAddress interrupt_address = static_cast<InterruptAddress>(mAddressBus);

  switch (interrupt_address)
  {
    case InterruptController::InterruptAddress::FLAG:
      mDataBus = mIF;
      break;
    case InterruptController::InterruptAddress::ENABLE:
      mDataBus = mIE;
      break;
    default:
      mDataBus = DEFAULT_READ;
      break;
  }
}

void InterruptController::Write()
{
  InterruptAddress interrupt_address = static_cast<InterruptAddress>(mAddressBus);

  switch (interrupt_address)
  {
    case InterruptController::InterruptAddress::FLAG:
      mIF = mDataBus;
      break;
    case InterruptController::InterruptAddress::ENABLE:
      mIE = mDataBus;
      break;
  }
}

uint16_t InterruptController::HandleInterrupt()
{
  mIME = false;

  if (InterruptExists(InterruptBitMask::VBLANK))
  {
    ClearInterrupt(InterruptBitMask::VBLANK);
    return GetInterruptVector(InterruptBitMask::VBLANK);
  }
  else if (InterruptExists(InterruptBitMask::LCD))
  {
    ClearInterrupt(InterruptBitMask::LCD);
    return GetInterruptVector(InterruptBitMask::LCD);
  }
  else if (InterruptExists(InterruptBitMask::TIMER))
  {
    ClearInterrupt(InterruptBitMask::TIMER);
    return GetInterruptVector(InterruptBitMask::TIMER);
  }
  else if (InterruptExists(InterruptBitMask::SERIAL))
  {
    ClearInterrupt(InterruptBitMask::SERIAL);
    return GetInterruptVector(InterruptBitMask::SERIAL);
  }
  else // if (InterruptExists(InterruptBitMask::JOYPAD))
  {
    ClearInterrupt(InterruptBitMask::JOYPAD);
    return GetInterruptVector(InterruptBitMask::JOYPAD);
  }
}

bool InterruptController::InterruptExists(InterruptBitMask bit_mask) const
{
  return (mIF & mIE) & GetInterruptBitMask(bit_mask);
}

void InterruptController::ReceiveInterrupt(InterruptBitMask bit_mask)
{
  mIF |= GetInterruptBitMask(bit_mask);
}

void InterruptController::ClearInterrupt(InterruptBitMask bit_mask)
{
  mIF &= ~GetInterruptBitMask(bit_mask);
}