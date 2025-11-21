#pragma once

#include "InterruptProvider.h"

#include <atomic>
#include <thread>

class JoypadController : public InterruptProvider
{
public:

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

  // Top 2 bits always 1
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

  // TODO: Fix interrupts
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

private:
  constexpr static uint8_t NO_BUTTONS_PRESSED = 0xF;

  JoypadSelect m_select = JoypadSelect::NONE_SELECTED;

  inline uint8_t GetSelect() const
  {
    return static_cast<uint8_t>(m_select);
  }

  // Leave this as atomic in case driver/ui calls sets these from a different thread
  std::atomic<uint8_t> m_action_buttons    = NO_BUTTONS_PRESSED;
  std::atomic<uint8_t> m_direction_buttons = NO_BUTTONS_PRESSED;
};