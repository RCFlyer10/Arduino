
// NMRA Dcc Multifunction Lighting Decoder
#include <Function_Led_V2.h>
#include <NmraDcc.h>

// Uncomment any of the lines below to enable debug messages for different parts of the code
//#define DEBUG_FUNCTIONS
//#define DEBUG_SPEED
//#define DEBUG_PWM
//#define DEBUG_DCC_ACK
//#define DEBUG_DCC_MSG

#if defined(DEBUG_FUNCTIONS) or defined(DEBUG_SPEED) or defined(DEBUG_PWM) or defined(DEBUG_DCC_ACK) or defined(DEBUG_DCC_MSG)
#define DEBUG_PRINT
#endif

#define DEBUG_PRINT

// This section defines the Arduino UNO Pins to use
#ifdef __AVR_ATmega328P__

// Define the Arduino input Pin number for the DCC Signal
#define DCC_PIN 2
#define ACK_PIN 8
#define FX1_PIN 6
#define FX2_PIN 5
#define FX3_PIN 9
#define FX4_PIN 10
#define FX5_PIN 11
#define FX6_PIN 3

#else
#error "Unsupported CPU, you need to add another configuration section for your CPU"
#endif

// Comment this line out after first initia
//#define InitalizeCVs

#define FUNCTION_GROUPS 5
#define FUNCTIONS 6
#define FEATURES 6

// cache function map and current function state
uint8_t functionMap [FUNCTIONS][FUNCTION_GROUPS];
uint8_t functionState [FUNCTIONS];

// cache consist direction, status and enable
bool consistFwd = true;
bool consisted = false;
uint8_t consistFuncEnable = 0;

// cache direction enable
uint8_t fwdDirEnable = 0;
uint8_t revDirEnable = 0;

// cache cv29, address and direction
uint8_t cv29Config = 0;
uint16_t myAddress = 0;
DCC_DIRECTION direction = DCC_DIR_FWD;

// function list
Function_Led *functionList[FUNCTIONS];
Function_Led functionFX1 = Function_Led(FX1_PIN);
Function_Led functionFX2 = Function_Led(FX2_PIN);
Function_Led functionFX3 = Function_Led(FX3_PIN);
Function_Led functionFX4 = Function_Led(FX4_PIN);
Function_Led functionFX5 = Function_Led(FX5_PIN);
Function_Led functionFX6 = Function_Led(FX6_PIN);

// Structure for CV Values Table
struct CVPair {
  uint16_t CV;
  uint8_t Value;
};

// CV Addresses we will be using
#define CV_CONSIST_ADDR 19

#define CV_CONSIST_FUNC_EN 21

#define CV_FWD_DIR_EN 45
#define CV_REV_DIR_EN 46

// configuration CVs
#define CV_FX1_CONFIG 60
#define CV_FX2_CONFIG 61
#define CV_FX3_CONFIG 62
#define CV_FX4_CONFIG 63
#define CV_FX5_CONFIG 64
#define CV_FX6_CONFIG 65
#define CV_FX1_BRIGHT 66
#define CV_FX2_BRIGHT 67
#define CV_FX3_BRIGHT 68
#define CV_FX4_BRIGHT 69
#define CV_FX5_BRIGHT 70
#define CV_FX6_BRIGHT 71
#define CV_FX1_DIM_VALUE 72
#define CV_FX2_DIM_VALUE 73
#define CV_FX3_DIM_VALUE 74
#define CV_FX4_DIM_VALUE 75
#define CV_FX5_DIM_VALUE 76
#define CV_FX6_DIM_VALUE 77
#define CV_FX1_FADE_RATE 78
#define CV_FX2_FADE_RATE 79
#define CV_FX3_FADE_RATE 80
#define CV_FX4_FADE_RATE 81
#define CV_FX5_FADE_RATE 82
#define CV_FX6_FADE_RATE 83
#define CV_FX1_FLASH_RATE 84
#define CV_FX2_FLASH_RATE 85
#define CV_FX3_FLASH_RATE 86
#define CV_FX4_FLASH_RATE 87
#define CV_FX5_FLASH_RATE 88
#define CV_FX6_FLASH_RATE 89
#define CV_FX1_PROBABILITY 90
#define CV_FX2_PROBABILITY 91
#define CV_FX3_PROBABILITY 92
#define CV_FX4_PROBABILITY 93
#define CV_FX5_PROBABILITY 94
#define CV_FX6_PROBABILITY 95

