#ifndef LocoPWMOutput_h
#define LocoPWMOutput_h

#include <Arduino.h>
#include "Flags.h"
#include <Adafruit_PWMServoDriver.h>

#define On 1
#define Off 0

enum {
	NONE = 0,
	NORMAL = 1,
	FADE = 2,
	FLICKER = 3,	
	TRAFFIC_SIGNAL_GREEN = 4,
	TRAFFIC_SIGNAL_YELLOW = 5,
	TRAFFIC_SIGNAL_RED = 6,
	TRAFFIC_SIGNAL_BRAKE = 7,
	WELDER_GLOW = 8,
	WELDER_ARC = 9,		
	STROBE = 10,
	BEACON = 11,
} EFFECT;

// Status flags
#define OUTPUT_ON 0
#define LED_ON 1
#define RESOLUTION_HIGH 2
#define ARC_ON 3
#define BRAKE_ON 4
#define FADING 5
#define INITIALIZED 6

// Strobe effect config
#define PERIOD 1256UL


// Beacon effect config
#define START_ANGLE PI * 1.5
#define MAX_ANGLE PI * 3.5

//#define DebugLocoPWMOutput

class OutputGroup;

typedef enum {
	GREEN,
	YELLOW,
	RED
} Traffic_Signal_Color;

extern const int OUTPUTGROUPS;	
extern Adafruit_PWMServoDriver ledDriver;
extern bool debug;
extern OutputGroup outputGroups[4];


/*!
 *  @brief  Class that stores state and functions for the LocoPWMOutput
 */
class LocoPWMOutput {
	public:
		// Constructor
		LocoPWMOutput();
		LocoPWMOutput(uint8_t pin, bool resolution);
		
		// Methods		
		uint16_t getAddress();
		void setAddressLSB(uint8_t value);
		void setAddressMSB(uint8_t value);		
		void setEffect(uint8_t effect);
		void setConfig_1(uint8_t value);
		void setConfig_2(uint8_t value);
		void setConfig_3(uint8_t value);
		void setOutputGroup(uint8_t group);
		void setInitialized(void);
		uint8_t getEffect();
		void setOn(bool on);		
		bool getOn();
		uint8_t getOutputNumber();
		void heartbeat();
		
	private:
		void setValues();
		void setOutput(uint8_t value);
		
	protected:
		// Instance variables
		Flags flags = Flags();
		uint8_t _outputNumber = 0;
		uint8_t _effect = 0;
		uint8_t _pin = 0;
		uint8_t _config_1 = 0;
		uint8_t _config_2 = 0;
		uint8_t _config_3 = 0;		
		uint8_t _outputGroup = OUTPUTGROUPS;
		uint8_t _timeFlickerOn = 0;		
		uint8_t _timeFlickerOff = 0;		
		uint16_t _timeArcOff = 0;
		uint16_t _timeArcOn = 0;		 
		uint16_t _address = 0;				
		float _step = 0;
		float _fade = 0;		
		unsigned long _previousMillis = 0;
		unsigned long _previousMillisFlicker = 0;
};
#endif