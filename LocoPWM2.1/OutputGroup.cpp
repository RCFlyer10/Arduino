
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

void OutputGroup::setGroupType(uint8_t type) {
	_groupType = type;
}

void OutputGroup::setOn(bool on) {	
	if (on) {
		_flags.clearFlags();
		_tsc = RED;
		_timeArcOff = 3000;
		_timeArcOn = 3000;
		_timeFlickerOn = 50;
		_timeFlickerOff = 170;
		_previousMillis = 0;
		_previousMillisArc = 0;
		_previousMillisFlicker = 0;
		_count = 0;
		_flags.setFlag(GROUP_ON);		
	}

	else 
	{
		_flags.clearFlags();
	}

	if (_groupType == STANDARD || _groupType == RANDOM) {
		for (uint8_t index = 0; index < _outputList.Count(); index++) {
			_outputList[index]->setOn(on);
			LocoNet.reportSensor(_outputList[index]->getAddress(), on);
		}
	}
}

void OutputGroup::setConfig1(uint8_t value) {
	_config1 = value;
}

void OutputGroup::setConfig2(uint8_t value) {
	_config2 = value;
}

uint8_t OutputGroup::getDurration() {
	return (_config1 & 0x0F) * 17;
}

uint8_t	OutputGroup::getProbability() {
	return ((_config1 & 0xF0) >> 4) * 17;
}

uint8_t OutputGroup::getRate() {
	return (_config2 & 0x0F) * 17;
}

bool OutputGroup::getOn() {
	return _flags.getSet(GROUP_ON);
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

void OutputGroup::heartbeat() 
{
	if (_outputList.Count() > 0)
	{
		unsigned long currentMillis = millis();

		switch (_groupType)
		{
			case WELDER:
			{
				if (_flags.getSet(GROUP_ON))
				{
					if (_flags.getNotSet(ARC_ON))
					{
						if (currentMillis - _previousMillisArc >= _timeArcOff)
						{
							_flags.setFlag(ARC_ON);
							_previousMillisArc = currentMillis;
							_timeArcOff = random(3000);
						}
					}

					else
					{
						if (currentMillis - _previousMillisArc >= _timeArcOn)
						{
							_flags.clearFlag(ARC_ON);
							_flags.clearFlag(ARC_LED_ON);
							_previousMillisArc = currentMillis;
							_timeArcOn = random(3000);
						}

						if (_flags.getSet(ARC_LED_ON))
						{
							if (currentMillis - _previousMillisFlicker >= _timeFlickerOn)
							{
								_previousMillisFlicker = currentMillis;
								_timeFlickerOn = random(20);
								_flags.clearFlag(ARC_LED_ON);
							}
						}

						else
						{
							if (currentMillis - _previousMillisFlicker >= _timeFlickerOff)
							{
								_previousMillisFlicker = currentMillis;
								_timeFlickerOff = random(100);
								_flags.setFlag(ARC_LED_ON);
							}
						}
					}

					if (_flags.getSet(ARC_LED_ON))
					{
						_outputList[0]->setOn(On);
					}

					else
					{
						_outputList[0]->setOn(Off);
					}

					if (_outputList.Count() > 1)
					{
						if (currentMillis - _previousMillis >= 125)
						{
							uint8_t temp = random(32, 255);
							_outputList[1]->setRemote(true);
							_outputList[1]->setOutput(temp);
							_previousMillis = currentMillis;
						}
					}

				}

				if (_flags.getNotSet(GROUP_ON))
				{
					_outputList[0]->setOn(Off);

					if (_outputList.Count() > 1)
					{
						_outputList[1]->setRemote(false);
						_outputList[1]->setOn(Off);
					}
				}

				break;
			}

			case RANDOM:
			{
				if (_outputList.Count() > 1)
				{
					if (_flags.getSet(GROUP_ON))
					{
						uint8_t randomnumber = 0;
						if (currentMillis - _previousMillis >= 10000) 
						{
							_previousMillis = currentMillis;
							randomnumber = random(100);

							if (getProbability() >= randomnumber) 
							{
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

				break;
			}

			case TRAFFIC_SIGNAL:
			{				
				if (_flags.getSet(GROUP_ON))
				{					

					if (_tsc == GREEN)
					{
						if (_flags.getSet(BRAKE_ON))
						{

							if (currentMillis - _previousMillis >= 2000)					// time to turn brake lights off ?
							{
								_outputList[3]->setOn(Off);    								// brake lights off
								_flags.clearFlag(BRAKE_ON);
							}
						}

						else if (currentMillis - _previousMillis >= getDurration() * 100)	// time to turn traffic signal to yellow ?
						{
							_outputList[0]->setOn(Off);										// green traffic signal off    							

							if (_outputList.Count() > 1)
							{
								_outputList[1]->setOn(On);									// yellow traffic signal on								
							}

							_tsc = YELLOW;
							_previousMillis = currentMillis;
						}
					}

					else if (_tsc == YELLOW)
					{

						if (_flags.getNotSet(BRAKE_ON))
						{

							if (currentMillis - _previousMillis >= 1000)					// time to turn brake lights on ?
							{
								if (_outputList.Count() > 2)
								{
									_outputList[3]->setOn(On);    							// brake lights on									
									_flags.setFlag(BRAKE_ON);
								}
							}
						}

						else if (currentMillis - _previousMillis >= 3000)					// time to turn the traffic signal to red ?
						{
							if (_outputList.Count() > 1)
							{
								_outputList[1]->setOn(Off);									// yellow traffic signal off								
							}

							if (_outputList.Count() > 2)
							{
								_outputList[2]->setOn(On); 									// red traffic signal on								
							}

							_tsc = RED;
							_previousMillis = currentMillis;
						}
					}

					else if (_tsc == RED)
					{
						if (currentMillis - _previousMillis >= getDurration() * 100)		// time to turn the traffic signal to green ?
						{
							if (_outputList.Count() > 2)
							{
								_outputList[2]->setOn(Off);    								// red traffic signal off								
							}

							_outputList[0]->setOn(On);    									// green traffic signal on
							_tsc = GREEN;
							_previousMillis = currentMillis;
						}
					}
				}

				else
				{
					for (uint8_t index = 0; index < _outputList.Count(); index++)
					{
						LocoPWMOutput*  output = _outputList[index];
						if (output->getOn())
						{
							output->setOn(Off);							
						}
					}
				}

				break;
				
			}

			case CHASE: {
				if (_flags.getSet(GROUP_ON)) {
					if (_outputList.Count() > 1) {
						if (currentMillis - _previousMillis > getRate()) {
							if (_count == 0) {
								_outputList[_outputList.Count() - 1]->setOn(Off);
								LocoNet.reportSensor(_outputList[_outputList.Count() - 1]->getAddress(), Off);
								_outputList[_count]->setOn(On);
								LocoNet.reportSensor(_outputList[_count]->getAddress(), On);
							}
							else {
								_outputList[_count - 1]->setOn(Off);
								LocoNet.reportSensor(_outputList[_count - 1]->getAddress(), Off);
								_outputList[_count]->setOn(On);
								LocoNet.reportSensor(_outputList[_count]->getAddress(), On);
							}
							_count++;

							if (_count == _outputList.Count()) {
								_count = 0;
							}
						}
					}
						
				}
				else {
					for (uint8_t index = 0; index < _outputList.Count(); index++) {
						_outputList[index]->setOn(Off);
						LocoNet.reportSensor(_outputList[index]->getAddress(), Off);
					}

				}
			}

			default:
			{
				break;
			}
		}
	}
}