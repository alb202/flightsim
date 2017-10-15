// -----------------------------------------------------------------------------------------------------
// Output codes for each toggle switch; in each pair, the first value is 'off' and the second is 'on'
const String TOGGLECODES[][2] = {
  {"E20","E21"}, {"E17","E18"}, {"A430","A431"}, {"NONE","NONE"},
  {"F020","F021"}, {"C420","C421"}, {"C430","C431"}, {"C440","C441"},
  {"C410", "C411"}, {"C450","C451"}, {"C06","C05"}, {"BRAKE ON","BRAKE OFF"}
  };
            
// Ouput codes for ignition 
const String IGNITIONCODES[5] = {"E11", "E12", "E13", "E14", "E41"};

// Input codes that map the input from the ignition to the output codes
const byte IGNITIONINPUTCODES[5] = {B110, B010, B100, B000, B011};

// -----------------------------------------------------------------------------------------------------

// Pin numbers for the toggle switches
const byte TOGGLEPINS[] = {2,3,4,5,6,7,8,9,10,11,12,17};

// Pin numbers for the inputs from the ignition switch
const byte IGNITIONPINS[] = {14,15,16};

// Initialize the pin count variables
byte TOGGLEPINCOUNT, IGNITIONPINCOUNT, IGNITIONCODECOUNT;

// Other constants
const int DELAY = 100;    // Time interval between checking the switch status (in milliseconds)
const bool FLIPONOFF = true;// Set to 'true' if the ON/OFF values are reversed (1 = Off, 0 = On)
const int TOGGLEVAL = 0;  // The numerical ID of the toggle code list
const int IGNITIONVAL = 1;  // The numerical ID of the IGNITION code list
const int DEBOUNCE = 10;  // Amount of time in milliseconds to debounce
const int DEBOUNCETRIES = 2; // Number of times the debounce function should check the value

// Starting values for toggle switches
int toggle_old = 0; 
int toggle_new = 0; 

// Starting values for ignition 
int ignition_old = 0;
int ignition_new = 0;


// initialize the loop counter for testing
//int loopCount = 0;
          
void setup() {
  // Open the serial connection
  Serial.begin(115200);
  //Serial.begin(9600);
  
  // Get the number of pins for each device type
  TOGGLEPINCOUNT = (sizeof(TOGGLEPINS)/sizeof(TOGGLEPINS[0]));
  IGNITIONPINCOUNT = (sizeof(IGNITIONPINS)/sizeof(IGNITIONPINS[0]));
  IGNITIONCODECOUNT = (sizeof(IGNITIONINPUTCODES)/sizeof(IGNITIONINPUTCODES[0]));
  
  // Output the number of pins for each type
  //Serial.println("Toggle pins");
  //Serial.println(TOGGLEPINCOUNT); 
  //Serial.println("Ignition pins");
  //Serial.println(IGNITIONPINCOUNT);
 
  // Initialize the digital pins as input with pullup resistors
  for (byte x = 0; x < TOGGLEPINCOUNT; x++){
    pinMode(TOGGLEPINS[x], INPUT_PULLUP);
  }
  for (byte x = 0; x < IGNITIONPINCOUNT; x++){
    pinMode(IGNITIONPINS[x], INPUT_PULLUP);
  }

  toggle_old = flipBit(0);
  ignition_old = IGNITIONINPUTCODES[0];
}

void loop(){
  Serial.flush();
  // Get the values for each device and write it as a bit (0/1)
  for (byte x = 0; x < TOGGLEPINCOUNT; x++){
    bitWrite(toggle_new, x, digitalRead(TOGGLEPINS[x]));
  }

  for (byte x = 0; x < IGNITIONPINCOUNT; x++){
    bitWrite(ignition_new, x, digitalRead(IGNITIONPINS[x]));
  }
  
  // Test the toggle switches for changes
  toggle_old = switchCheck(toggle_old, toggle_new, TOGGLEVAL); 
  
  // Test the ignition for changes
  ignition_old = switchCheck(ignition_old, ignition_new, IGNITIONVAL);
    
  // Delay the loop
  delay(DELAY);
  
  // Ouput loop counter for testing and increment
  //Serial.println(loopCount);
    //loopCount = loopCount + 1; 
}

int switchCheck (int oldValue, int newValue, int switchType) {
  // Check if new value is different than previous value
  if(newValue == oldValue) {
     // If it hasn't changed, return the old value
    return oldValue;
    } else {
    // Find the difference in bits between the new and old values
    int changeValue = (oldValue ^ newValue); 
    // Check if the difference is in the toggle or multi-switch
    if(switchType==TOGGLEVAL){
        for (byte x = 0; x < TOGGLEPINCOUNT; x++){
          // If the bit is different, send the code
          if (bitRead(changeValue, x) == 1){
            // Check the stability of the reading
            if (digitalDebounce(bitRead(newValue, x), TOGGLEPINS[x]) == 1){
              // Lookup the code to send
              Serial.println(TOGGLECODES[x][flipBit(bitRead(newValue, x))]); 
            }}}
    } else {        
        bool isStable = 1;
        for (byte y = 0; y < IGNITIONPINCOUNT; y++){
          // Check the stability of the reading
          if (isStable == 1){
            isStable = digitalDebounce(bitRead(newValue, y), IGNITIONPINS[y]);
          }}
        if (isStable == 1){
          for (byte x = 0; x < IGNITIONCODECOUNT; x++){
            if (flipIgnition(newValue) == IGNITIONINPUTCODES[x]){
              Serial.println(IGNITIONCODES[x]);
            }}} 
        }
    // Send linefeed
    //Serial.println(""); 
    // Return the new value to be saved as the new old value
    return newValue;
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

byte flipBit(byte value){
  // Flip a byte based on the FLIPONOFF variable
  if (FLIPONOFF == true){
    return (value ^ 1);
  } else{
    return value;
  }
}

int flipIgnition(int value){
    // Flip a byte based on the FLIPONOFF variable
  if (FLIPONOFF == true){
    return (value ^ 7);
  } else{
    return value;
  }
}
