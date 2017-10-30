// -----------------------------------------------------------------------------------------------------
/*
fuel shuttoff   1
cabin lights	1
Static air		1
fuel tanks		2
elevator trim	2
cowl flaps		3
flaps			3
throttle		A1
prop			A1			
mixture			A1
*/
// -----------------------------------------------------------------------------------------------------
// Set the codes that are transmitted to the computer when a switch is used
// Set the pin combinations that are used in any multi-pin inputs, using binary, and the number of unique combinations
// Set the pin numbers to be used for each input, and the number of pins used

// Codes for toggle switches; in each pair, the first value is 'off' and the second is 'on'
const String TOGGLECODES[][2] = { {"F30","F30"}, {"C500","C501"}, {"XXX","XXX"} };
const byte TOGGLEPINS[3] = {2,3,4};
const byte TOGGLEPINCOUNT = 3; 

// Codes for elevator trim
const String ELEVATORTRIMCODES[2] = {"C18", "C19"};
const byte ELEVATORTRIMPINS[2] = {18,19};
const byte ELEVATORTRIMPINCOUNT = 2;

// Codes for fuel tanks
const String FUELTANKCODES[3] = {"F041", "F042", "F043"};
const byte FUELTANKINPUTCODES[3] = {B00, B01, B10};  // Input codes that map the input from the fuel tank switch to the output codes
const byte FUELTANKINPUTCODECOUNT = 3;
const byte FUELTANKPINS[2] = {5,6};
const byte FUELTANKPINCOUNT = 2;

// Codes for flaps
const String FLAPCODES[4] = {"C17000", "C17033", "C17066", "C17100"};
const byte FLAPINPUTCODES[4] = {B001, B011, B010, B110};  // Input codes that map the input from the cowl flap switch to the output codes
const byte FLAPINPUTCODECOUNT = 4; 
const byte FLAPPINS[3] = {7,8,9};
const byte FLAPPINCOUNT = 3; 

// Codes for cowl flaps
const String COWLFLAPCODES[5] = {"C30000", "C30025", "C30050", "C30075", "C30100"};
const byte COWLFLAPINPUTCODES[5] = {B001, B011, B010, B110, B100};  // Input codes that map the input from the ignition to the output codes
const byte COWLFLAPINPUTCODECOUNT = 5; 
const byte COWLFLAPPINS[3] = {10,11,12};
const byte COWLFLAPPINCOUNT = 3; 

// Codes for engine settings
const String THROTTLECODE = "XX"; // Output for throttle settings 
const byte THROTTLEPIN = A0;
const String PROPELLERCODE = "YY"; // Output for propeller settings
const byte PROPELLERPIN = A1;
const String MIXTURECODE = "ZZ"; // Output for mixture settings
const byte MIXTUREPIN = A2;

// ----------------------------------------------------------------------------------------------
// Other constants
const int DELAY = 10;    // Time interval between checking the switch status (in milliseconds)
const bool FLIPONOFF = true;// Set to 'true' if the ON/OFF values of each input are reversed (1 = Off, 0 = On)
//const int THROTTLEVAL = 1;  // The numerical ID of the IGNITION code list
//const int PROPELLERVAL = 2;  // The numerical ID of the IGNITION code list
//const int MIXTUREVAL = 3;  // The numerical ID of the IGNITION code list
const int DEBOUNCE = 1;  // Amount of time in milliseconds to debounce
const int DEBOUNCETRIES = 1; // Number of times the debounce function should check the value
const int ANALOGTOLERANCE = 10; // Amount the engine setting change should be greater than to trigger a change (out of 1024)
const long ANALOGMIN = 30; 		// The analog signal that will be equal to 0
const long ANALOGMAX = 994; 	// The analog signal that will be equal to 100
const int ELEVATORTRIM_REDUCER = 1;	 // Number of detents for each print
const int ELEVATORTRIM_MULTIPLIER = 1;	// Number of prints for each detent
         
