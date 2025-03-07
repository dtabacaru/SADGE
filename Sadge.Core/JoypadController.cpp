#include "JoypadController.h"


#include "raylib.h"

void JoypadController::CheckButtons()
{
	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT))
	{
		PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_MIDDLE_LEFT))
	{
		ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
	{
		PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
	{
		ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
	{
		PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
	{
		ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > JOYSTICK_THRESH) && !m_joystick_down)
	{
		m_joystick_down = true;
		PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < JOYSTICK_THRESH) && m_joystick_down)
	{
		m_joystick_down = false;
		ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -JOYSTICK_THRESH) && !m_joystick_up)
	{
		m_joystick_up = true;
		PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > -JOYSTICK_THRESH) && m_joystick_up)
	{
		m_joystick_up = false;
		ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > JOYSTICK_THRESH) && !m_joystick_right)
	{
		m_joystick_right = true;
		PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < JOYSTICK_THRESH) && m_joystick_right)
	{
		m_joystick_right = false;
		ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -JOYSTICK_THRESH) && !m_joystick_left)
	{
		m_joystick_left = true;
		PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > -JOYSTICK_THRESH) && m_joystick_left)
	{
		m_joystick_left = false;
		ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
	{
		PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
	{
		ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
	{
		PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
	{
		ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
	{
		PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
	{
		ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
	{
		PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
	{
		ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
}

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
