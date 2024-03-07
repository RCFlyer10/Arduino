#include "List.h"
#include <EEPROM.h>
#include "LocoPWMOutput.h"
#include "OutputGroup.h"
#include <Adafruit_PWMServoDriver.h>
#include <LocoNet.h>
#include "LocoNetHelper.h"
#include "Flags.h"

#define LNtxPin               7       // LocoNet Transmit pin (LocoShield uses pin 9)
#define addrPE               15
#define addrPLed             14
#define On                    1
#define Off                   0
#define addrLSBset            0
#define addrMSBset            1
#define addrProgamEnable      2


// Comment this line out after first initialization
//#define InitializeCVs

#define DEBUG

Flags flags = Flags();

uint8_t FactoryDefaultCVIndex = 0;

const int OUTPUTS = 20;
const int OUTPUTGROUPS = 10;
unsigned long progMillis = 0;
uint16_t BoardAddress;

// Structure for CV Values Table
struct CVPair {
  uint16_t CV;
  uint8_t Value;
};

#define CV_VERSION 7
#define CV_MANUFACTURE 8
#define CV_LONG_ADDRESS_MSB 17
#define CV_LONG_ADDRESS_LSB 18

#define CV_OUTPUT_0_EFFECTS 60
#define CV_OUTPUT_1_EFFECTS 61
#define CV_OUTPUT_2_EFFECTS 62
#define CV_OUTPUT_3_EFFECTS 63
#define CV_OUTPUT_4_EFFECTS 64
#define CV_OUTPUT_5_EFFECTS 65
#define CV_OUTPUT_6_EFFECTS 66
#define CV_OUTPUT_7_EFFECTS 67
#define CV_OUTPUT_8_EFFECTS 68
#define CV_OUTPUT_9_EFFECTS 69
#define CV_OUTPUT_10_EFFECTS 70
#define CV_OUTPUT_11_EFFECTS 71
#define CV_OUTPUT_12_EFFECTS 72
#define CV_OUTPUT_13_EFFECTS 73
#define CV_OUTPUT_14_EFFECTS 74
#define CV_OUTPUT_15_EFFECTS 75
#define CV_OUTPUT_16_EFFECTS 76
#define CV_OUTPUT_17_EFFECTS 77
#define CV_OUTPUT_18_EFFECTS 78
#define CV_OUTPUT_19_EFFECTS 79

#define CV_OUTPUT_0_ADDRESS_LSB 80
#define CV_OUTPUT_1_ADDRESS_LSB 81
#define CV_OUTPUT_2_ADDRESS_LSB 82
#define CV_OUTPUT_3_ADDRESS_LSB 83
#define CV_OUTPUT_4_ADDRESS_LSB 84
#define CV_OUTPUT_5_ADDRESS_LSB 85
#define CV_OUTPUT_6_ADDRESS_LSB 86
#define CV_OUTPUT_7_ADDRESS_LSB 87
#define CV_OUTPUT_8_ADDRESS_LSB 88
#define CV_OUTPUT_9_ADDRESS_LSB 89
#define CV_OUTPUT_10_ADDRESS_LSB 90
#define CV_OUTPUT_11_ADDRESS_LSB 91
#define CV_OUTPUT_12_ADDRESS_LSB 92
#define CV_OUTPUT_13_ADDRESS_LSB 93
#define CV_OUTPUT_14_ADDRESS_LSB 94
#define CV_OUTPUT_15_ADDRESS_LSB 95
#define CV_OUTPUT_16_ADDRESS_LSB 96
#define CV_OUTPUT_17_ADDRESS_LSB 97
#define CV_OUTPUT_18_ADDRESS_LSB 98
#define CV_OUTPUT_19_ADDRESS_LSB 99

#define CV_OUTPUT_0_ADDRESS_MSB 100
#define CV_OUTPUT_1_ADDRESS_MSB 101
#define CV_OUTPUT_2_ADDRESS_MSB 102
#define CV_OUTPUT_3_ADDRESS_MSB 103
#define CV_OUTPUT_4_ADDRESS_MSB 104
#define CV_OUTPUT_5_ADDRESS_MSB 105
#define CV_OUTPUT_6_ADDRESS_MSB 106
#define CV_OUTPUT_7_ADDRESS_MSB 107
#define CV_OUTPUT_8_ADDRESS_MSB 108
#define CV_OUTPUT_9_ADDRESS_MSB 109
#define CV_OUTPUT_10_ADDRESS_MSB 110
#define CV_OUTPUT_11_ADDRESS_MSB 111
#define CV_OUTPUT_12_ADDRESS_MSB 112
#define CV_OUTPUT_13_ADDRESS_MSB 113
#define CV_OUTPUT_14_ADDRESS_MSB 114
#define CV_OUTPUT_15_ADDRESS_MSB 115
#define CV_OUTPUT_16_ADDRESS_MSB 116
#define CV_OUTPUT_17_ADDRESS_MSB 117
#define CV_OUTPUT_18_ADDRESS_MSB 118
#define CV_OUTPUT_19_ADDRESS_MSB 119