void setup() {
	// Open the serial connection
	Serial.begin(115200);
	//Serial.begin(9600);

	// Initialize the digital pins as input
	for (byte x = 0; x < TOGGLEPINCOUNT; x++){
		pinMode(TOGGLEPINS[x], INPUT_PULLUP);
	}
	for (byte x = 0; x < ELEVATORTRIMPINCOUNT; x++){
		pinMode(ELEVATORTRIMPINS[x], INPUT_PULLUP);
	}
	for (byte x = 0; x < FUELTANKPINCOUNT; x++){
		pinMode(FUELTANKPINS[x], INPUT_PULLUP);
	}
	for (byte x = 0; x < FLAPPINCOUNT; x++){
		pinMode(FLAPPINS[x], INPUT_PULLUP);
	}
	for (byte x = 0; x < COWLFLAPPINCOUNT; x++){
		pinMode(COWLFLAPPINS[x], INPUT_PULLUP);
	}
	
	// Initialize the analog pins
	pinMode(THROTTLEPIN, INPUT);
	pinMode(PROPELLERPIN, INPUT);
	pinMode(MIXTUREPIN, INPUT);

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
	static int elevatortrim_oldA = HIGH;
	static int elevatortrim_oldB = HIGH;
	int elevatortrim_newA = 0; 
	int elevatortrim_newB = 0; 
	static int fueltank_old = FUELTANKINPUTCODES[0];
	static int fueltank_new = 0;
	static int flap_old = FLAPINPUTCODES[0];
	static int flap_new = 0;
	static int cowlflap_old = COWLFLAPINPUTCODES[0];
	static int cowlflap_new = 0;
	static int throttle_old = 0;
	static int throttle_new = 0;
	static int propeller_old = 0;
	static int propeller_new = 0;
	static int mixture_old = 0;
	static int mixture_new = 0;
  
	// ---------- Read the analog inputs ---------------------------
	throttle_new = analogRead(THROTTLEPIN);
	propeller_new = analogRead(PROPELLERPIN);
	mixture_new = analogRead(MIXTUREPIN);
	
	// ---------- Read the rotary encoder inputs ---------------------------
	elevatortrim_newA = digitalRead(ELEVATORTRIMPINS[0]);
	elevatortrim_newB = digitalRead(ELEVATORTRIMPINS[1]);
	
	// ---------- Read the digital inputs ---------------------------
	// Toggle pins
	for (byte x = 0; x < TOGGLEPINCOUNT; x++){
		bitWrite(toggle_new, x, digitalRead(TOGGLEPINS[x]));
	}
	// Fuel tank pins
	for (byte x = 0; x < FUELTANKPINCOUNT; x++){
		bitWrite(fueltank_new, x, digitalRead(FUELTANKPINS[x]));
	}
	// Flap pins
	for (byte x = 0; x < FLAPPINCOUNT; x++){
		bitWrite(flap_new, x, digitalRead(FLAPPINS[x]));
	}
	// Cowl flap pins
	for (byte x = 0; x < COWLFLAPPINCOUNT; x++){
		bitWrite(cowlflap_new, x, digitalRead(COWLFLAPPINS[x]));
	}
	
	// ------- Look for changes and send codes -----------
	// Analog controls
	throttle_old = analogCheck(throttle_old, throttle_new, THROTTLECODE, ANALOGMIN, ANALOGMAX, ANALOGTOLERANCE);
	propeller_old = analogCheck(propeller_old, propeller_new, PROPELLERCODE, ANALOGMIN, ANALOGMAX, ANALOGTOLERANCE);
	mixture_old = analogCheck(mixture_old, mixture_new, MIXTURECODE, ANALOGMIN, ANALOGMAX, ANALOGTOLERANCE);
	//int analogCheck (int oldValue, int newValue, const int code, const int min, const int max, const int tolerance){
	
	// Elevator Trim
	encoderCheck(elevatortrim_oldA, elevatortrim_oldB, elevatortrim_newA, elevatortrim_newB, ELEVATORTRIMCODES); 

	// Toggle Switches
	if (toggle_old != toggle_new){
		toggleCheck(toggle_new, toggle_old, TOGGLEPINS, TOGGLEPINCOUNT, TOGGLECODES);
		toggle_old = toggle_new;
	}

	// Fuel Tanks
	if (fueltank_old != fueltank_new){
		multipinCheck(fueltank_new, FUELTANKPINS, FUELTANKPINCOUNT, FUELTANKCODES, FUELTANKINPUTCODES, FUELTANKINPUTCODECOUNT);
		fueltank_old = fueltank_new; 
	}

	// Flaps
	if (flap_old != flap_new){
		multipinCheck(flap_new, FLAPPINS, FLAPPINCOUNT, FLAPCODES, FLAPINPUTCODES, FLAPINPUTCODECOUNT);
		flap_old = flap_new;
	}
	
	// Cowl Flaps
	if (cowlflap_old != cowlflap_new){
		multipinCheck(cowlflap_new, COWLFLAPPINS, COWLFLAPPINCOUNT, COWLFLAPCODES, COWLFLAPINPUTCODES, COWLFLAPINPUTCODECOUNT);
		cowlflap_old = cowlflap_new;
	}		
	
	// Delay the loop
	delay(DELAY);

	// Ouput loop counter for testing and increment
	// Serial.println(loopCount);
	// static int loopCount = loopCount + 1; 
}

int analogCheck (int oldValue, int newValue, const String code, const int min, const int max, const int tolerance){
	int change = oldValue - newValue;
	if (abs(change) < tolerance){
		return oldValue; 
	} else {
		long output;
		if (newValue <= min){
			output = 0L; 
		} else if (newValue >= max){
			output = 100L; 
		} else {
			output = (newValue*101L)/1024L;
		}
		String outputString = String(code + output);
		Serial.println(outputString);
		return newValue;
	}
}

void encoderCheck(int &oldA, int &oldB, int newA, int newB, const String codes[]) {
  int result = 0;
  if (newA != oldA || newB != oldB) { //if the value of clkPin or the dtPin has changed
    if (oldA == HIGH && newA == LOW) {
      result = (oldB * 2 - 1);
    }
  }
  oldA = newA;
  oldB = newB;
  
  if (result < 0){
    Serial.println(codes[0]);
  }
  if (result > 0){
    Serial.println(codes[1]);
  }
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
