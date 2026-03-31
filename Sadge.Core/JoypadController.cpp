#include "JoypadController.h"

JoypadController::JoypadController(InterruptReceiver& intRec,
                                   uint8_t& dataBus,
                                   uint16_t& addrBus) :
  InterruptProvider(intRec, InterruptBitMask::JOYPAD),
  mDataBus(dataBus),
  mAddressBus(addrBus)
{
}

void JoypadController::Write()
{
  mSelect = static_cast<JoypadSelect>(0xC0 | (mDataBus & 0x30));
}

void JoypadController::Read() const
{
  switch (mSelect)
  {
    case JoypadSelect::DIRECTION_SELECTED:
      mDataBus = GetSelect() | mDButtons;
      break;
    case JoypadSelect::ACTION_SELECTED:
      mDataBus = GetSelect() | mAButtons;
      break;
    case JoypadSelect::BOTH_SELECTED:
      mDataBus = GetSelect() | (mAButtons & mDButtons);
      break;
    case JoypadSelect::NONE_SELECTED:
      // Fall through
    default:
      mDataBus = GetSelect() | NO_BUTTONS_PRESSED;
      break;
  }
}

void JoypadController::PressActionButton(JoypadButtonBitMask button)
{
  if (mAButtons == NO_BUTTONS_PRESSED)
    TriggerInterrupt();

  mAButtons &= ~static_cast<uint8_t>(button);
}

void JoypadController::ReleaseActionButton(JoypadButtonBitMask button)
{
  mAButtons |= static_cast<uint8_t>(button);
}

void JoypadController::PressDirectionButton(JoypadButtonBitMask button)
{
  if (mDButtons == NO_BUTTONS_PRESSED)
    TriggerInterrupt();

  mDButtons &= ~static_cast<uint8_t>(button);
}

void JoypadController::ReleaseDirectionButton(JoypadButtonBitMask button)
{
  mDButtons |= static_cast<uint8_t>(button);
}

uint8_t JoypadController::GetSelect() const
{
  return static_cast<uint8_t>(mSelect);
}
