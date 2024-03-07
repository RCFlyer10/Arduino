#include "LocoNetHelper.h"

LocoNetHelperClass::LocoNetHelperClass() {
}

uint8_t LocoNetHelperClass::processStationaryDecoderInterrogate(lnMsg* LnPacket) {	
	if (LnPacket->sr.command == OPC_SW_REQ) {
		Serial.println("Yes");
		if ((!(((LnPacket->srq.sw2 & 0xCF) == 0x0F) && ((LnPacket->srq.sw1 & 0xFC) == 0x78))) 
			&& (!(((LnPacket->srq.sw2 & 0xCF) == 0x07) && ((LnPacket->srq.sw1 & 0xFC) == 0x78)))) {	
			// ordinary form, LPU V1.0 page 9
			// do not handle cases which are not "stationary decoder interrogate" messages
			return 0;
		}
		else {			
			// handle cases which are "stationary decoder interrogate" messages.
			if (notifyStationaryDecoderInterrogate) {
				uint8_t range = 0;
				range |= (LnPacket->srq.sw2 & 0x20) >> 3;
				range |= (LnPacket->srq.sw1 & 0x02);
				range |= (LnPacket->srq.sw1 & 0x01);
				notifyStationaryDecoderInterrogate(range);
				return 1;
			}
		}		
	}
	return 0;
}

uint8_t LocoNetHelperClass::processProgramTaskMessage(lnMsg* LnPacket) {	
	uint16_t Address;	
	uint16_t CV;
	uint8_t pCMD;
	uint8_t Value;		
	
	// Programmer Task message
	if (LnPacket->data[0] == OPC_WR_SL_DATA && LnPacket->data[2] == PRG_SLOT)	{ 
		progTaskMsg* pMSG = &LnPacket->pt;
		CV = PROG_CV_NUM(pMSG) + 1;
		pCMD = pMSG->pcmd & PCMD_MODE_MASK;		
		
		switch (pCMD) {
			case OPS_BYTE_NO_FEEDBACK:			
				Address = ((pMSG->lopsa & 0x7F) | ((pMSG->hopsa & 0x7F) << 7));				
				// write								
				if (notifyOpsModeByteWrite) {
					Value = PROG_DATA(pMSG);
					notifyOpsModeByteWrite(Address, CV, Value);
					return 1;
				}
				break;				
			case OPS_BYTE_FEEDBACK:			
				Address = ((pMSG->lopsa & 0x7F) | ((pMSG->hopsa & 0x7F) << 7));				
				// read				
				if (notifyOpsModeByteRead) {
					notifyOpsModeByteRead(Address, CV);
					return 1;
				}								
				break;			
			case PAGED_ON_SRVC_TRK:				
				// write
				if (pMSG->pcmd & PCMD_RW)	{					
					if (notifySvsModeByteWrite) {
						Value = PROG_DATA(pMSG);
						notifySvsModeByteWrite(CV, Value);
						return 1;
					}
				}
				// read
				else {
				}				
				break;				
			default:
				return 0;
		}		
	}
	return 0;
}

LN_STATUS LocoNetHelperClass::programTaskResponse(uint16_t Address, uint16_t CV, uint8_t Value) {
	lnMsg SendPacket;	
	SendPacket.data[0] = OPC_SL_RD_DATA;
	SendPacket.data[1] = 0x0E;
	SendPacket.data[2] = PRG_SLOT;
	SendPacket.data[3] = OPS_BYTE_NO_FEEDBACK | PCMD_RW;
	SendPacket.data[4] = 0;
	SendPacket.data[5] = Address >> 7;
	SendPacket.data[6] = Address & 0x7F;
	SendPacket.data[7] = 0;
	SendPacket.data[8] = (((CV >> 4) & 0x30) | ((CV & 0x80) >> 7) | ((Value & 0x80) >> 6));
	SendPacket.data[9] = CV & 0x7F;
	SendPacket.data[10] = Value & 0x7F;	
	SendPacket.data[11] = 0;
	SendPacket.data[12] = 0;
	return LocoNet.send(&SendPacket);
}