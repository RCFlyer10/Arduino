#include "LocoPWMOutput.h"
#include "OutputGroup.h"


LocoPWMOutput::LocoPWMOutput() {	
}

LocoPWMOutput::LocoPWMOutput(uint8_t pin, bool resolution) {
	_pin = pin;	

	if (resolution) {
		_flags.setFlag(RESOLUTION_HIGH);
	}
	else {
		_flags.clearFlag(RESOLUTION_HIGH);		
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
}

void LocoPWMOutput::setRemote(bool on)
{
	if (on)	{
		_flags.setFlag(REMOTE);
	}

	else 	{
		_flags.clearFlag(REMOTE);
	}
}

void LocoPWMOutput::setOn(bool on) {

	if (on)	{		
		_flags.setFlag(OUTPUT_ON);
		_flags.clearFlag(LED_ON);
		_fade = 0;
		_previousMillis = 0;
		setValues();
	}

	else {
		_flags.clearFlag(OUTPUT_ON);
	}
}

void LocoPWMOutput::setValues() {	

	if (getEffect() == FADE) {
		_step = (getBright() - getDim()) * .01;
	}	
	else if (getEffect() == BEACON) {
		float freq = (getPeriod() / 500.0) + 0.74;	
		_step = (TWO_PI / 125) * freq;		
	}	
	
	_previousMillis = 0;		
}

uint16_t LocoPWMOutput::getAddress() {
	return _address;
}

uint8_t LocoPWMOutput::getEffect() {
	return _effect;
}

uint8_t LocoPWMOutput::getBright() {
	return (_config_1 & 0x0F) * 17;
}

uint8_t LocoPWMOutput::getDim() {
	return ((_config_1 & 0xF0) >> 4) * 17;
}

uint8_t LocoPWMOutput::getRate() {
	return (_config_2 & 0x0F) * 17;
}

uint8_t LocoPWMOutput::getTiming() {
	return ((_config_2 & 0xF0) >> 4) * 17;
}

uint8_t LocoPWMOutput::getDurration() {
	return ((_config_3 & 0xF0) >> 4) * 17;
}

uint8_t LocoPWMOutput::getPeriod() {
	return (_config_3 & 0x0F) * 17;
}

bool LocoPWMOutput::getOn() {
	return _flags.getSet(OUTPUT_ON);
}

void LocoPWMOutput::setOutput(uint8_t value) {
	if (_flags.getSet(RESOLUTION_HIGH)) {

		int temp = map(value, 0, 255, 0, 4095);
		ledDriver.setPin(_pin, temp, true);
	}
	else {
		analogWrite(_pin, 255 - value);
	}
}

void LocoPWMOutput::heartbeat() {

	if (_flags.getNotSet(REMOTE))	{
		unsigned long currentMillis = millis();

		switch (getEffect()) {
			case NORMAL: {
				if (_flags.getSet(OUTPUT_ON)) {
					setOutput(getBright());
				}

				else {
					setOutput(getDim());
				}
				break;
			}

			case FADE: {
			if (currentMillis - _previousMillis >= getRate() * 5) {
				_previousMillis = currentMillis;

				if (_flags.getSet(OUTPUT_ON)) {
					uint8_t bright = getBright();

					if (_fade < bright)	{
						_fade += _step;    				// increase fade by step											
					}

					if (_fade > bright)	{
						
						_fade = bright;					// keep fade in bounds
					}
					_flags.setFlag(FADING);
				}

				else {
					uint8_t dim = getDim();

					if (_fade > dim) {
						_fade -= _step;    				// decrease fade by step
					}

					if (_fade < dim) {
						_fade = dim;					// keep fade in bounds				
					}
					_flags.setFlag(FADING);
				}

				if (_flags.getSet(FADING)) {
					setOutput((uint8_t)_fade);
					_flags.clearFlag(FADING);
				}
			}

			break;
		}

			case FLICKER: {
			if (_flags.getSet(OUTPUT_ON)) {

				if (currentMillis - _previousMillis >= 255 - random(getTiming())) {
					_previousMillis = currentMillis;
					uint8_t temp = random(getDim(), getBright());
					setOutput(temp);
				}
			}

			else {
				setOutput(Off);
			}

			break;
		}

			case STROBE: {
			if (_flags.getSet(OUTPUT_ON)) {
				if (currentMillis - _previousMillis >= PERIOD - (getPeriod() * 2)) {
					if (_flags.getNotSet(LED_ON)) {
						_previousMillis = currentMillis;
						_flags.setFlag(LED_ON);
					}
				}

				else if (currentMillis - _previousMillis >= getDurration())	{
					if (_flags.getSet(LED_ON)) {
						_flags.clearFlag(LED_ON);
					}
				}

				if (_flags.getSet(LED_ON)) {
					setOutput(getBright());
				}

				else {
					setOutput(Off);
				}
			}

			else if (_flags.getSet(LED_ON))	{
				setOutput(Off);
				_flags.clearFlag(LED_ON);
			}

			break;
		}

			case BEACON: {
			if (_flags.getSet(OUTPUT_ON))
			{
				if (currentMillis - _previousMillis >= 8U) {
					_previousMillis = currentMillis;
					_fade += _step;                             // increase angle by step
				}

				if (_fade >= MAX_ANGLE)	{                                   			
					_fade = START_ANGLE;						// keep angle in bounds
				}

				uint8_t top = getBright() >> 2;
				float value = sin(_fade) * top + top;			// calculate fade, will follow sine wave	
				setOutput(value);								// setFlag beacon to fade value					

			}

			else {
				setOutput(Off);
			}

			break;
		}

			default: {
			}
		}
	}
}

