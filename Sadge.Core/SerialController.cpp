#include "SerialController.h"

#include "Constants.h"

SerialController::SerialController(InterruptReceiver& intRec,
																	 uint8_t& dataBus,
																	 uint16_t& addrBus) :
	InterruptProvider(intRec, InterruptBitMask::SERIAL),
	mDataBus(dataBus),
	mAddressBus(addrBus)
{
}

void SerialController::Read() const
{
	Address addr = static_cast<Address>(mAddressBus);

	switch (addr)
	{
		case Address::DATA:
			mDataBus = mSB;
			break;
		case Address::CONTROL:
			mDataBus = mSC;
			break;
		default:
			mDataBus = DEFAULT_READ;
			break;
	}
}

void SerialController::Write()
{
	Address addr = static_cast<Address>(mAddressBus);

	switch (addr)
	{
		case Address::DATA:
			mSB = mDataBus;
			break;
		case Address::CONTROL:
			mSC = mDataBus;
			break;
		default:
			break;
	}
}