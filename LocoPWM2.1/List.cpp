// 
// 
// 

#include "List.h"

List::List() {	
}


void List::Add(LocoPWMOutput* output) {
	if (items <= GROUPOUTPUTS) {
		outputList[items++] = output;
	}
}

void List::Remove(LocoPWMOutput* output) {
	for (uint8_t index = 0; index < items; index++) {
		if (outputList[index] == output) {
			if (index != GROUPOUTPUTS - 1) { // not the last element of the list
				for (uint8_t temp = index; temp < items - 1; temp++) {
					outputList[temp] = outputList[temp + 1];
				}
			}
			items--;
		}
	}
}

LocoPWMOutput* List::operator[](const size_t index) {
	return outputList[index];
}

size_t List::Count() const {
	return items;
}

