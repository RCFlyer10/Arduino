#include "LocoPWMOutput.h"
#include "OutputGroup.h"


Traffic_Signal_Color trafficSignalColor = GREEN;
uint8_t outputNumber = 0;

LocoPWMOutput::LocoPWMOutput() {	
}

LocoPWMOutput::LocoPWMOutput(uint8_t pin, bool resolution) {
	_pin = pin;
	_outputNumber = outputNumber++;
	

	if (resolution) {
		flags.setFlag(RESOLUTION_HIGH);
	}
	else {
		flags.clearFlag(RESOLUTION_HIGH);		
	}
	
	setOutput(Off);
}

void LocoPWMOutput::setAddressLSB(uint8_t value) {
	uint16_t temp = _address & 0xff00;
	temp |= (value + 1);
	_address = temp;	
}

void LocoPWMOutput::setAddressMSB(uint8_t value) {
	uint16_t temp = _address & 0x00ff;
	temp |= (value << 8);
	_address = temp;	
}

void LocoPWMOutput::setConfig_1(uint8_t value) {
	_config_1 = value;
	setValues();
}

void LocoPWMOutput::setConfig_2(uint8_t value) {
	_config_2 = value;
	setValues();
 }
 
 void LocoPWMOutput::setConfig_3(uint8_t value) {
	_config_3 = value;
	setValues();
 }

void LocoPWMOutput::setEffect(uint8_t effect) {
	_effect = effect;
	if (effect == NONE) {
		setOutput(Off);
	}
	setValues();
}

void LocoPWMOutput::setOutputGroup(uint8_t group) {	
	
	if (_outputGroup < OUTPUTGROUPS) {		
		outputGroups[_outputGroup].removeOutput(this);
	}
	if (group < OUTPUTGROUPS)  {		
		outputGroups[group].addOutput(this);
	}
	_outputGroup = group;
	setValues();
}

void LocoPWMOutput::setOn(bool on) {

	if (on) {
		flags.setFlag(OUTPUT_ON);		
		_timeArcOff = 3000;
		_timeArcOn = 3000;		
		_timeFlickerOn = 50;
		_timeFlickerOff = 170;
		_previousMillis = 0;
		_previousMillisFlicker = 0;
		_fade = 0;
	}
	else {
		flags.clearFlag(OUTPUT_ON);			
	}
}

void LocoPWMOutput::setValues() {	

	if (getEffect() == FADE) {
		_step = (_config_1 - _config_3) * .01;		
	}	
	else if (getEffect() == BEACON) {
		float freq = (_config_2 / 500.0) + 0.74;	
		_step = (TWO_PI / 125) * freq;		
	}	
	
	trafficSignalColor = GREEN;	
	_previousMillis = 0;
	_previousMillisFlicker = 0;	
}

uint16_t LocoPWMOutput::getAddress() {
	return _address;
}

uint8_t LocoPWMOutput::getEffect() {
	return _effect;
}

bool LocoPWMOutput::getOn() {
	return flags.getSet(OUTPUT_ON);
}

uint8_t LocoPWMOutput::getOutputNumber() {
	return _outputNumber;
}

void LocoPWMOutput::setOutput(uint8_t value) {
	if (flags.getSet(RESOLUTION_HIGH)) {

		int temp = map(value, 0, 255, 0, 4095);
		ledDriver.setPin(_pin, temp, true);
	}
	else {
		analogWrite(_pin, 255 - value);
	}
}

void LocoPWMOutput::setInitialized() {
	flags.setFlag(INITIALIZED);
}

