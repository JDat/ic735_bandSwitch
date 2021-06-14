//#define debug             // Uncomment to enable serial debug

#define tallyPin 9          // TX/PTT out from aarduino to relay

#define txInPin 10          // TX/PTT from tranceiver(out) to arduino(in)

#define rotarySwitchPin A3  // Rotary switch position analog pin to arduino(in)
#define analogRefPin A0     // +8V reference form tranceiver(out) to arduino(in). Currently no used in code
#define bandInPin A1        // BAND voltage from tranceiver(out) to arduino(in) analog input

// relay outputs to ULN
#define relayPin1 8         // Arduino out
#define relayPin2 7         // Arduino out
#define relayPin3 6         // Arduino out
#define relayPin4 5         // Arduino out
#define relayPin5 4         // Arduino out
#define relayPin6 3         // Arduino out
#define relayPin7 2         // Arduino out for TX/PTT lamp
// Phisical pin ampping in array
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
// ADC Rotary switch values array for easy access in loop code. Don't remember why weird order.
int rotaryValues[]={rotaryVal0, rotaryVal1, rotaryVal2, rotaryVal3, 
                    rotaryVal4, rotaryVal5, rotaryVal6, rotaryVal7, 
                    rotaryVal8, rotaryVal9, rotaryVal10, rotaryVal11};

// ADC values for BAND input. Measured on BAND pin and digitalized in arduino.
#define band30      50
#define band10      230
#define band15      340
#define band20      450
#define band40      600
#define band80      750
#define band160     970

// ADC BAND value array for easy access in loop code. Order copied from IC-735 manual
int bandValues[]={band30, band10, band15, band20, band40, band80, band160};

// Logical relay numbers for easy to use in bandMatrix code, no need to change
#define relay1    0
#define relay2    1
#define relay3    2
#define relay4    3
#define relay5    4
#define relay6    5
#define relay7    6

#define tallyBit  7

#define nc        255   // no connection, nothig used for this band. For tricky programming.

//This is cruitical
// in each element need to be relay number, who should be turned on
//bands assinged in follwing order (numbers ar meters):
//                   30m,10m,  15m,  20m,40m,  80m,160m
// TODO: rewrite to more easy use and more logical code
uint8_t bandMatrix[]={nc,     // 30m
                      nc,     // 10m
                      relay3, // 15m
                      relay2, // 20m
                      nc,     // 40m
                      relay1, // 80m
                      nc};    // 160m

// each array element represent switch position
// in each element need to be relay number, who should be turned on
// element 0 is reserved for auto mode, do not use and leave as nc
// TODO: rewrite as BAND names for easy of use
uint8_t rotaryMatrix[]={nc,relay1,relay2,relay3};         // for rotary switch

// macro hack
#define checkTX() bitWrite( pinByte, tallyBit, (digitalRead(txInPin) ? 0 : 1))

void setup() {

  #ifdef debug
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only (arduino micro/pro micro)
  }
  #endif

  //Set relay pins to output
  for (uint8_t i = 0; i < sizeof(pinBitArray); i++){
    pinMode(pinBitArray[i], OUTPUT);
  }
  
  pinMode(txInPin, INPUT_PULLUP);

}

void loop() {
  uint8_t analogIndex;    // temp variable
  uint8_t pinIndex;    // temp variable
  
  pinByte = 0;              // start by releasing all relay bits (all relays off)
  
  analogIndex = readRotary();         // Read rotary switch
  if ( analogIndex > 0 && analogIndex < nc ) {     // if true, we are in manual mode
    pinIndex = rotaryMatrix[analogIndex];    // get logical relay num from array
    if (pinIndex < nc) {           // do nothing if we don't have relay for this rotary position
        bitWrite( pinByte, pinIndex, 1 );    // Set logical relay in pinByte storage variable
    }
  }
  else {                  // we have nc, so we are in auto mode
    analogIndex = readBand();         // Get BAND from tranceiver and return array index
    if ( analogIndex < nc ) {            // Do nothing if no antenna in this BAND. Array have Not Connected (nc)
      pinIndex = bandMatrix[analogIndex];    // Get logical relay index from banndArray
      if ( pinIndex < nc ) {         // Do nothing if no relay assingend (nc)
        bitWrite( pinByte, pinIndex, 1 );   // Set logical relay bin in pinByte storage variable
      }
    }
  }

  checkTX();              // check for TXing
  setPins();              // Translate from pinBits array logical relay numbers to hardware pins

  #ifdef debug
  Serial.print("Logical Pin (relay) state: ");
  Serial.println(pinByte,BIN);
  Serial.println(a);
  Serial.print("Rotary ADC RAW value: ");
  Serial.print(analogRead(rotarySwitchPin));
  Serial.print("\t");
  Serial.print("Rotary switch position: ");
  Serial.println(readRotary());
  delay(100);
  #endif
  
}

/*
* Set hardware relay pins from pinByte variable in quick loop
*/
void setPins(){
  for ( uint8_t i = 0; i < sizeof(pinBitArray); i++){
    digitalWrite( pinBitArray[i], (bitRead(pinByte, i) ? HIGH : LOW) );
  }
}

/*
* Read rotary switch RAW value, convert to array index for array and return it.
*/
uint8_t readRotary(){
  int a;
  a = analogRead(rotarySwitchPin);
  for ( uint8_t i=0; i < ( sizeof(rotaryValues) / sizeof(rotaryValues[0]) ); i++ ){
    if ( a < rotaryValues[i] ) return i;
  }
  return nc;
}

/*
* Read BAND RAW value, convert to array index for array and return it.
*/
uint8_t readBand(){
  int a;
  a = analogRead(bandInPin);

  #ifdef debug
  Serial.print("BAND RAW Analog:\t");
  Serial.print(a);
  #endif  

  for (uint8_t i=0; i < ( sizeof(bandValues)/sizeof(bandValues[0]) ); i++){
    if ( a < bandValues[i] ) {
      #ifdef debug
      Serial.print("\tBand index: ");
      Serial.println(i);
      #endif
      return i;
    }
  }
  
  #ifdef debug
  Serial.print("\tBand index (nc): ");
  Serial.println(nc);
  #endif

  return nc;  
}
