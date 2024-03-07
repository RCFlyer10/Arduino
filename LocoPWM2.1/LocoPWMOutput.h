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
	STROBE = 4,
	BEACON = 5
} EFFECT;

// Status flags
#define OUTPUT_ON 0
#define LED_ON 1
#define RESOLUTION_HIGH 2
#define FADING 3
#define REMOTE 4


// Strobe effect config
#define PERIOD 1256UL


// Beacon effect config
#define START_ANGLE PI * 1.5
#define MAX_ANGLE PI * 3.5

//#define DebugLocoPWMOutput

class OutputGroup;

extern const int OUTPUTGROUPS;	
extern Adafruit_PWMServoDriver ledDriver;
extern bool debug;
extern OutputGroup outputGroups[10];


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
		void setRemote(bool on);
		uint8_t getEffect();
		void setOn(bool on);		
		bool getOn();		
		void heartbeat();
		void setOutput(uint8_t value);
		uint8_t getBright();
		uint8_t getDim();

	private:
		void setValues();		
		uint8_t getRate();
		uint8_t getDurration();
		uint8_t getPeriod();
		uint8_t getTiming();

	protected:
		// Instance variables
		Flags _flags = Flags();		
		uint8_t _effect = 0;
		uint8_t _pin = 0;
		uint8_t _config_1 = 0;
		uint8_t _config_2 = 0;
		uint8_t _config_3 = 0;		
		uint8_t _outputGroup = OUTPUTGROUPS;		 
		uint16_t _address = 0;				
		float _step = 0;
		float _fade = 0;		
		unsigned long _previousMillis = 0;		
};
#endif