// function map CVs
#define CV_FN_MAP_FX1_F0_F4 100
#define CV_FN_MAP_FX1_F5_F8 101
#define CV_FN_MAP_FX1_F9_F12 102 
#define CV_FN_MAP_FX1_F13_F20 103
#define CV_FN_MAP_FX1_F21_F28 104
#define CV_FN_MAP_FX2_F0_F4 105
#define CV_FN_MAP_FX2_F5_F8 106
#define CV_FN_MAP_FX2_F9_F12 107
#define CV_FN_MAP_FX2_F13_F20 108
#define CV_FN_MAP_FX2_F21_F28 109
#define CV_FN_MAP_FX3_F0_F4 110
#define CV_FN_MAP_FX3_F5_F8 111
#define CV_FN_MAP_FX3_F9_F12 112
#define CV_FN_MAP_FX3_F13_F20 113
#define CV_FN_MAP_FX3_F21_F28 114
#define CV_FN_MAP_FX4_F0_F4 115
#define CV_FN_MAP_FX4_F5_F8 116
#define CV_FN_MAP_FX4_F9_F12 117
#define CV_FN_MAP_FX4_F13_F20 118
#define CV_FN_MAP_FX4_F21_F28 119
#define CV_FN_MAP_FX5_F0_F4 120
#define CV_FN_MAP_FX5_F5_F8 121
#define CV_FN_MAP_FX5_F9_F12 122
#define CV_FN_MAP_FX5_F13_F20 123
#define CV_FN_MAP_FX5_F21_F28 124
#define CV_FN_MAP_FX6_F0_F4 125
#define CV_FN_MAP_FX6_F5_F8 126
#define CV_FN_MAP_FX6_F9_F12 127
#define CV_FN_MAP_FX6_F13_F20 128
#define CV_FN_MAP_FX6_F21_F28 129