#define CV_OUTPUT_0_CONFIG_1 120
#define CV_OUTPUT_1_CONFIG_1 121
#define CV_OUTPUT_2_CONFIG_1 122
#define CV_OUTPUT_3_CONFIG_1 123
#define CV_OUTPUT_4_CONFIG_1 124
#define CV_OUTPUT_5_CONFIG_1 125
#define CV_OUTPUT_6_CONFIG_1 126
#define CV_OUTPUT_7_CONFIG_1 127
#define CV_OUTPUT_8_CONFIG_1 128
#define CV_OUTPUT_9_CONFIG_1 129
#define CV_OUTPUT_10_CONFIG_1 130
#define CV_OUTPUT_11_CONFIG_1 131
#define CV_OUTPUT_12_CONFIG_1 132
#define CV_OUTPUT_13_CONFIG_1 133
#define CV_OUTPUT_14_CONFIG_1 134
#define CV_OUTPUT_15_CONFIG_1 135
#define CV_OUTPUT_16_CONFIG_1 136
#define CV_OUTPUT_17_CONFIG_1 137
#define CV_OUTPUT_18_CONFIG_1 138
#define CV_OUTPUT_19_CONFIG_1 139

#define CV_OUTPUT_0_CONFIG_2 140
#define CV_OUTPUT_1_CONFIG_2 141
#define CV_OUTPUT_2_CONFIG_2 142
#define CV_OUTPUT_3_CONFIG_2 143
#define CV_OUTPUT_4_CONFIG_2 144
#define CV_OUTPUT_5_CONFIG_2 145
#define CV_OUTPUT_6_CONFIG_2 146
#define CV_OUTPUT_7_CONFIG_2 147
#define CV_OUTPUT_8_CONFIG_2 148
#define CV_OUTPUT_9_CONFIG_2 149
#define CV_OUTPUT_10_CONFIG_2 150
#define CV_OUTPUT_11_CONFIG_2 151
#define CV_OUTPUT_12_CONFIG_2 152
#define CV_OUTPUT_13_CONFIG_2 153
#define CV_OUTPUT_14_CONFIG_2 154
#define CV_OUTPUT_15_CONFIG_2 155
#define CV_OUTPUT_16_CONFIG_2 156
#define CV_OUTPUT_17_CONFIG_2 157
#define CV_OUTPUT_18_CONFIG_2 158
#define CV_OUTPUT_19_CONFIG_2 159

#define CV_OUTPUT_0_CONFIG_3 160
#define CV_OUTPUT_1_CONFIG_3 161
#define CV_OUTPUT_2_CONFIG_3 162
#define CV_OUTPUT_3_CONFIG_3 163
#define CV_OUTPUT_4_CONFIG_3 164
#define CV_OUTPUT_5_CONFIG_3 165
#define CV_OUTPUT_6_CONFIG_3 166
#define CV_OUTPUT_7_CONFIG_3 167
#define CV_OUTPUT_8_CONFIG_3 168
#define CV_OUTPUT_9_CONFIG_3 169
#define CV_OUTPUT_10_CONFIG_3 170
#define CV_OUTPUT_11_CONFIG_3 171
#define CV_OUTPUT_12_CONFIG_3 172
#define CV_OUTPUT_13_CONFIG_3 173
#define CV_OUTPUT_14_CONFIG_3 174
#define CV_OUTPUT_15_CONFIG_3 175
#define CV_OUTPUT_16_CONFIG_3 176
#define CV_OUTPUT_17_CONFIG_3 177
#define CV_OUTPUT_18_CONFIG_3 178
#define CV_OUTPUT_19_CONFIG_3 179

