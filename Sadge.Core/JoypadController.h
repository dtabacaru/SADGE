#pragma once

#include "InterruptProvider.h"

#include <atomic>
#include <thread>

class JoypadController : public InterruptProvider
{
public:

  constexpr static float JOYSTICK_THRESH = 0.55f;

  enum class JoypadAddress : uint16_t
  {
    STATE = 0xFF00
  };

  enum class JoypadButtonBitMask : uint8_t
  {
    BUTTONS_A_RIGHT    = 0b00000001, // R
    BUTTONS_B_LEFT     = 0b00000010, // R
    BUTTONS_SELECT_UP  = 0b00000100, // R
    BUTTONS_START_DOWN = 0b00001000, // R
  };

  // Top 2 bits always set
  // 0 = set, 1 = reset for joypad
  enum class JoypadSelect : uint8_t
  {
    NONE_SELECTED      = 0xF0,
    DIRECTION_SELECTED = 0xE0,
    ACTION_SELECTED    = 0xD0,
    BOTH_SELECTED      = 0xC0
  };
  
  JoypadController(InterruptReceiver& interrupt_receiver) : InterruptProvider(interrupt_receiver, InterruptBitMask::JOYPAD) {}
  ~JoypadController() {}

  inline void WriteSelect(uint8_t select)
  {
    m_select = static_cast<JoypadSelect>(0xC0 | (select & 0x30));
  }

  uint8_t ReadJoypad() const;
  void CheckButtons();

private:
  constexpr static uint8_t NO_BUTTONS_PRESSED = 0xF;

  inline uint8_t GetSelect() const
  {
    return static_cast<uint8_t>(m_select);
  }

  inline void PressActionButton(JoypadButtonBitMask button)
  {
    m_action_buttons &= ~static_cast<uint8_t>(button);
    TriggerInterrupt();
  }

  inline void ReleaseActionButton(JoypadButtonBitMask button)
  {
    m_action_buttons |= static_cast<uint8_t>(button);
    TriggerInterrupt();
  }

  inline void PressDirectionButton(JoypadButtonBitMask button)
  {
    m_direction_buttons &= ~static_cast<uint8_t>(button);
    TriggerInterrupt();
  }

  inline void ReleaseDirectionButton(JoypadButtonBitMask button)
  {
    m_direction_buttons |= static_cast<uint8_t>(button);
    TriggerInterrupt();
  }

  JoypadSelect m_select = JoypadSelect::NONE_SELECTED;

  bool m_joystick_down  = false;
  bool m_joystick_up    = false;
  bool m_joystick_left  = false;
  bool m_joystick_right = false;

  // Although CheckButtons() is run in the main thread, technically, button presses are 
  // running on the window thread - make these atomic.
  std::atomic<uint8_t> m_action_buttons    = NO_BUTTONS_PRESSED;
  std::atomic<uint8_t> m_direction_buttons = NO_BUTTONS_PRESSED;
};