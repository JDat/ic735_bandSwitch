//#define debug

#define tallyPin 9

#define txInPin 10          // TXing from tranceiver

#define rotarySwitchPin A3
#define analogRefPin A0     // +8V form tranceiver
#define bandInPin A1        // BAND analog input

// relay outputs to ULN
#define relayPin1 8
#define relayPin2 7
#define relayPin3 6
#define relayPin4 5
#define relayPin5 4
#define relayPin6 3
#define relayPin7 2
uint8_t pinBitArray[]={relayPin1, relayPin2, relayPin3, relayPin4, relayPin5, relayPin6, relayPin7,
                      tallyPin};

//storage for output relay state
uint8_t pinByte;

// ADC values for rotary switch
#define rotaryVal0  10
#define rotaryVal1  120
#define rotaryVal2  200
#define rotaryVal3  270
#define rotaryVal4  1023
#define rotaryVal5  1023
#define rotaryVal6  1023
#define rotaryVal7  1023
#define rotaryVal8  1023
#define rotaryVal9  1023
#define rotaryVal10 1023
#define rotaryVal11 1023
int rotaryValues[]={rotaryVal0, rotaryVal1, rotaryVal2, rotaryVal3, 
                    rotaryVal4, rotaryVal5, rotaryVal6, rotaryVal7, 
                    rotaryVal8, rotaryVal9, rotaryVal10, rotaryVal11};

// ADC values for BAN input
#define band30      10
#define band10      20
#define band15      30
#define band20      40
#define band40      50
#define band80      60
#define band160     70
int bandValues[]={band30, band10, band15, band20, band40, band80, band160};

#define relay1    0
#define relay2    1
#define relay3    2
#define relay4    3
#define relay5    4
#define relay6    5
#define relay7    6

#define tallyBit  7

#define nc        255   // no connection nothig used for this band

//This is cruitical
// in each element need to be relay number, who should be turned on
//bands assinged in follwing order (numbers ar meters): 30m,10m,15m,20m,40m,80m,160m
uint8_t bandMatrix[]={nc,nc,relay3,relay2,nc,relay1,nc}; //  for analog band voltage

// each array element represent switch position
// in each element need to be relay number, who should be turned on
// element 0 is reserved for auto mode, do not use and leav as nc
uint8_t rotaryMatrix[]={nc,relay1,relay2,relay3};         // for rotary switch

// macro hack
#define checkTX() bitWrite( pinByte, tallyBit, (digitalRead(txInPin) ? 0 : 1))

void setup() {

#ifdef debug
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif

  //Set pins to output
  for (uint8_t i=0; i<sizeof(pinBitArray); i++){
    pinMode(pinBitArray[i],OUTPUT);
  }
  
  pinMode(txInPin,INPUT_PULLUP);

}

void loop() {
  uint8_t a;    // temp variable
  uint8_t b;    // temp variable
  
  pinByte=0;              // start by releasing all relay bits (all relays off)
  
  a=readRotary();         // Read rotary switch
  if ( a>0 && a<255 ){
    b=rotaryMatrix[a];
    if (b<255) {
        bitWrite( pinByte, b, 1);    // If rotary active then set relay
    }
  }
  else {
    a=readBand();
    if (a<255){
      b=bandMatrix[a];
      if (b<255) {
        bitWrite( pinByte,b,1);
      }
    }
  }

  checkTX();              // check for TXing
  setPins();              // Set Relay hardware pins from pinByte variable

#ifdef debug
  //Serial.println(pinByte,BIN);
  //Serial.println(a);
  //Serial.print(analogRead(rotarySwitchPin));
  //Serial.print("\t");
  //Serial.println(readRotary());
  delay(100);
#endif
  
}

void setPins(){
  for ( uint8_t i=0; i<sizeof(pinBitArray); i++){
    digitalWrite(pinBitArray[i], (bitRead(pinByte, i) ? HIGH : LOW));
  }
}

uint8_t readRotary(){
  int a;
  a=analogRead(rotarySwitchPin);
  for (uint8_t i=0; i<(sizeof(rotaryValues)/sizeof(rotaryValues[0]));i++){
    if (a<rotaryValues[i]) return i;
  }
  return 255;
}

uint8_t readBand(){
  int a;
  a=analogRead(bandInPin);
#ifdef debug
  Serial.println(a);
#endif  
  for (uint8_t i=0; i<(sizeof(bandValues)/sizeof(bandValues[0]));i++){
    if (a<bandValues[i]) return i;
  }
  return 255;  
}