#define CV_OUTPUT_0_OUTPUTGROUP 180
#define CV_OUTPUT_1_OUTPUTGROUP 181
#define CV_OUTPUT_2_OUTPUTGROUP 182
#define CV_OUTPUT_3_OUTPUTGROUP 183
#define CV_OUTPUT_4_OUTPUTGROUP 184
#define CV_OUTPUT_5_OUTPUTGROUP 185
#define CV_OUTPUT_6_OUTPUTGROUP 186
#define CV_OUTPUT_7_OUTPUTGROUP 187
#define CV_OUTPUT_8_OUTPUTGROUP 188
#define CV_OUTPUT_9_OUTPUTGROUP 189
#define CV_OUTPUT_10_OUTPUTGROUP 190
#define CV_OUTPUT_11_OUTPUTGROUP 191
#define CV_OUTPUT_12_OUTPUTGROUP 192
#define CV_OUTPUT_13_OUTPUTGROUP 193
#define CV_OUTPUT_14_OUTPUTGROUP 194
#define CV_OUTPUT_15_OUTPUTGROUP 195
#define CV_OUTPUT_16_OUTPUTGROUP 196
#define CV_OUTPUT_17_OUTPUTGROUP 197
#define CV_OUTPUT_18_OUTPUTGROUP 198
#define CV_OUTPUT_19_OUTPUTGROUP 199

#define CV_OUTPUTGROUP_0_TYPE 200
#define CV_OUTPUTGROUP_1_TYPE 201
#define CV_OUTPUTGROUP_2_TYPE 202
#define CV_OUTPUTGROUP_3_TYPE 203
#define CV_OUTPUTGROUP_4_TYPE 204
#define CV_OUTPUTGROUP_5_TYPE 205
#define CV_OUTPUTGROUP_6_TYPE 206
#define CV_OUTPUTGROUP_7_TYPE 207
#define CV_OUTPUTGROUP_8_TYPE 208
#define CV_OUTPUTGROUP_9_TYPE 209

#define CV_OUTPUTGROUP_0_ADDRESS_LSB 210
#define CV_OUTPUTGROUP_1_ADDRESS_LSB 211
#define CV_OUTPUTGROUP_2_ADDRESS_LSB 212
#define CV_OUTPUTGROUP_3_ADDRESS_LSB 213
#define CV_OUTPUTGROUP_4_ADDRESS_LSB 214
#define CV_OUTPUTGROUP_5_ADDRESS_LSB 215
#define CV_OUTPUTGROUP_6_ADDRESS_LSB 216
#define CV_OUTPUTGROUP_7_ADDRESS_LSB 217
#define CV_OUTPUTGROUP_8_ADDRESS_LSB 218
#define CV_OUTPUTGROUP_9_ADDRESS_LSB 219

#define CV_OUTPUTGROUP_0_ADDRESS_MSB 220
#define CV_OUTPUTGROUP_1_ADDRESS_MSB 221
#define CV_OUTPUTGROUP_2_ADDRESS_MSB 222
#define CV_OUTPUTGROUP_3_ADDRESS_MSB 223
#define CV_OUTPUTGROUP_4_ADDRESS_MSB 224
#define CV_OUTPUTGROUP_5_ADDRESS_MSB 225
#define CV_OUTPUTGROUP_6_ADDRESS_MSB 226
#define CV_OUTPUTGROUP_7_ADDRESS_MSB 227
#define CV_OUTPUTGROUP_8_ADDRESS_MSB 228
#define CV_OUTPUTGROUP_9_ADDRESS_MSB 229

#define CV_OUTPUTGROUP_0_CONFIG_1 230
#define CV_OUTPUTGROUP_1_CONFIG_1 231
#define CV_OUTPUTGROUP_2_CONFIG_1 232
#define CV_OUTPUTGROUP_3_CONFIG_1 233
#define CV_OUTPUTGROUP_4_CONFIG_1 234
#define CV_OUTPUTGROUP_5_CONFIG_1 235
#define CV_OUTPUTGROUP_6_CONFIG_1 236
#define CV_OUTPUTGROUP_7_CONFIG_1 237
#define CV_OUTPUTGROUP_8_CONFIG_1 238
#define CV_OUTPUTGROUP_9_CONFIG_1 239

#define CV_OUTPUTGROUP_0_CONFIG_2 240
#define CV_OUTPUTGROUP_1_CONFIG_2 241
#define CV_OUTPUTGROUP_2_CONFIG_2 242
#define CV_OUTPUTGROUP_3_CONFIG_2 243
#define CV_OUTPUTGROUP_4_CONFIG_2 244
#define CV_OUTPUTGROUP_5_CONFIG_2 245
#define CV_OUTPUTGROUP_6_CONFIG_2 246
#define CV_OUTPUTGROUP_7_CONFIG_2 247
#define CV_OUTPUTGROUP_8_CONFIG_2 248
#define CV_OUTPUTGROUP_9_CONFIG_2 249

