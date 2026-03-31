#pragma once

#include "InterruptProvider.h"

#include <atomic>
#include <thread>

class JoypadController : public InterruptProvider
{
public:

  enum class Address : uint16_t
  {
    STATE = 0xFF00
  };

  constexpr static uint16_t GetAddress(Address addr)
  {
    return static_cast<uint16_t>(addr);
  }

  enum class JoypadButtonBitMask : uint8_t
  {
    BUTTONS_A_RIGHT    = 0b00000001, // R
    BUTTONS_B_LEFT     = 0b00000010, // R
    BUTTONS_SELECT_UP  = 0b00000100, // R
    BUTTONS_START_DOWN = 0b00001000, // R
  };

  JoypadController(InterruptReceiver& intRec,
                   uint8_t& dataBus,
                   uint16_t& addrBus);

  void Write();
  void Read() const;

  void PressActionButton(JoypadButtonBitMask button);
  void ReleaseActionButton(JoypadButtonBitMask button);
  void PressDirectionButton(JoypadButtonBitMask button);
  void ReleaseDirectionButton(JoypadButtonBitMask button);

private:
  // Top 2 bits always 1
  // 0 = set, 1 = reset for joypad
  enum class JoypadSelect : uint8_t
  {
    NONE_SELECTED = 0xF0,
    DIRECTION_SELECTED = 0xE0,
    ACTION_SELECTED = 0xD0,
    BOTH_SELECTED = 0xC0
  };

  constexpr static uint8_t NO_BUTTONS_PRESSED = 0xF;

  JoypadSelect mSelect = JoypadSelect::NONE_SELECTED;

  uint8_t GetSelect() const;

  // Leave these as atomic in case driver/ui sets these from a different thread
  std::atomic<uint8_t> mAButtons = NO_BUTTONS_PRESSED;
  std::atomic<uint8_t> mDButtons = NO_BUTTONS_PRESSED;

  uint8_t& mDataBus;
  uint16_t& mAddressBus;
};