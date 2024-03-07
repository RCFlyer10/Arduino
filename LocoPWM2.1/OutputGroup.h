#ifndef OutputGroup_h
#define OutputGroup_h

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <LocoNet.h>
#include "Flags.h"
#include "List.h"

enum {
    GROUP_ON,
    BRAKE_ON,
    ARC_ON,
    ARC_LED_ON
} GROUP_FLAGS;

enum {
    STANDARD = 0,
    TRAFFIC_SIGNAL = 1,
    WELDER = 2,
    RANDOM = 3,
    CHASE = 4
} TYPE;

enum {
    GREEN,
    YELLOW,
    RED
} TSC;

class OutputGroup {		
	public:
		OutputGroup();
        void addOutput(LocoPWMOutput* output);
        void removeOutput(LocoPWMOutput* output);
        void setGroupType(uint8_t type);		
        void setOn(bool on);
        //void setDurration(uint8_t value);
	    uint16_t getAddress();
	    bool getOn();
	    void setAddressLSB(uint8_t value);
	    void setAddressMSB(uint8_t value); 
        void setConfig1(uint8_t value);
        void setConfig2(uint8_t value);
        void heartbeat();

	private:
        uint8_t getProbability();
        uint8_t getDurration();
        uint8_t getRate();
        // Instance variables
        Flags _flags;
        uint8_t _config1;
        uint8_t _config2;
        uint8_t _count;
        uint8_t _groupType = STANDARD;
        uint8_t _tsc = RED;        
	    uint16_t _address = 0;
        uint8_t _timeFlickerOn = 0;
        uint8_t _timeFlickerOff = 0;
        uint16_t _timeArcOff = 0;
        uint16_t _timeArcOn = 0;
        unsigned long _previousMillis;
        unsigned long _previousMillisArc;
        unsigned long _previousMillisFlicker;
        List _outputList = List();
};
#endif