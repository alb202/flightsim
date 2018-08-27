// -----------------------------------------------------------------------------------------------------
/*
battery      1
alternator    1
avionics    1
fuel pump   1
beacon      1
landing light 1
taxi light    1
navigation light1
strobe light  1
pitot heat    1
parking brake 1
landing gear  1
carb heat   1
engine off    1
magneto R   1
magneto L   1
magneto both  1
engine start  1
*/
// -----------------------------------------------------------------------------------------------------
// Set the codes that are transmitted to the computer when a switch is used
// Set the pin combinations that are used in any multi-pin inputs, using binary, and the number of unique combinations
// Set the pin numbers to be used for each input, and the number of pins used

//#include "Arduino.h"
//#include "string.h"

// Codes for toggle switches; in each pair, the first value is 'off' and the second is 'on'
const String TOGGLECODES[][2] = {
  {"E20","E21"}, {"E17","E18"}, {"A430","A431"}, {"NONE","NONE"},
  {"F020","F021"}, {"C420","C421"}, {"C430","C431"}, {"C440","C441"},
  {"C410", "C411"}, {"C450","C451"}, {"C06","C05"}, {"C02", "C01"}, {"F13", "F14"}
  };

const byte TOGGLEPINS[] = {2,3,4,5,6,7,8,9,10,11,12,18,19};
const byte TOGGLEPINCOUNT = 13; 

const String TOGGLEALWAYSONCODES[][2] = {{"C040","C041"}};
const byte TOGGLEALWAYSONPINS[] = {17};
const byte TOGGLEALWAYSONPINCOUNT = 1; 

// Codes for ignition
const String IGNITIONCODES[] = {"E11", "E12", "E13", "E14", "E41"};
const byte IGNITIONINPUTCODES[] = {B110, B010, B100, B000, B011};  // Input codes that map the input from the fuel tank switch to the output codes
const byte IGNITIONINPUTCODECOUNT = 5;
const byte IGNITIONPINS[] = {14,15,16};
const byte IGNITIONPINCOUNT = 3;

// ----------------------------------------------------------------------------------------------
// Other constants
const int DELAY = 10;    // Time interval between checking the switch status (in milliseconds)
const bool FLIPONOFF = true;// Set to 'true' if the ON/OFF values of each input are reversed (1 = Off, 0 = On)
const int DEBOUNCE = 100;  // Amount of time in milliseconds to debounce
const int DEBOUNCETRIES = 1; // Number of times the debounce function should check the value
         
void setup() {
  // Open the serial connection
  Serial.begin(115200);
  //Serial.begin(9600);

  // Initialize the digital pins as input
  for (byte x = 0; x < TOGGLEPINCOUNT; x++){
    pinMode(TOGGLEPINS[x], INPUT_PULLUP);
  }

  // Initialize the digital pins as input for always-on toggles
  for (byte x = 0; x < TOGGLEALWAYSONPINCOUNT; x++){
    pinMode(TOGGLEALWAYSONPINS[x], INPUT_PULLUP);
  }
  
  for (byte x = 0; x < IGNITIONPINCOUNT; x++){
    pinMode(IGNITIONPINS[x], INPUT_PULLUP);
  }

}