#define CV_START_UP_BYTE_0 250
#define CV_START_UP_BYTE_1 251
#define CV_START_UP_BYTE_2 252
#define CV_START_UP_BYTE_3 253



// Default CV Values Table
const CVPair FactoryDefaultCVs[] PROGMEM = {

  { CV_VERSION, 2 },
  { CV_MANUFACTURE, 13 },

  // These two CVs define the Long DCC Address
  { CV_LONG_ADDRESS_MSB, 250 },
  { CV_LONG_ADDRESS_LSB, 152 },

  { CV_OUTPUT_0_EFFECTS, 0 },
  { CV_OUTPUT_1_EFFECTS, 0 },
  { CV_OUTPUT_2_EFFECTS, 0 },
  { CV_OUTPUT_3_EFFECTS, 0 },
  { CV_OUTPUT_4_EFFECTS, 0 },
  { CV_OUTPUT_5_EFFECTS, 0 },
  { CV_OUTPUT_6_EFFECTS, 0 },
  { CV_OUTPUT_7_EFFECTS, 0 },
  { CV_OUTPUT_8_EFFECTS, 0 },
  { CV_OUTPUT_9_EFFECTS, 0 },
  { CV_OUTPUT_10_EFFECTS, 0 },
  { CV_OUTPUT_11_EFFECTS, 0 },
  { CV_OUTPUT_12_EFFECTS, 0 },
  { CV_OUTPUT_13_EFFECTS, 0 },
  { CV_OUTPUT_14_EFFECTS, 0 },
  { CV_OUTPUT_15_EFFECTS, 0 },
  { CV_OUTPUT_16_EFFECTS, 0 },
  { CV_OUTPUT_17_EFFECTS, 0 },
  { CV_OUTPUT_18_EFFECTS, 0 },
  { CV_OUTPUT_19_EFFECTS, 0 },

  { CV_OUTPUT_0_ADDRESS_LSB, 208 },
  { CV_OUTPUT_1_ADDRESS_LSB, 208 },
  { CV_OUTPUT_2_ADDRESS_LSB, 208 },
  { CV_OUTPUT_3_ADDRESS_LSB, 208 },
  { CV_OUTPUT_4_ADDRESS_LSB, 208 },
  { CV_OUTPUT_5_ADDRESS_LSB, 208 },
  { CV_OUTPUT_6_ADDRESS_LSB, 208 },
  { CV_OUTPUT_7_ADDRESS_LSB, 208 },
  { CV_OUTPUT_8_ADDRESS_LSB, 208 },
  { CV_OUTPUT_9_ADDRESS_LSB, 208 },
  { CV_OUTPUT_10_ADDRESS_LSB, 208 },
  { CV_OUTPUT_11_ADDRESS_LSB, 208 },
  { CV_OUTPUT_12_ADDRESS_LSB, 208 },
  { CV_OUTPUT_13_ADDRESS_LSB, 208 },
  { CV_OUTPUT_14_ADDRESS_LSB, 208 },
  { CV_OUTPUT_15_ADDRESS_LSB, 208 },
  { CV_OUTPUT_16_ADDRESS_LSB, 208 },
  { CV_OUTPUT_17_ADDRESS_LSB, 208 },
  { CV_OUTPUT_18_ADDRESS_LSB, 208 },
  { CV_OUTPUT_19_ADDRESS_LSB, 208 },

  { CV_OUTPUT_0_ADDRESS_MSB, 7 },
  { CV_OUTPUT_1_ADDRESS_MSB, 7 },
  { CV_OUTPUT_2_ADDRESS_MSB, 7 },
  { CV_OUTPUT_3_ADDRESS_MSB, 7 },
  { CV_OUTPUT_4_ADDRESS_MSB, 7 },
  { CV_OUTPUT_5_ADDRESS_MSB, 7 },
  { CV_OUTPUT_6_ADDRESS_MSB, 7 },
  { CV_OUTPUT_7_ADDRESS_MSB, 7 },
  { CV_OUTPUT_8_ADDRESS_MSB, 7 },
  { CV_OUTPUT_9_ADDRESS_MSB, 7 },
  { CV_OUTPUT_10_ADDRESS_MSB, 7 },
  { CV_OUTPUT_11_ADDRESS_MSB, 7 },
  { CV_OUTPUT_12_ADDRESS_MSB, 7 },
  { CV_OUTPUT_13_ADDRESS_MSB, 7 },
  { CV_OUTPUT_14_ADDRESS_MSB, 7 },
  { CV_OUTPUT_15_ADDRESS_MSB, 7 },
  { CV_OUTPUT_16_ADDRESS_MSB, 7 },
  { CV_OUTPUT_17_ADDRESS_MSB, 7 },
  { CV_OUTPUT_18_ADDRESS_MSB, 7 },
  { CV_OUTPUT_19_ADDRESS_MSB, 7 },

  { CV_OUTPUT_0_CONFIG_1, 15 },
  { CV_OUTPUT_1_CONFIG_1, 15 },
  { CV_OUTPUT_2_CONFIG_1, 15 },
  { CV_OUTPUT_3_CONFIG_1, 15 },
  { CV_OUTPUT_4_CONFIG_1, 15 },
  { CV_OUTPUT_5_CONFIG_1, 15 },
  { CV_OUTPUT_6_CONFIG_1, 15 },
  { CV_OUTPUT_7_CONFIG_1, 15 },
  { CV_OUTPUT_8_CONFIG_1, 15 },
  { CV_OUTPUT_9_CONFIG_1, 15 },
  { CV_OUTPUT_10_CONFIG_1, 15 },
  { CV_OUTPUT_11_CONFIG_1, 15 },
  { CV_OUTPUT_12_CONFIG_1, 15 },
  { CV_OUTPUT_13_CONFIG_1, 15 },
  { CV_OUTPUT_14_CONFIG_1, 15 },
  { CV_OUTPUT_15_CONFIG_1, 15 },
  { CV_OUTPUT_16_CONFIG_1, 15 },
  { CV_OUTPUT_17_CONFIG_1, 15 },
  { CV_OUTPUT_18_CONFIG_1, 15 },
  { CV_OUTPUT_19_CONFIG_1, 15 },

  { CV_OUTPUT_0_CONFIG_2, 119 },
  { CV_OUTPUT_1_CONFIG_2, 119 },
  { CV_OUTPUT_2_CONFIG_2, 119 },
  { CV_OUTPUT_3_CONFIG_2, 119 },
  { CV_OUTPUT_4_CONFIG_2, 119 },
  { CV_OUTPUT_5_CONFIG_2, 119 },
  { CV_OUTPUT_6_CONFIG_2, 119 },
  { CV_OUTPUT_7_CONFIG_2, 119 },
  { CV_OUTPUT_8_CONFIG_2, 119 },
  { CV_OUTPUT_9_CONFIG_2, 119 },
  { CV_OUTPUT_10_CONFIG_2, 119 },
  { CV_OUTPUT_11_CONFIG_2, 119 },
  { CV_OUTPUT_12_CONFIG_2, 119 },
  { CV_OUTPUT_13_CONFIG_2, 119 },
  { CV_OUTPUT_14_CONFIG_2, 119 },
  { CV_OUTPUT_15_CONFIG_2, 119 },
  { CV_OUTPUT_16_CONFIG_2, 119 },
  { CV_OUTPUT_17_CONFIG_2, 119 },
  { CV_OUTPUT_18_CONFIG_2, 119 },
  { CV_OUTPUT_19_CONFIG_2, 119 },

  { CV_OUTPUT_0_CONFIG_3, 119 },
  { CV_OUTPUT_1_CONFIG_3, 119 },
  { CV_OUTPUT_2_CONFIG_3, 119 },
  { CV_OUTPUT_3_CONFIG_3, 119 },
  { CV_OUTPUT_4_CONFIG_3, 119 },
  { CV_OUTPUT_5_CONFIG_3, 119 },
  { CV_OUTPUT_6_CONFIG_3, 119 },
  { CV_OUTPUT_7_CONFIG_3, 119 },
  { CV_OUTPUT_8_CONFIG_3, 119 },
  { CV_OUTPUT_9_CONFIG_3, 119 },
  { CV_OUTPUT_10_CONFIG_3, 119 },
  { CV_OUTPUT_11_CONFIG_3, 119 },
  { CV_OUTPUT_12_CONFIG_3, 119 },
  { CV_OUTPUT_13_CONFIG_3, 119 },
  { CV_OUTPUT_14_CONFIG_3, 119 },
  { CV_OUTPUT_15_CONFIG_3, 119 },
  { CV_OUTPUT_16_CONFIG_3, 119 },
  { CV_OUTPUT_17_CONFIG_3, 119 },
  { CV_OUTPUT_18_CONFIG_3, 119 },
  { CV_OUTPUT_19_CONFIG_3, 119 },

  { CV_OUTPUT_0_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_1_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_2_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_3_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_4_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_5_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_6_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_7_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_8_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_9_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_10_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_11_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_12_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_13_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_14_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_15_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_16_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_17_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_18_OUTPUTGROUP, OUTPUTGROUPS },
  { CV_OUTPUT_19_OUTPUTGROUP, OUTPUTGROUPS },

  { CV_OUTPUTGROUP_0_TYPE, 0},
  { CV_OUTPUTGROUP_1_TYPE, 0},
  { CV_OUTPUTGROUP_2_TYPE, 0},
  { CV_OUTPUTGROUP_3_TYPE, 0},
  { CV_OUTPUTGROUP_4_TYPE, 0 },
  { CV_OUTPUTGROUP_5_TYPE, 0 },
  { CV_OUTPUTGROUP_6_TYPE, 0 },
  { CV_OUTPUTGROUP_7_TYPE, 0 },
  { CV_OUTPUTGROUP_8_TYPE, 0 },
  { CV_OUTPUTGROUP_9_TYPE, 0 },

  { CV_OUTPUTGROUP_0_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_1_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_2_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_3_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_4_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_5_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_6_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_7_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_8_ADDRESS_LSB, 160 },
  { CV_OUTPUTGROUP_9_ADDRESS_LSB, 160 },

  { CV_OUTPUTGROUP_0_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_1_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_2_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_3_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_4_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_5_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_6_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_7_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_8_ADDRESS_MSB, 15 },
  { CV_OUTPUTGROUP_9_ADDRESS_MSB, 15 },

  { CV_OUTPUTGROUP_0_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_1_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_2_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_3_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_4_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_5_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_6_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_7_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_8_CONFIG_1, 119 },
  { CV_OUTPUTGROUP_9_CONFIG_1, 119 },

  { CV_OUTPUTGROUP_0_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_1_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_2_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_3_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_4_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_5_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_6_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_7_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_8_CONFIG_2, 7 },
  { CV_OUTPUTGROUP_9_CONFIG_2, 7 },
  
  { CV_START_UP_BYTE_0, 0 },
  { CV_START_UP_BYTE_1, 0 },
  { CV_START_UP_BYTE_2, 0 },
  { CV_START_UP_BYTE_3, 0 }
};