// Default CV Values Table
CVPair FactoryDefaultCVs[] = {
  // The CV Below defines the Short DCC Address
  { CV_MULTIFUNCTION_PRIMARY_ADDRESS, DEFAULT_MULTIFUNCTION_DECODER_ADDRESS },
  
  // The CVs below define the lighting configuration
  { CV_FX1_CONFIG, 0 },
  { CV_FX2_CONFIG, 0 },
  { CV_FX3_CONFIG, 0 },
  { CV_FX4_CONFIG, 0 },
  { CV_FX5_CONFIG, 0 },
  { CV_FX6_CONFIG, 0 },
  { CV_FX1_BRIGHT, 255 },
  { CV_FX2_BRIGHT, 255 },  
  { CV_FX3_BRIGHT, 255 },
  { CV_FX4_BRIGHT, 255 },
  { CV_FX5_BRIGHT, 255 },
  { CV_FX6_BRIGHT, 255 },
  { CV_FX1_DIM_VALUE, 128 },
  { CV_FX2_DIM_VALUE, 128 },
  { CV_FX3_DIM_VALUE, 128 },
  { CV_FX4_DIM_VALUE, 128 },
  { CV_FX5_DIM_VALUE, 128 },
  { CV_FX6_DIM_VALUE, 128 }, 
  { CV_FX1_FADE_RATE, 128 },
  { CV_FX2_FADE_RATE, 128 },
  { CV_FX3_FADE_RATE, 128 },
  { CV_FX4_FADE_RATE, 128 },
  { CV_FX5_FADE_RATE, 128 },
  { CV_FX6_FADE_RATE, 128 },
  { CV_FX1_FLASH_RATE, 128 },
  { CV_FX2_FLASH_RATE, 128 },
  { CV_FX3_FLASH_RATE, 128 },
  { CV_FX4_FLASH_RATE, 128 },
  { CV_FX5_FLASH_RATE, 128 },
  { CV_FX6_FLASH_RATE, 128 },
  { CV_FX1_PROBABILITY, 50 }, 
  { CV_FX2_PROBABILITY, 50 },  
  { CV_FX3_PROBABILITY, 50 },  
  { CV_FX4_PROBABILITY, 50 },
  { CV_FX5_PROBABILITY, 50 },
  { CV_FX6_PROBABILITY, 50 },  

  // The CVs Below define the Function Map
  { CV_FN_MAP_FX1_F0_F4, 16 },
  { CV_FN_MAP_FX1_F5_F8, 0 },
  { CV_FN_MAP_FX1_F9_F12, 2 },
  { CV_FN_MAP_FX1_F13_F20, 0 },
  { CV_FN_MAP_FX1_F21_F28, 0 },
  { CV_FN_MAP_FX2_F0_F4, 1 },
  { CV_FN_MAP_FX2_F5_F8, 0 },
  { CV_FN_MAP_FX2_F9_F12, 2 },
  { CV_FN_MAP_FX2_F13_F20, 0 },
  { CV_FN_MAP_FX2_F21_F28, 0 },
  { CV_FN_MAP_FX3_F0_F4, 4 },
  { CV_FN_MAP_FX3_F5_F8, 0 },
  { CV_FN_MAP_FX3_F9_F12, 2 },
  { CV_FN_MAP_FX3_F13_F20, 0 },
  { CV_FN_MAP_FX3_F21_F28, 0 },
  { CV_FN_MAP_FX4_F0_F4, 8 },  
  { CV_FN_MAP_FX4_F5_F8, 0 },  
  { CV_FN_MAP_FX4_F9_F12, 2 },  
  { CV_FN_MAP_FX4_F13_F20, 0 },  
  { CV_FN_MAP_FX4_F21_F28, 0 },
  { CV_FN_MAP_FX5_F0_F4, 0},
  { CV_FN_MAP_FX5_F5_F8, 1},
  { CV_FN_MAP_FX5_F9_F12, 2},
  { CV_FN_MAP_FX5_F13_F20, 0},
  { CV_FN_MAP_FX5_F21_F28, 0},
  { CV_FN_MAP_FX6_F0_F4, 0},
  { CV_FN_MAP_FX6_F5_F8, 2},
  { CV_FN_MAP_FX6_F9_F12, 2},
  { CV_FN_MAP_FX6_F13_F20, 0},
  { CV_FN_MAP_FX6_F21_F28, 0},


  // The CV Below defines Advanced Consist Address
  { CV_CONSIST_ADDR, 0 },

  // The CV Below defines Advanced Consist Function Enable 1
  { CV_CONSIST_FUNC_EN, 255 },  

  // The CV Below defines Forward Direction Enable
  { CV_FWD_DIR_EN, 255 },

  // The CV Below defines Reverse Direction Enable
  { CV_REV_DIR_EN, 255 },

  // These two CVs define the Long DCC Address
  { CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB, 0 },
  { CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB, 0 },

  // ONLY uncomment 1 CV_29_CONFIG line below as approprate
  //  {CV_29_CONFIG,                                      0}, // Short Address 14 Speed Steps
  { CV_29_CONFIG, CV29_F0_LOCATION },  // Short Address 28/128 Speed Steps
  //  {CV_29_CONFIG, CV29_EXT_ADDRESSING | CV29_F0_LOCATION}, // Long  Address 28/128 Speed Steps
};

// instansiate the DCC library
NmraDcc Dcc;

uint8_t FactoryDefaultCVIndex = 0;

// This call-back function is called when a CV Value changes so we can update CVs we're using
void notifyCVChange(uint16_t CV, uint8_t Value) {
  switch (CV) { 
    case CV_CONSIST_ADDR:      
    case CV_MULTIFUNCTION_PRIMARY_ADDRESS:
    case CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB:
    case CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB:
      setAddress();
      break;
    case CV_CONSIST_FUNC_EN:
      consistFuncEnable = Value;
      break;
    case CV_29_CONFIG:
      cv29Config = Value;
      break;
    case CV_FWD_DIR_EN:
      fwdDirEnable = Value;
      break;
    case CV_REV_DIR_EN:
      revDirEnable = Value;
      break;
    case CV_FX1_CONFIG ... CV_FX1_CONFIG + (FUNCTIONS * FEATURES) - 1:
      updateFunctions(CV, Value);
      break;      
    case CV_FN_MAP_FX1_F0_F4 ... CV_FN_MAP_FX1_F0_F4 + (FUNCTION_GROUPS * FUNCTIONS) - 1:    
      updateFunctionMap(CV, Value);
      break;

#ifdef DEBUG_PRINT
      Serial.print(CV);
      Serial.print(" ");
      Serial.println(Value);
#endif
  }
}

