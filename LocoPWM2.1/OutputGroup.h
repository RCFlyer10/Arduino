#ifndef OutputGroup_h
#define OutputGroup_h

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <LocoNet.h>
#include "Flags.h"
#include "List.h"

#define GROUP_ON 0
#define BRAKE_ON 1
#define ARC_ON 2
#define ARC_LED_ON 3

enum {
    STANDARD = 0,
    TRAFFIC_SIGNAL = 1,
    WELDER = 2,
    RANDOM = 3
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
        void setConfig(uint8_t value);
        void heartbeat();

	private:
        uint8_t getProbability();
        uint8_t getDurration();
        // Instance variables
        Flags _flags;
        uint8_t _config;
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