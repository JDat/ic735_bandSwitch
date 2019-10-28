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
#define band160     10
#define band80      10
#define band40      10
#define band20      10
#define band15      10
#define band10      10
#define band30      10
int bandValues[]={band160, band80, band40, band20, band15, band10, band30};

#define tallyBit 7

// macro hack
#define checkTX() bitWrite( pinByte, tallyBit, (digitalRead(txInPin) ? 0 : 1))

void setup() {
  
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Set pins to output
  for (uint8_t i=0; i<sizeof(pinBitArray); i++){
    pinMode(pinBitArray[i],OUTPUT);
  }
  
  pinMode(txInPin,INPUT_PULLUP);

}

void loop() {
  uint8_t a;    // temp variable

  pinByte=0;              // start by releasing arr relay bits
  a=readRotary();         // Read rotary switch
  if ( a>0 && a<255 ) bitWrite( pinByte, a-1, 1);    // If rotary active then set relay
  else {
    a=readBand();
    if (a<255){
      bitWrite( pinByte,a,1);
    }
  }

  checkTX();              // check for TXing
  setPins();              // Set Relay hardware pins from pinByte variable
  
  //Serial.println(pinByte,BIN);
  //Serial.println(readRotary());
  //Serial.print(analogRead(rotarySwitchPin));
  //Serial.print("\t");
  //Serial.println(readRotary());
  //delay(100);
  
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
  for (uint8_t i=0; i<(sizeof(bandValues)/sizeof(bandValues[0]));i++){
    if (a<bandValues[i]) return i;
  }
  return 255;  
}
