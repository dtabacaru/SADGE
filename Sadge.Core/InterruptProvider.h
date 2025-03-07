#pragma once

#include "InterruptReceiver.h"

class InterruptProvider
{
public:
  InterruptProvider(InterruptReceiver& receiver, InterruptBitMask default_bit_mask)
    : m_receiver(receiver), m_default_bit_mask(default_bit_mask) {}

  ~InterruptProvider() {}

  inline void TriggerInterrupt()
  {
    m_receiver.ReceiveInterrupt(m_default_bit_mask);
  }

  inline void TriggerInterrupt(InterruptBitMask bit_mask)
  {
    m_receiver.ReceiveInterrupt(bit_mask);
  }

private:
  InterruptReceiver& m_receiver;
  InterruptBitMask   m_default_bit_mask;
};
