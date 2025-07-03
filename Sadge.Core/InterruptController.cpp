#include "InterruptController.h"

uint8_t InterruptController::HandleRead(uint16_t address) const
{
  InterruptAddress interrupt_address = static_cast<InterruptAddress>(address);

  switch (interrupt_address)
  {
    case InterruptController::InterruptAddress::FLAG:
      return m_if;
    case InterruptController::InterruptAddress::ENABLE:
      return m_ie;
    default:
      return DEFAULT_READ;
  }
}

void InterruptController::HandleWrite(uint16_t address, uint8_t val)
{
  InterruptAddress interrupt_address = static_cast<InterruptAddress>(address);

  switch (interrupt_address)
  {
    case InterruptController::InterruptAddress::FLAG:
      m_if = val;
      break;
    case InterruptController::InterruptAddress::ENABLE:
      m_ie = val;
      break;
    default:
      (void)val;
  }
}

uint16_t InterruptController::HandleInterrupt()
{
  m_ime = false;

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