void updateFunctions(uint16_t CV, uint8_t Value) {  
  int index = CV % FUNCTIONS;  
  Function_Led *function = functionList[index];
  switch (CV) {
    case CV_FX1_CONFIG ... (CV_FX1_CONFIG + FUNCTIONS) -1:
      function->setConfig(Value);
      break;
    case CV_FX1_BRIGHT ... (CV_FX1_BRIGHT + FUNCTIONS) -1:
      function->setBrightness(Value);
      break;
    case CV_FX1_DIM_VALUE ... (CV_FX1_DIM_VALUE + FUNCTIONS) -1:
      function->setDimValue(Value);
      break;
    case CV_FX1_FADE_RATE ... (CV_FX1_FADE_RATE + FUNCTIONS) -1:
      function->setFadeRate(Value);
      break;
    case CV_FX1_FLASH_RATE ... (CV_FX1_FLASH_RATE + FUNCTIONS) -1:
      function->setFlashRate(Value);
      break;
    default:
      function->setProbability(Value);    
  }
}

void updateFunctionMap(uint16_t CV, uint8_t Value) {   
  int x;
  int y = CV % FUNCTION_GROUPS;
  switch (CV) {
    case CV_FN_MAP_FX1_F0_F4 ... CV_FN_MAP_FX1_F21_F28:
      x = 0;
      break;
    case CV_FN_MAP_FX2_F0_F4 ... CV_FN_MAP_FX2_F21_F28:
      x = 1;
      break;
    case CV_FN_MAP_FX3_F0_F4 ... CV_FN_MAP_FX3_F21_F28:
      x = 2;
      break;
    case CV_FN_MAP_FX4_F0_F4 ... CV_FN_MAP_FX4_F21_F28:
      x = 3;
      break;
    case CV_FN_MAP_FX5_F0_F4 ... CV_FN_MAP_FX5_F21_F28:
      x = 4;
      break;
    default:
      x = 5;
  }
  functionMap[x][y] = Value;    
}

void setAddress() {
  uint8_t addr = Dcc.getCV(CV_CONSIST_ADDR);
  if (addr > 0) {
    myAddress = addr & 0x7f;
    consistFwd = (addr & 0x80) ? false : true;    
    consisted = true;
  }
  else {
    myAddress = Dcc.getAddr();
    consisted = false;
  }  
} 

void notifyCVResetFactoryDefault() {
  // Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
  FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs) / sizeof(CVPair);
}

// This call-back function is called whenever we receive a DCC Speed packet for our address
void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
#ifdef DEBUG_SPEED
  Serial.print("notifyDccSpeed: Addr: ");
  Serial.print(Addr, DEC);
  Serial.print((AddrType == DCC_ADDR_SHORT) ? "-S" : "-L");
  Serial.print(" Speed: ");
  Serial.print(Speed, DEC);
  Serial.print(" Steps: ");
  Serial.print(SpeedSteps, DEC);
  Serial.print(" Dir: ");
  Serial.println((Dir == DCC_DIR_FWD) ? "Forward" : "Reverse");
#endif
  bool dimmerOn;
  if (Addr == myAddress) {
    if (consisted && !consistFwd) {       
      direction = ((cv29Config & CV29_LOCO_DIR) ^ Dir) ? DCC_DIR_REV : DCC_DIR_FWD;      
    }
    else {
      direction = ((cv29Config & CV29_LOCO_DIR) ^ Dir) ? DCC_DIR_FWD : DCC_DIR_REV;
    }
    dimmerOn = (direction == DCC_DIR_REV) ? true : false;    
    for (int i = 0; i < FUNCTIONS; i++) {
      functionList[i]->setDimmerOn(dimmerOn);
    } 
  }   
}

// This call-back function is called whenever we receive a DCC Function packet for our address
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState) {
#ifdef DEBUG_FUNCTIONS
  Serial.print("notifyDccFunc: Addr: ");
  Serial.print(Addr, DEC);
  Serial.print((AddrType == DCC_ADDR_SHORT) ? 'S' : 'L');
  Serial.print("  Function Group: ");
  Serial.print(FuncGrp, DEC);
#endif

  bool state = false;  
  uint8_t functionBits;
  uint8_t functionEnableBits;  
  if (Addr == myAddress) {    
    for (int x = 0; x < FUNCTIONS; x++) {      
      functionBits = functionMap[x][FuncGrp - 1];      
      if (functionBits) {         
        functionEnableBits = 1 << x;        
        if (consisted) {
          if (direction == DCC_DIR_FWD) {
            state = ((FuncState & functionBits) && (fwdDirEnable & functionEnableBits) && (consistFuncEnable & functionEnableBits)) ? true : false;  
          } 
          else {
            state = ((FuncState & functionBits) && (revDirEnable & functionEnableBits) && (consistFuncEnable & functionEnableBits)) ? true : false;
          }
        }
        else {        
          if (direction == DCC_DIR_FWD) {
            state = ((FuncState & functionBits) && (fwdDirEnable & functionEnableBits)) ? true : false;
          }     
          else {
            state = ((FuncState & functionBits) && (revDirEnable & functionEnableBits)) ? true : false;
          }
        }        
        if (state) {
          functionState[x] |= 1 << FuncGrp;
        }
        else {
          functionState[x] &= ~(1 << FuncGrp);
        }        
      }
      functionList[x]->setState(functionState[x]);
    }    
  }
#ifdef DEBUG_FUNCTIONS
  Serial.println();
#endif
}

