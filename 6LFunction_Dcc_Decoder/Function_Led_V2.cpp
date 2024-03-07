#include "Function_Led_V2.h"

/*!
 *  @brief  Instantiates a new Function_Led using the Adafruit_PWMServoDriver
 *  @param  pin The pin the led to be driven is connected to  
 */
Function_Led::Function_Led(uint8_t pin) {	
	_pin = pin;
	_state = Off;
	_config = 0;	
	_dimmerOn = false;
	pinMode(_pin, OUTPUT);	
}

void Function_Led::setConfig(uint8_t config) {	
	_config = config;	
	setState(Off);
}

void Function_Led::setBrightness(uint8_t brightness) {
	_brightValue = brightness;
}

void Function_Led::setDimValue(uint8_t value) {
	_dimValue = value;
}

void Function_Led::setFadeRate(uint8_t fadeRate) {
	_fadeRate = fadeRate;
}

void Function_Led::setFlashRate(uint8_t flashRate) {	
	_flashRate = flashRate;
	float freq = (_flashRate / 500.0) + 0.74;	
	_step = (TWO_PI / 125) * freq;	
}

void Function_Led::setProbability(uint8_t probability) {
	_probability = probability;
}

void Function_Led::setDimmerOn(bool on) {
	_dimmerOn = on;
}

void Function_Led::setState(bool state) {
	if (_state != state) {
		_ledState = LOW;
		_fadeDir = true;
		_fading = true;
		_previousMillis = 0;
		_randomNumber = 0;				
	}
	_state = state;	
}

void Function_Led::heartbeat() {
	unsigned long currentMillis = millis();
	switch (_config) {
		case DIMMABLE : {
			if (_state == On) {
				if (_dimmerOn) {
					analogWrite(_pin, 255 - _dimValue);
				}
				else {
					analogWrite(_pin, 255 - _brightValue);
				}
			}
			else {
				analogWrite(_pin, 255);
			}
			break;
		}
		case FADE : {
			if (currentMillis - _previousMillis >= 256U - _fadeRate) {   	
				_previousMillis = currentMillis;			
				if (_state  == On) { 
					if (_fade < _brightValue) {
						_fade = _fade + STEP;    															// increase fade by step					
					}				
					if (_fade > _brightValue) {
						_fade = _brightValue;      														// keep fade in bounds					
					}
					analogWrite(_pin, 255 - _fade);				
				}		
				else if (_state == Off) {
					if (_fade > 0) {
						_fade = _fade - (STEP + 2);     											// decrease fade by step
					}				
					if (_fade < 0)	{
						_fade = 0;                        										// keep fade in bounds				
					}
					analogWrite(_pin, 255 - _fade);				
				}
			}
			break;
		}
		case STROBE : {
			if (_state == On) {    
				if (currentMillis - _previousMillis >= PERIOD - (2 * _flashRate) && _ledState == LOW) {
					_previousMillis = currentMillis;
					_ledState = HIGH;      
				}
				else if (currentMillis - _previousMillis >= DURATION && _ledState == HIGH) {
					_ledState = LOW; 
				}				
				if (_ledState ==  HIGH) {
					analogWrite(_pin, 255 - _brightValue);
				}				
				else {
					analogWrite(_pin, 255);
				}
			}
			else {
				analogWrite(_pin, 255);
			}
			break;
		}
		case RANDOM : {
			if (_state == On) {
				if (currentMillis - _previousMillis >= TIME) {   	
					_previousMillis = currentMillis;										
					if (_probability >= _randomNumber) {
						if (_ledState == HIGH) {
							analogWrite(_pin, 255);
							_ledState = LOW;						
						}
						else {
							analogWrite(_pin, 255 - _brightValue);
							_ledState = HIGH;						
						}					
					}
					_randomNumber = random(100);
				}		
			}
			else {
				analogWrite(_pin, 255);
			}
			break;
		}
		case BEACON : {
			if (_state == On) {
				uint8_t intensity = _brightValue / 2;
				if (currentMillis - _previousMillis >= 8U) {
					_previousMillis = currentMillis;
					_angle = _angle + _step;                                		// increase angle by step
				}
				if (_angle >= MAX_ANGLE) {                                   	// keep angle in bounds
					_angle = START_ANGLE;
				}      
				_value = sin(_angle) * intensity + intensity;         				// calculate fade, will follow sine wave
				analogWrite(_pin, 255 - _value);                   						// set beacon to fade value
			}
			else {
				analogWrite(_pin, 255);
			}
			break;
		}
		case MARS : {
			if (_state == On) {
				static uint8_t fade = 0;
				static uint8_t count = 0;
				uint8_t intensity = _brightValue / 12;
				if (currentMillis - _previousMillis >= 24U - (_flashRate / 16) && _fading) {
					_previousMillis = currentMillis;
					if (_fadeDir) {
						fade++;
					}
					if (fade > intensity) {
						fade = intensity;
						_fadeDir = false;
						count++;
						if (count >= 3){
							analogWrite(_pin, 255 - _brightValue);
							_fading = false;
							count = 0;
						}
					}
					if (!_fadeDir) {
						fade--;
					}
					if (fade == 0) {
						_fadeDir = true;
						count++;
					}
					if (_fading) {
						analogWrite(_pin, 255 - fade);
					}				
				}
				else if (currentMillis - _previousMillis >= DURATION) {
					analogWrite(_pin, 250);
					_fading = true;
				}
			}
			else {
				analogWrite(_pin, 255);
			}
			break;
		}
		case FLICKER: {
			if (_state == On) {

				if (currentMillis - _previousMillis >= 255 - random(_flashRate)) {
					_previousMillis = currentMillis;
					uint8_t temp = random(_dimValue, _brightValue);
					analogWrite(_pin, 255 - temp);
				}
			}
			else {
				analogWrite(_pin, 255);
			}
			break;
		}
		default : { // NORMAL
			if (_state == On) {
				analogWrite(_pin, 255 - _brightValue);
			}
			else {
				analogWrite(_pin, 255);
			}
		}			
	}
}