LocoPWMOutput outputs[OUTPUTS];
OutputGroup outputGroups[OUTPUTGROUPS];


// Create PWM driver, it uses the default address 0x40
Adafruit_PWMServoDriver ledDriver = Adafruit_PWMServoDriver();

void createOutputGroups() {
  for (uint8_t index = 0; index < OUTPUTGROUPS; index++) {
    outputGroups[index] = OutputGroup();
  }
}

void createOuputs() { 

    // On chip PWM outputs
  outputs[0] = LocoPWMOutput(11, false); 
  outputs[1] = LocoPWMOutput(6, false); 
  outputs[2] = LocoPWMOutput(5, false); 
  outputs[3] = LocoPWMOutput(3, false);
  
    // PWM Servo Driver outputs
  for (uint8_t index = 0; index < OUTPUTS - 4; index++) {
    outputs[index + 4] = LocoPWMOutput(index, true);    
  } 
}

void updateOuputSettings(uint16_t CV, uint8_t Value) {
  int index = CV % OUTPUTS; 

  switch (CV) {
      case CV_OUTPUT_0_EFFECTS ... (CV_OUTPUT_0_EFFECTS + OUTPUTS) - 1: {
          outputs[index].setEffect(Value);
      }
        break;
      case CV_OUTPUT_0_ADDRESS_LSB ... (CV_OUTPUT_0_ADDRESS_LSB + OUTPUTS) - 1: {
          outputs[index].setAddressLSB(Value);
      }
        break;
      case CV_OUTPUT_0_ADDRESS_MSB ... (CV_OUTPUT_0_ADDRESS_MSB + OUTPUTS) - 1: {
          outputs[index].setAddressMSB(Value);
      }
        break;
      case CV_OUTPUT_0_CONFIG_1 ... (CV_OUTPUT_0_CONFIG_1 + OUTPUTS) - 1: {
          outputs[index].setConfig_1(Value);
      }
        break;
      case CV_OUTPUT_0_CONFIG_2 ... (CV_OUTPUT_0_CONFIG_2 + OUTPUTS) - 1: {
          outputs[index].setConfig_2(Value);
      }
        break;
      case CV_OUTPUT_0_CONFIG_3 ... (CV_OUTPUT_0_CONFIG_3 + OUTPUTS) - 1: {
          outputs[index].setConfig_3(Value);
      }
        break;
      case CV_OUTPUT_0_OUTPUTGROUP ... (CV_OUTPUT_0_OUTPUTGROUP + OUTPUTS) - 1: {
          outputs[index].setOutputGroup(Value);
      }
        break;          
  }  
}