// This call-back function is called whenever we receive a DCC Packet
#ifdef DEBUG_DCC_MSG
void notifyDccMsg(DCC_MSG* Msg) {
  Serial.print("notifyDccMsg: ");
  for (uint8_t i = 0; i < Msg->Size; i++) {
    Serial.print(Msg->Data[i], HEX);
    Serial.write(' ');
  }
  Serial.println();
}
#endif

// This call-back function is called by the NmraDcc library when a DCC ACK needs to be sent
// Calling this function should cause an increased 60ma current drain on the power supply for 6ms to ACK a CV Read
// So we will just turn the motor on for 8ms and then turn it off again.

void notifyCVAck(void) {
#ifdef DEBUG_DCC_ACK
  Serial.println("Acknowledge");
#endif
  digitalWrite(ACK_PIN, HIGH);
  delay(8);
  digitalWrite(ACK_PIN, LOW);
}

void setup() {
#ifdef DEBUG_PRINT
  Serial.begin(115200);
  uint8_t maxWaitLoops = 255;
  while (!Serial && maxWaitLoops--)
    delay(20);

  Serial.println("NMRA Dcc Multifunction Motor Decoder Demo");
#endif

  // Setup the Pin for the ACK  
  pinMode(ACK_PIN, OUTPUT);

  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  // Many Arduino Cores now support the digitalPinToInterrupt() function that makes it easier to figure out the
  // Interrupt Number for the Arduino Pin number, which reduces confusion.

#ifdef digitalPinToInterrupt
  Dcc.pin(DCC_PIN, false);
#else
  Dcc.pin(0, DCC_PIN, false);
#endif

  Dcc.init(MAN_ID_DIY, 10, FLAGS_OUTPUT_ADDRESS_MODE | FLAGS_AUTO_FACTORY_DEFAULT, 0);

  // Load the Function List
  functionList[0] = &functionFX1;
  functionList[1] = &functionFX2;
  functionList[2] = &functionFX3;
  functionList[3] = &functionFX4;
  functionList[4] = &functionFX5;
  functionList[5] = &functionFX6; 

  setAddress();
  // Read the current CV values into cache 
  consistFuncEnable = Dcc.getCV(CV_CONSIST_FUNC_EN);  
  fwdDirEnable = Dcc.getCV(CV_FWD_DIR_EN);
  revDirEnable = Dcc.getCV(CV_REV_DIR_EN);
  cv29Config = Dcc.getCV(CV_29_CONFIG);  

  // cache Function Map
  uint16_t index = CV_FN_MAP_FX1_F0_F4; 
  uint8_t functionBits; 
  for (int x = 0; x < FUNCTIONS; x++) {       
    for (int y = 0; y < FUNCTION_GROUPS; y++) {
      functionBits = Dcc.getCV(index);                
      if (functionBits > 0) {        
        functionMap[x][y] = functionBits;       
      }
      index++;     
    }
  }

  // load funtion settings
  for (int index = CV_FX1_CONFIG; index < CV_FX1_CONFIG + (FEATURES * FUNCTIONS); index++ ) {
    updateFunctions(index, Dcc.getCV(index));
  }  
  
#ifdef InitalizeCVs
  notifyCVResetFactoryDefault();
#warning "Initializing CVs, please configure to not do so next upload!!!!!!!!!!!!!!"
#endif  
}

void loop() {
  // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  Dcc.process();

  for (int i = 0; i < FUNCTIONS; i++) {
    functionList[i]->heartbeat();
  }  

  // Handle resetting CVs back to Factory Defaults
  if (FactoryDefaultCVIndex && Dcc.isSetCVReady()) {
    FactoryDefaultCVIndex--;  // Decrement first as initially it is the size of the array
    Dcc.setCV(FactoryDefaultCVs[FactoryDefaultCVIndex].CV, FactoryDefaultCVs[FactoryDefaultCVIndex].Value);
  }
}
