#ifndef OutputGroup_h
#define OutputGroup_h

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <LocoNet.h>
#include "Flags.h"
#include "List.h"

#define OUTPUT_ON 0
#define RANDOM 1


class OutputGroup {		
	public:
		OutputGroup();
        void addOutput(LocoPWMOutput* output);
        void removeOutput(LocoPWMOutput* output);
        void setRandom(bool random);		
        void setOn(bool on);		
	    uint16_t getAddress();
	    bool getOn();
	    void setAddressLSB(uint8_t value);
	    void setAddressMSB(uint8_t value); 
        void setProbability(uint8_t value);
        void heartbeat();

	private:
        // Instance variables
        Flags flags;		
        uint8_t _probability = 0;
	    uint16_t _address = 0;
        unsigned long _previousMillis;
        List _outputList = List();
};
#endif