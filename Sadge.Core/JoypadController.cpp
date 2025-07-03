#include "JoypadController.h"

uint8_t JoypadController::ReadJoypad() const
{
  switch (m_select)
  {
    case JoypadSelect::DIRECTION_SELECTED:
			return GetSelect() | m_direction_buttons;
    case JoypadSelect::ACTION_SELECTED:
			return GetSelect() | m_action_buttons;
    case JoypadSelect::BOTH_SELECTED:
			return GetSelect() | (m_action_buttons & m_direction_buttons);
    case JoypadSelect::NONE_SELECTED:
      // Fall through
    default:
			return GetSelect() | NO_BUTTONS_PRESSED;
  }
}