void LocoPWMOutput::heartbeat() {
	unsigned long currentMillis = millis();	
	
	switch(getEffect()) {		
		case NORMAL : {			
			if (flags.getSet(OUTPUT_ON)) {

				if (flags.getNotSet(LED_ON)) {
					setOutput(_config_1);					
					flags.setFlag(LED_ON);
				}	
			}
			else if (flags.getSet(LED_ON)) {				
				setOutput(_config_3);				
				flags.clearFlag(LED_ON);
			}
			break;
		}
		
		case FADE : {
			if (currentMillis - _previousMillis >= _config_2 * 10) { 	
				_previousMillis = currentMillis;

				if (flags.getSet(OUTPUT_ON)) {
					if (_fade < _config_1) {
							_fade += _step;    				// increase fade by step											
					}
					if (_fade > _config_1) {
					            							// keep fade in bounds
						_fade = _config_1;
					}
					flags.setFlag(FADING);									
				}		
				else {
					if (_fade > _config_3) {
						_fade -= _step;    					// decrease fade by step
					}										
					if (_fade < _config_3) {
						_fade = _config_3;					// keep fade in bounds				
					}
					flags.setFlag(FADING);					
				}
				if (flags.getSet(FADING)) {
					setOutput((uint8_t)_fade);
					flags.clearFlag(FADING);
				}
			}					
			break;
		}
		
		case FLICKER : {
			if (flags.getSet(OUTPUT_ON)) {

				if(currentMillis - _previousMillis >= random(_config_2)) {			
					_previousMillis = currentMillis;
					flags.setFlag(LED_ON);
					uint8_t temp = random(_config_3, _config_1);
					setOutput(temp);														
				}				
			}
			else if (flags.getSet(LED_ON)) {
				setOutput(Off);				
				flags.clearFlag(LED_ON);		
			}
			break;			
		}
		
		case STROBE : {
			if (flags.getSet(OUTPUT_ON)) {    
				if (currentMillis - _previousMillis >= PERIOD - (_config_2 * 2)) {
					if (flags.getNotSet(LED_ON)) {
						_previousMillis = currentMillis;
						flags.setFlag(LED_ON);
					}
				}					
				else if (currentMillis - _previousMillis >= _config_3) {
					if (flags.getSet(LED_ON)) {
						flags.clearFlag(LED_ON);
					}						
				}											
				if (flags.getSet(LED_ON)) {					
					setOutput(_config_1);
				}				
				else {
					setOutput(Off);					
				}
			}
			else if (flags.getSet(LED_ON)) {				
				setOutput(Off);				
				flags.clearFlag(LED_ON);
			}
			break;
		}
		
		case BEACON : {
			if (flags.getSet(LED_ON)) {				
				if (currentMillis - _previousMillis >= 8U) {
					_previousMillis = currentMillis;
					_fade += _step;                                				// increase angle by step
				}
				if (_fade >= MAX_ANGLE) {                                   			// keep angle in bounds
					_fade = START_ANGLE;
				}
				uint8_t top = _config_1 >> 2;
				float value = sin(_fade) * top + top;         		  							// calculate fade, will follow sine wave
				if (flags.getSet(RESOLUTION_HIGH)) {
					setOutput(value);					
				}
				else {
					analogWrite(_pin, 255 - value);                   			// setFlag beacon to fade value
				}
				flags.setFlag(LED_ON);
			}
			else if (flags.getSet(LED_ON)) {
				if (flags.getSet(RESOLUTION_HIGH)) {
					ledDriver.setPin(_pin, 0, true);
				}
				else {
					 analogWrite(_pin, 255);
				}
				flags.clearFlag(LED_ON);
			}
			break;
		}
		
		case TRAFFIC_SIGNAL_GREEN : {
			if (flags.getSet(OUTPUT_ON)) {
				if (trafficSignalColor == GREEN) {
					if (flags.getNotSet(LED_ON)) {
						setOutput(_config_1);						
						flags.setFlag(LED_ON);
					}
					else if (currentMillis - _previousMillis >= _config_2 * 100) {
						setOutput(_config_3);						
						flags.clearFlag(LED_ON);
						_previousMillis = currentMillis;
						trafficSignalColor = YELLOW;
					}
				}
			}
			else if (flags.getSet(LED_ON)) {
				setOutput(_config_3);				
				flags.clearFlag(LED_ON);
			}
			break;
		}
		
		case TRAFFIC_SIGNAL_YELLOW : {
			if (flags.getSet(OUTPUT_ON)) {
				if (trafficSignalColor == YELLOW) {
					if (!(flags.getSet(LED_ON))) {
						setOutput(_config_1);						
						flags.setFlag(LED_ON);
					}
					else if (currentMillis - _previousMillis >= _config_2 * 50) {
						setOutput(_config_3);						
						flags.clearFlag(LED_ON);
						_previousMillis = currentMillis;
						trafficSignalColor = RED;
					}
				}
			}
			else if (flags.getSet(LED_ON)) {
				setOutput(_config_3);				
				flags.clearFlag(LED_ON);
			}
			break;
		}
		
		case TRAFFIC_SIGNAL_RED : {
			if (flags.getSet(OUTPUT_ON)) {
				if (trafficSignalColor == RED) {
					if (!(flags.getSet(LED_ON))) {
						setOutput(_config_1);						
						flags.setFlag(LED_ON);
					}
					else if (currentMillis - _previousMillis >= _config_2 * 100) {
						setOutput(_config_3);						
						flags.clearFlag(LED_ON);
						_previousMillis = currentMillis;
						trafficSignalColor = GREEN;
					}
				}
			}
			else if (flags.getSet(LED_ON)) {
				setOutput(_config_3);				
				flags.clearFlag(LED_ON);
			}
			break;
		}
		
		case TRAFFIC_SIGNAL_BRAKE : {
			if (flags.getSet(OUTPUT_ON)) {
				if (trafficSignalColor == YELLOW) {
					if (flags.getNotSet(LED_ON)) {
						setOutput(_config_1);						
						flags.setFlag(LED_ON);
					}
				}
				else if (trafficSignalColor == GREEN) {
					if (flags.getSet(LED_ON)) {
						if (flags.getNotSet(BRAKE_ON)) {
							_previousMillis = currentMillis;
							flags.setFlag(BRAKE_ON);
						}							
						else if (currentMillis - _previousMillis >= _config_2 * 10) {
							setOutput(_config_3);							
							flags.clearFlag(LED_ON);
							flags.clearFlag(BRAKE_ON);
						}
					}
				}
			}
			else if (flags.getSet(LED_ON)) {
				setOutput(_config_3);				
				flags.clearFlag(LED_ON);
			}
			break;
		}
		
		case WELDER_ARC : {			
			if (flags.getSet(OUTPUT_ON)) {
				if (flags.getNotSet(ARC_ON)) {
					if (currentMillis - _previousMillis >= _timeArcOff) {
						_previousMillis = currentMillis;
						flags.setFlag(ARC_ON);
						_timeArcOff = random(_config_2 * 20);
					}
				}
				else if (flags.getSet(ARC_ON)) {
					if (currentMillis - _previousMillis >= _timeArcOn) {
						_previousMillis = currentMillis;
						flags.clearFlag(ARC_ON);
						flags.clearFlag(LED_ON);
						_timeArcOn = random(_config_2 * 20);
					}
				}
				if (flags.getSet(ARC_ON)) {
					if (flags.getNotSet(LED_ON)) {
						if (currentMillis - _previousMillisFlicker >= _timeFlickerOff) {
							flags.setFlag(LED_ON);
							_previousMillisFlicker = currentMillis;
							_timeFlickerOff = random(_config_3);
						}
					}
					else if (currentMillis - _previousMillisFlicker >= _timeFlickerOn) {
						flags.clearFlag(LED_ON);
						_previousMillisFlicker = currentMillis;
						_timeFlickerOn = random(_config_3 / 5);
					}
				}
			}    			
			if (flags.getSet(LED_ON)) {
				setOutput(_config_1);				
			}
			else {
				setOutput(Off);				
			}
			break;
		}
		
		case WELDER_GLOW : {
			if (flags.getSet(OUTPUT_ON)) {
				if (currentMillis - _previousMillis >= _config_2) {
					_previousMillis = currentMillis;
					uint8_t temp = random(_config_3, _config_1);
					setOutput(temp);
					flags.setFlag(LED_ON);					
				}				
			}
			else {
				if (flags.getSet(LED_ON)) {
					setOutput(Off);					
					flags.clearFlag(LED_ON);
				}
			}
			break;
		}
		default : {				
		}
	}	
}