void loop(){
  Serial.flush();
  
  /*
  // Measure the time of each loop
  static unsigned long old_time;
  static unsigned long new_time;
  new_time = millis();
  Serial.println(new_time-old_time);
  old_time = new_time;
  */
  // -------------------------------------
  // Starting values for inputs
  static int toggle_old = flipOneBit(0); 
  static int toggle_new = 0;
  static int togglealwayson_old = flipOneBit(0); 
  static int togglealwayson_new = 0; 
  static int ignition_old = IGNITIONINPUTCODES[0];
  static int ignition_new = 0;
  
  // ---------- Read the digital inputs ---------------------------
  // Toggle pins
  for (byte x = 0; x < TOGGLEPINCOUNT; x++){
    bitWrite(toggle_new, x, digitalRead(TOGGLEPINS[x]));
  }

  // Toggle always-on pins
  for (byte x = 0; x < TOGGLEALWAYSONPINCOUNT; x++){
    bitWrite(togglealwayson_new, x, digitalRead(TOGGLEALWAYSONPINS[x]));
  }
 
  // Ignition pins
  for (byte x = 0; x < IGNITIONPINCOUNT; x++){
    bitWrite(ignition_new, x, digitalRead(IGNITIONPINS[x]));
  }
  
  // ------- Look for changes and send codes -----------
  // Toggle Switches
  if (toggle_old != toggle_new){
    toggleCheck(toggle_new, toggle_old, TOGGLEPINS, TOGGLEPINCOUNT, TOGGLECODES);
    toggle_old = toggle_new;
  }

  // Toggle always-on Switches
  if (togglealwayson_new == togglealwayson_new){
    toggleAlwaysOnCheck(togglealwayson_new, TOGGLEALWAYSONPINS, TOGGLEALWAYSONPINCOUNT, TOGGLEALWAYSONCODES);
  }

  // Ignition
  if (ignition_old != ignition_new){
    multipinCheck(ignition_new, IGNITIONPINS, IGNITIONPINCOUNT, IGNITIONCODES, IGNITIONINPUTCODES, IGNITIONINPUTCODECOUNT);
    ignition_old = ignition_new; 
  }
  
  // Delay the loop
  delay(DELAY);

  // Ouput loop counter for testing and increment
  // Serial.println(loopCount);
  // static int loopCount = loopCount + 1; 
}


void multipinCheck(int newValue, const byte PINS[], byte PINCOUNT, const String CODES[], const byte INPUTCODES[], byte INPUTCODECOUNT){
  bool isStable = 1;
  for (byte x = 0; x < PINCOUNT; x++){
  // Check the stability of the reading
    if (isStable == 1){
      isStable = digitalDebounce(bitRead(newValue, x), PINS[x]);
    } 
  }
  if (isStable == 1){
    for (byte x = 0; x < INPUTCODECOUNT; x++){
      //if (flipIgnition(newValue) == INPUTCODES[x]){
      if (flipAllBits(newValue, PINCOUNT) == INPUTCODES[x]){
          Serial.println(CODES[x]);
      }
    }
  } 
}

void toggleCheck(int newValue, int oldValue, const byte PINS[], byte PINCOUNT, const String CODES[][2]){
  int changeValue = (oldValue ^ newValue);  
  for (byte x = 0; x < PINCOUNT; x++){
    // If the bit is different, send the code
    if (bitRead(changeValue, x) == 1){
      // Check the stability of the reading
      if (digitalDebounce(bitRead(newValue, x), PINS[x]) == 1){
        // Lookup the code to send
        Serial.println(CODES[x][flipOneBit(bitRead(newValue, x))]); 
      }
    }
  }
}

void toggleAlwaysOnCheck(int newValue, const byte PINS[], byte PINCOUNT, const String CODES[][2]){
//  int changeValue = (oldValue ^ newValue);  
  for (byte x = 0; x < PINCOUNT; x++){
    // If the bit is different, send the code
    //    if (bitRead(changeValue, x) == 1){
    // Check the stability of the reading
    if (digitalDebounce(bitRead(newValue, x), PINS[x]) == 1){
      // Lookup the code to send
      Serial.println(CODES[x][flipOneBit(bitRead(newValue, x))]);
    }
  }
}



byte digitalDebounce (byte newValue, byte pin){
  // This function will use the digital pin value to check the input x times and wait between them, and return 1 if they all agree, 0 if they don't
  for (byte x = 0; x < DEBOUNCETRIES; x++){
    delay(DEBOUNCE);
    byte debounceValue = digitalRead(pin);
    if (newValue != debounceValue){
      return 0; 
    }
  }
  return 1; 
}

byte flipOneBit(byte value){
  // Flip a bit based on the FLIPONOFF variable
  if (FLIPONOFF == true){
    return (value ^ 1);
  } else{
    return value;
  }
}

int flipAllBits(int value, int pins) {
  if (FLIPONOFF == true){
    int mask = (1 << pins) - 1;
    return ~value & mask;
  } else{
    return value;
  }
}