void updateOutputGroupSettings(uint16_t CV, uint8_t Value) {
  int index = CV % OUTPUTGROUPS;

  switch (CV) {
      case CV_OUTPUTGROUP_0_TYPE ... (CV_OUTPUTGROUP_0_TYPE + OUTPUTGROUPS) - 1: {
          outputGroups[index].setGroupType(Value);
      }
        break;
      case CV_OUTPUTGROUP_0_ADDRESS_LSB ... (CV_OUTPUTGROUP_0_ADDRESS_LSB + OUTPUTGROUPS) - 1: {
          outputGroups[index].setAddressLSB(Value);
      }
        break;
      case CV_OUTPUTGROUP_0_ADDRESS_MSB ... (CV_OUTPUTGROUP_0_ADDRESS_MSB + OUTPUTGROUPS) - 1: {
          outputGroups[index].setAddressMSB(Value);
      }
        break;
      case CV_OUTPUTGROUP_0_CONFIG_1 ... (CV_OUTPUTGROUP_0_CONFIG_1 + OUTPUTGROUPS) - 1: {
          outputGroups[index].setConfig1(Value);
      }
      case CV_OUTPUTGROUP_0_CONFIG_2 ... (CV_OUTPUTGROUP_0_CONFIG_2 + OUTPUTGROUPS) - 1: {
          outputGroups[index].setConfig2(Value);
      }
        break;    
  }
}

