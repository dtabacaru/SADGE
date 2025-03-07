#include "SerialController.h"

#include <iostream>

SerialController::SerialController(InterruptReceiver& interrupt_receiver) : InterruptProvider(interrupt_receiver, InterruptBitMask::SERIAL)
{
}

SerialController::~SerialController()
{
}

uint8_t SerialController::HandleRead(uint16_t address) const
{
	Address serial_address = static_cast<Address>(address);

	switch (serial_address)
	{
		case SerialController::Address::DATA:
			return m_data;
		case SerialController::Address::CONTROL:
			return m_control;
		default:
			return DEFAULT_READ;
	}
}

void SerialController::HandleWrite(uint16_t address, uint8_t val)
{
	Address serial_address = static_cast<Address>(address);

	switch (serial_address)
	{
		case SerialController::Address::DATA:
			m_data = val;
			break;
		case SerialController::Address::CONTROL:
			m_control = val;
			break;
		default:
			(void)val;
			break;
	}
}