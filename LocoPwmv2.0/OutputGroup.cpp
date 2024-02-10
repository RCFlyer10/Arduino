
#include "OutputGroup.h"
#include "LocoPWMOutput.h"

OutputGroup::OutputGroup() {
}

void OutputGroup::addOutput(LocoPWMOutput* output) {
	_outputList.Add(output);	
}

void OutputGroup::removeOutput(LocoPWMOutput* output) {	
	_outputList.Remove(output);	
}

void OutputGroup::setRandom(bool random) {
	if (random) {
		flags.setFlag(RANDOM);
	}
	else {
		flags.clearFlag(RANDOM);
	}	
}

void OutputGroup::setOn(bool on) {	
	if (_outputList.Count() > 0) {
		for (uint8_t index = 0; index < _outputList.Count(); index++) {			
			_outputList[index]->setOn(on);
			LocoNet.reportSensor(_outputList[index]->getAddress(), on);			
		}
	}
	if (on) {
		flags.setFlag(OUTPUT_ON);		
	}
	else {
		flags.clearFlag(OUTPUT_ON);
	}	
}

bool OutputGroup::getOn() {
	return flags.getSet(OUTPUT_ON);
}

uint16_t OutputGroup::getAddress() {
	return _address;
}

void OutputGroup::setAddressLSB(uint8_t value) {
	uint16_t temp = _address & 0xff00;
	temp |= (value + 1);
	_address = temp;	
}

void OutputGroup::setAddressMSB(uint8_t value) {
	uint16_t temp = _address & 0x00ff;
	temp |= (value << 8);
	_address = temp;	
}

void OutputGroup::setProbability(uint8_t value) {
	_probability = value;
}

void OutputGroup::heartbeat() {
	if (flags.getSet(OUTPUT_ON)) {
		if (flags.getSet(RANDOM)) {
			if (_outputList.Count() > 0) {				
				unsigned long currentMillis = millis();
				uint8_t randomnumber = 0; 		
				if (currentMillis - _previousMillis >= 10000) {   	
					_previousMillis = currentMillis;
					randomnumber = random(100);                 
					if (_probability >= randomnumber) {
						// randomly pick a light to toggle
						randomSeed(analogRead(A6));
						randomnumber = random(_outputList.Count());						
						LocoPWMOutput* output = _outputList[randomnumber];
						output->setOn(!output->getOn());
						LocoNet.reportSensor(output->getAddress(), output->getOn());
					}
				}
			}
		}			
	}
}