void updateStartupItems() {

    uint32_t items;
    uint8_t shift = 0;

    EEPROM.get(CV_START_UP_BYTE_0, items); // get startup items

    for (uint8_t index; index < OUTPUTS; index++) { // turn on appropriate outputs

        if (items & (1 << shift)) {

            outputs[index].setOn(On);
            shift += 1;
        }
    }

    for (uint8_t index; index < OUTPUTGROUPS; index++) { // turn on appropriate outputgroups

        if (items & (1 << shift)) {

            outputGroups[index].setOn(On);
            shift += 1;
        }
    }
}

lnMsg *LnPacket;

unsigned long previousMillis;

void setup() 
{
  // First initialize the LocoNet interface
  LocoNet.init(LNtxPin); 

  pinMode(addrPE, INPUT);
  pinMode(addrPLed, OUTPUT);
  digitalWrite(addrPLed, 1);

  if (!digitalRead(addrPE)) {    
    flags.setFlag(addrProgamEnable);
  }
  // Initialize PWM
  ledDriver.begin();

  BoardAddress = getBoardAddress();  

  createOutputGroups();

  createOuputs(); 

  // load output group settings
  for (uint16_t CV = CV_OUTPUTGROUP_0_TYPE; CV <= CV_OUTPUTGROUP_9_CONFIG_2; CV++) {
      uint8_t value = EEPROM.read(CV);
      updateOutputGroupSettings(CV, value);       
  }    
  
  // load output settings
  for (uint16_t CV = CV_OUTPUT_0_EFFECTS; CV <= CV_OUTPUT_19_OUTPUTGROUP; CV++ ) {
      uint8_t value = EEPROM.read(CV);
      updateOuputSettings(CV, value);        
  }

  updateStartupItems();

#ifdef DEBUG
  // Configure the serial port for 115200 baud
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("Loco PWM"));
  Serial.print(F("Board Address "));
  BoardAddress = getBoardAddress();
  Serial.println(BoardAddress);
#endif
  
#ifdef InitializeCVs
  notifyCVResetFactoryDefault();
  #warning "Initializing CVs, please configure to not do so next upload!!!!!!!!!!!!!!"
#endif      
}

void notifyCVResetFactoryDefault() {
  // Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
  FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs) / sizeof(CVPair);  

  
}

