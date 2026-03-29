#pragma once

#include "InterruptReceiver.h"

#include <atomic>

class InterruptController : public InterruptReceiver
{
public:

  enum class InterruptAddress : uint16_t
  {
    FLAG   = 0xFF0F,
    ENABLE = 0xFFFF
  };

  constexpr static uint16_t GetAddress(InterruptAddress addr)
  {
    return static_cast<uint16_t>(addr);
  }

  InterruptController(uint8_t& dataBus,
                      uint16_t& addrBus);

  uint16_t GetInterruptVector(InterruptBitMask mask) const;

  bool InterruptRequested() const;
  bool InterruptExists() const;

  void EnableInterrupts();
  void DisableInterrupts();

  void Update(bool ieReq);

  void Read() const;
  void Write();

  uint16_t HandleInterrupt();

  private:

  bool InterruptExists(InterruptBitMask mask) const;
  void ReceiveInterrupt(InterruptBitMask mask);
  void ClearInterrupt(InterruptBitMask mask);

  // Leave this as atomic in case driver/ui set this from a different thread
  std::atomic<uint8_t> mIF{};
  uint8_t mIE{};

  bool mIME = false;
  bool mEnableNext = false;

  uint8_t& mDataBus;
  uint16_t& mAddressBus;
};