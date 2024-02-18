// List.h

#ifndef _LIST_h
#define _LIST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define GROUPOUTPUTS 10

class LocoPWMOutput;

class List {
	public:
		List();
		void Add(LocoPWMOutput* output);
		void Remove(LocoPWMOutput* output);
		LocoPWMOutput* operator[](const size_t index);
		size_t Count() const;

	private:
		LocoPWMOutput* outputList[GROUPOUTPUTS];
		uint8_t items = 0;
};
#endif

