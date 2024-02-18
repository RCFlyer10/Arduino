#ifndef LocoNetHelper_h
#define LocoNetHelper_h

#include <Arduino.h>
#include <LocoNet.h>


class LocoNetHelperClass {
	public:
		LocoNetHelperClass();
		uint8_t processStationaryDecoderInterrogate(lnMsg* LnPacket);	
		uint8_t processProgramTaskMessage(lnMsg* LnPacket);
		LN_STATUS programTaskResponse(uint16_t Address, uint16_t CV, uint8_t Value);
};

	extern LocoNetHelperClass LocoNetHelper;	

// Stationary Decoder Interrogate Call-back
	extern void notifyStationaryDecoderInterrogate(uint8_t AddressRange) __attribute__((weak));
	
// Programmer Task Call-back functions
	extern void notifyOpsModeByteRead(uint16_t Address, uint16_t CV) __attribute__((weak));
	extern void notifyOpsModeByteWrite(uint16_t Address, uint16_t CV, uint8_t Value) __attribute__((weak));
	extern void notifySvsModeByteWrite(uint16_t CV, uint8_t Value) __attribute__((weak));
#endif