void loop() {
  LnPacket = LocoNet.receive();
  if (LnPacket) {    
    
     // Process the packet for Program Task Message
    if (!LocoNetHelper.processProgramTaskMessage(LnPacket)) {
      // Process the packet for Stationary Decoder Interrogate Message
      if (!LocoNetHelper.processStationaryDecoderInterrogate(LnPacket)) {
        // Process the packet for Switch or Sensor Message        
        LocoNet.processSwitchSensorMessage(LnPacket);
      }
    }   
  } 

  if (FactoryDefaultCVIndex) {    
    FactoryDefaultCVIndex--;  // Decrement first as initially it is the size of the array    
    EEPROM.write(pgm_read_word(&FactoryDefaultCVs[FactoryDefaultCVIndex].CV), pgm_read_byte(&FactoryDefaultCVs[FactoryDefaultCVIndex].Value));    
  } 
  
  // update the heartbeat for each output group
  for (uint8_t index = 0; index < OUTPUTGROUPS; index++) {
    outputGroups[index].heartbeat();
  } 

  // update the heartbeat for each output
  for (uint8_t index = 0; index < OUTPUTS; index++) {
    outputs[index].heartbeat();
  } 

  if (flags.getSet(addrProgamEnable)) {
    unsigned long currentMillis = millis();
    if (currentMillis - progMillis >= 500) {
      digitalWrite(addrPLed, !digitalRead(addrPLed));
      progMillis = currentMillis;
    }
  }  
}


int getBoardAddress() {  
  return (((EEPROM.read(CV_LONG_ADDRESS_MSB) - 192) << 8) | EEPROM.read(CV_LONG_ADDRESS_LSB));
}

// This call-back function is called from LocoNetHelper.processStationaryDecoderInterrogage
// for all Stationary Decoder Interrogate messages
void notifyStationaryDecoderInterrogate(uint8_t AddressRange) {  

  for (uint8_t index = 0; index < OUTPUTS; index++) {
    uint16_t address = outputs[index].getAddress();
    int responseAddr = address % 8;
    

    if (responseAddr == AddressRange) {       
      LocoNet.reportSensor(address, outputs[index].getOn());
    }
  }

  for (uint8_t index = 0; index < OUTPUTGROUPS; index++) {
      uint16_t address = outputGroups[index].getAddress();
      int responseAddr = address % 8;


      if (responseAddr == AddressRange) {
          LocoNet.reportSensor(address, outputGroups[index].getOn());
      }
  }
}

// This call-back function is called from LocoNet.processSwitchSensorMessage
// for all Sensor messages
void notifySensor( uint16_t Address, uint8_t State ) {    

  // Set Outputs State
  for (uint8_t index = 0; index < OUTPUTS; index++) {    
    if (outputs[index].getAddress() == Address) {     
      if(State == OPC_INPUT_REP_HI) {
        outputs[index].setOn(On);        
      }
      else {
        outputs[index].setOn(Off);        
      }           
    }
  }

  // Set OutputGroups State
  for (uint8_t index = 0; index < OUTPUTGROUPS; index++) {
    if (outputGroups[index].getAddress() == Address) {     
      if(State == OPC_INPUT_REP_HI) {
        outputGroups[index].setOn(On);        
      }
      else {
        outputGroups[index].setOn(Off);        
      }           
    }
  }  
}

void notifyOpsModeByteWrite(uint16_t Address, uint16_t CV, uint8_t Value) { 
  if (Address == BoardAddress) {
    if (CV != CV_LONG_ADDRESS_MSB && CV != CV_LONG_ADDRESS_LSB) {
      // save the cv to EEPROM
      EEPROM.update(CV, Value);
      // cache the CV Value
      if (CV < CV_OUTPUTGROUP_0_TYPE) {
        updateOuputSettings(CV, Value);
      }
      else {
        updateOutputGroupSettings(CV, Value);
      }
    }
  }
}

void notifyOpsModeByteRead(uint16_t Address, uint16_t CV) {  
  if (Address == BoardAddress) { 
    // Send program task response 
    uint8_t data = EEPROM.read(CV);
    LocoNetHelper.programTaskResponse(Address, CV, data);
  }
}

void notifySvsModeByteWrite(uint16_t CV, uint8_t Value) {
  if (flags.getSet(addrProgamEnable)) {  
    // Long Address MSB  
    if (CV == CV_LONG_ADDRESS_MSB) {
      EEPROM.update(CV, Value);      
      flags.setFlag(addrMSBset);
    }
    // Long Address LSB
    else if (CV == CV_LONG_ADDRESS_LSB) {
      EEPROM.update(CV, Value);
      flags.setFlag(addrLSBset);      
    }    

    // Board Address set
    if (flags.getSet(addrMSBset) && flags.getSet(addrLSBset)) {
      BoardAddress = getBoardAddress();
      flags.clearFlag(addrProgamEnable);
      flags.clearFlag(addrLSBset);
      flags.clearFlag(addrMSBset);
      digitalWrite(addrPLed, 1);
    }
  }

  // Factory reset
  else if (CV == 8 && Value == 8) {
    notifyCVResetFactoryDefault();
  }   
}
