#pragma once

#include "InterruptReceiver.h"

#include <atomic>
#include <stdint.h>

class InterruptController : public InterruptReceiver
{
public:
  constexpr static uint8_t DEFAULT_READ = 0xFF;

  enum class InterruptAddress : uint16_t
  {
    FLAG   = 0xFF0F,
    ENABLE = 0xFFFF
  };

  inline constexpr uint8_t GetBitMask(InterruptBitMask mask) const
  {
    return static_cast<uint8_t>(mask);
  }

  inline constexpr uint16_t GetInterruptVector(InterruptBitMask mask) const
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

  InterruptController() {};
  ~InterruptController() {};

  inline bool InterruptRequested() const
  {
    return m_master_enable_flag && InterruptExists();
  }

  inline bool InterruptExists() const
  {
    return m_flag & m_enable;
  }

  inline void EnableInterrupts()
  {
    m_master_enable_flag = true;
  }

  inline void DisableInterrupts()
  {
    m_master_enable_flag = false;
  }

  inline void Update(bool interrupt_enable_request)
  {
    if (interrupt_enable_request)
    {
      m_enable_next_instruction = true;
    }
    else if(m_enable_next_instruction) // interrupt enable requested last instruction, enable it now
    {
      m_master_enable_flag = true;
      m_enable_next_instruction = false;
    }
  }

  inline void SetState(bool interrupt_master_enable_flag, uint8_t interrupt_enable)
  {
    m_master_enable_flag = interrupt_master_enable_flag;
    m_enable = interrupt_enable;
  }

  inline bool CheckState(bool interrupt_master_enable_flag) const
  {
    if (interrupt_master_enable_flag != m_master_enable_flag) return false;
    else return true;
  }

  uint8_t HandleRead(uint16_t address) const;
  void HandleWrite(uint16_t address, uint8_t val);

  uint16_t HandleInterrupt();

private:

  inline bool InterruptExists(InterruptBitMask bit_mask) const
  {
    return (m_flag & m_enable) & GetBitMask(bit_mask);
  }

  inline void ReceiveInterrupt(InterruptBitMask bit_mask)
  {
    m_flag |= GetBitMask(bit_mask);
  }

  inline void ClearInterrupt(InterruptBitMask bit_mask)
  {
    m_flag &= ~GetBitMask(bit_mask);
  }

  bool m_master_enable_flag = false;
  bool m_enable_next_instruction = false;
  // The interrupt flag may be triggered by keypresses which are registered to the
  // window thread and not the main thread - make the flag atomic.
  std::atomic<uint8_t> m_flag{};
  uint8_t m_enable{};
};