
#ifndef Function_Led_V2_h
#define Function_Led_V2_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#define On 1
#define Off 0

// Modes
#define NORMAL 0
#define DIMMABLE 1
#define FADE 2
#define STROBE 3
#define RANDOM 4
#define BEACON 5
#define MARS 6

// Fade mode config
#define STEP 2

// Strobe mode config
#define PERIOD 1128UL
#define DURATION 50

// Random mode config
#define TIME 10000
#define PROBABILITY 50

// Beacon mode congig
#define START_ANGLE PI * 1.5
#define MAX_ANGLE PI * 3.5

/*!
 *  @brief  Class that stores state and functions for the Funtion Led
 */
class Function_Led {
	public:
		// Constructor
		Function_Led(uint8_t pin);
		// Methods
		void setConfig(uint8_t config);		
		void setState(bool state);		
		void setBrightness(uint8_t brightness);
		void setDimValue(uint8_t value);
		void setFadeRate(uint8_t fadeRate);
		void setFlashRate(uint8_t flashRate);
		void setProbability(uint8_t probability);
		void setDimmerOn(bool on);		
		void heartbeat();				
	
	private:
		// Instance variables
		unsigned long _previousMillis;
		uint8_t _randomNumber;		
		uint8_t _pin;		
		uint8_t _config;
		uint8_t _dimValue;
		uint8_t _fadeRate;
		uint8_t _flashRate;		
		uint8_t _brightness;
		uint8_t _probability;
		uint8_t _beaconDelaySP;
		bool _dimmerOn;
		bool _fading;
		bool _fadeDir;
		bool _state;		
		bool _ledState;
		int _fade;
		float _angle;
		float _value;
		float _step;
};
#endif