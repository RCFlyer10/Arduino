#ifndef Flags_h
#define Flags_h

#include <Arduino.h>

class Flags {
	public:	
		void setFlag(uint8_t n) {
			_flags |= 1 << n;
		}		
		void clearFlag(uint8_t n) {
			_flags &= ~(1 << n);
		}		
		uint8_t getSet(uint8_t n) {
			return ((_flags >> n) & 1);
		}
		uint8_t getNotSet(uint8_t n) {
			return ((~(_flags >> n)) & 1);
		}		
		uint8_t getFlags() {
			return _flags;
		}
		
	private:	
		uint8_t _flags;
};
#endif