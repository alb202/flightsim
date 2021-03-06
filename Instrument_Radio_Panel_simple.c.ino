// -----------------------------------------------------------------------------------------------------
/*
attitude reference adjust	2			1x rotary encoder
altimeter (kohlsman)	2				1x rotary encoder
heading		2							1x rotary encoder
heading	bug  2							1x rotary encoder
airspeed adjust	2						1x rotary encoder
com/nav pull test 1							4x momentary button	
com/nav swap standby 1						4x momentary button
com/nav adjust 3							4x rotary encoder with button
swap com1/com2 1						N/A
transponder 6 (up/down for each digit using binary)	8x momentary button
obs nav1   2							1x rotary encoder
obs nav2	2							1x rotary encoder
adf heading select knob 2				1x rotary encoder
adf frequency 2							1x rotary encoder
DME knob 	3							1x rotary encoder with button
DME nav selector 1						1x momentary button
NAV/GPS selector 1						1x momentary button

totals = 24x rotary encoders, 18x momentary buttons
*/
// -----------------------------------------------------------------------------------------------------
// Set the codes that are transmitted to the computer when a switch is used

// Output codes for each toggle switch; in each pair, the first value is 'off' and the second is 'on'
const String TOGGLECODES[][2] = { {"F30","F30"}, {"C500","C501"}, {"XXX","XXX"} };

// Codes for elevator trim
const String ELEVATORTRIMCODES[2] = {"C18", "C19"};

// Codes for fuel tanks
const String FUELTANKCODES[3] = {"F041", "F042", "F043"};

// Codes for flaps
const String FLAPCODES[4] = {"C17000", "C17033", "C17066", "C17100"};

// Codes for cowl flaps
const String COWLFLAPCODES[5] = {"C30000", "C30025", "C30050", "C30075", "C30100"}; 

// Codes for engine settings
const String THROTTLECODE = "XX"; // Output for throttle settings 
const String PROPELLERCODE = "YY"; // Output for propeller settings 
const String MIXTURECODE = "ZZ"; // Output for mixture settings 

// -----------------------------------------------------------------------------------------------------
// Set the pin combinations that are used in any multi-pin inputs, using binary, and the number of unique combinations

// Input codes for fuel tanks
const byte FUELTANKINPUTCODES[3] = {B00, B01, B10};  // Input codes that map the input from the fuel tank switch to the output codes
const byte FUELTANKINPUTCODECOUNT = 3; 

// Input codes for flaps
const byte FLAPINPUTCODES[4] = {B001, B011, B010, B110};  // Input codes that map the input from the cowl flap switch to the output codes
const byte FLAPINPUTCODECOUNT = 4; 

// Input codes for cowl flaps
const byte COWLFLAPINPUTCODES[5] = {B001, B011, B010, B110, B100};  // Input codes that map the input from the ignition to the output codes
const byte COWLFLAPINPUTCODECOUNT = 5; 

// -----------------------------------------------------------------------------------------------------
// Set the pin numbers to be used for each input, and the number of pins used
// Pin numbers for the toggle switches
const byte TOGGLEPINS[] = {2,3,4};
const byte TOGGLEPINCOUNT = 3; 

// Pin numbers for the inputs from the fuel tanks
const byte FUELTANKPINS[] = {5,6};
const byte FUELTANKPINCOUNT = 2;

// Pin numbers for the inputs from the elevator trim
const byte ELEVATORTRIMPINS[] = {18,19};
const byte ELEVATORTRIMPINCOUNT = 2;

// Pin numbers for the inputs from the cowl flaps
const byte FLAPPINS[] = {7,8,9};
const byte FLAPPINCOUNT = 3; 

// Pin numbers for the inputs from the flaps
const byte COWLFLAPPINS[] = {10,11,12};
const byte COWLFLAPPINCOUNT = 3; 

// Pin numbers for the inputs from the flaps
const byte THROTTLEPIN = A0;
const byte PROPELLERPIN = A1;
const byte MIXTUREPIN = A2;

// ----------------------------------------------------------------------------------------------
// Other constants
const int DELAY = 100;    // Time interval between checking the switch status (in milliseconds)
const bool FLIPONOFF = true;// Set to 'true' if the ON/OFF values of each input are reversed (1 = Off, 0 = On)
const int TOGGLEVAL = 0;  // The numerical ID of the toggle code list
const int FUELTANKVAL = 1;  // The numerical ID of the IGNITION code list
const int ELEVATORTRIMVAL = 2;  // The numerical ID of the IGNITION code list
const int FLAPVAL = 3;  // The numerical ID of the IGNITION code list
const int COWLFLAPVAL = 4;  // The numerical ID of the IGNITION code list
const int THROTTLEVAL = 5;  // The numerical ID of the IGNITION code list
const int PROPELLERVAL = 6;  // The numerical ID of the IGNITION code list
const int MIXTUREVAL = 7;  // The numerical ID of the IGNITION code list
const int DEBOUNCE = 5;  // Amount of time in milliseconds to debounce
const int DEBOUNCETRIES = 1; // Number of times the debounce function should check the value
const int ENGINETOLERANCE = 10; // Amount the engine setting change should be greater than to trigger a change (out of 1024)
const long ANALOGMIN = 30; 
const long ANALOGMAX = 994; 
const int ELEVATORTRIM_REDUCER = 1;	 // Number of detents for each print
const int ELEVATORTRIM_MULTIPLIER = 1;	// Number of prints for each detent

// initialize the loop counter for testing
//int loopCount = 0;
          
void setup() {
	// Open the serial connection
	//Serial.begin(115200);
	Serial.begin(9600);

	// Initialize the digital pins as input
	for (byte x = 0; x < TOGGLEPINCOUNT; x++){
		pinMode(TOGGLEPINS[x], INPUT_PULLUP);
	}
	for (byte x = 0; x < ELEVATORTRIMPINCOUNT; x++){
		pinMode(ELEVATORTRIMPINS[x], INPUT);
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
	// -------------------------------------
	// Starting values for inputs
	static int toggle_old = flipOneBit(0); 
	static int toggle_new = 0; 
	static int elevatortrim_old = 0;
	static int elevatortrim_newA = 0;
	static int elevatortrim_newB = 0;
	static int elevatortrim_pos = 0;
	static int fueltank_old = FUELTANKINPUTCODES[0];
	static int fueltank_new = 0;
	static int flap_old = FLAPINPUTCODES[0];
	static int flap_new = 0;
	static int cowlflap_old = COWLFLAPINPUTCODES[0];
	static int cowlflap_new = 0;
	static int throttle_old = 0;
	int throttle_new = 0;
	static int propeller_old = 0;
	int propeller_new = 0;
	static int mixture_old = 0;
	int mixture_new = 0;
  
	throttle_new = analogRead(THROTTLEPIN);
	propeller_new = analogRead(PROPELLERPIN);
	mixture_new = analogRead(MIXTUREPIN);
	
	// Get the values for each pin and write it as a bit (0/1)
	elevatortrim_newA = digitalRead(ELEVATORTRIMPINS[0]);
	elevatortrim_newB = digitalRead(ELEVATORTRIMPINS[1]);
	
	for (byte x = 0; x < TOGGLEPINCOUNT; x++){
		bitWrite(toggle_new, x, digitalRead(TOGGLEPINS[x]));
	}

	for (byte x = 0; x < FUELTANKPINCOUNT; x++){
		bitWrite(fueltank_new, x, digitalRead(FUELTANKPINS[x]));
	}

	for (byte x = 0; x < FLAPPINCOUNT; x++){
		bitWrite(flap_new, x, digitalRead(FLAPPINS[x]));
	}

	for (byte x = 0; x < COWLFLAPPINCOUNT; x++){
		bitWrite(cowlflap_new, x, digitalRead(COWLFLAPPINS[x]));
	}
	
	// Look for changes in each input and send code if value has changed
	
	// ------ Engine Controls -------
	throttle_old = analogCheck(throttle_old, throttle_new, THROTTLEVAL);
	propeller_old = analogCheck(propeller_old, propeller_new, PROPELLERVAL);
	mixture_old = analogCheck(mixture_old, mixture_new, MIXTUREVAL);
	
	// ------ Elevator Trim -------
	elevatortrim_old = encoderCheck(elevatortrim_old, elevatortrim_newA, elevatortrim_newB, elevatortrim_pos, ELEVATORTRIM_REDUCER, ELEVATORTRIM_MULTIPLIER);
	
	// ------ Toggle Switches -------
	toggle_old = switchCheck(toggle_old, toggle_new, TOGGLEVAL); 

	// ------ Fuel Tanks -------
	fueltank_old = switchCheck(fueltank_old, fueltank_new, FUELTANKVAL);

	// ------ Flaps -------
	flap_old = switchCheck(flap_old, flap_new, FLAPVAL);

	// ------ Cowl Flaps -------
	cowlflap_old = switchCheck(cowlflap_old, cowlflap_new, COWLFLAPVAL);
	
	// Delay the loop
	delay(DELAY);

	// Ouput loop counter for testing and increment
	//Serial.println(loopCount);
	//loopCount = loopCount + 1; 
}

int analogCheck (int oldValue, int newValue, int switchType){
	int change = oldValue - newValue;
	if (abs(change) < ENGINETOLERANCE){
		return oldValue; 
	} else {
		long output;
		if (newValue <= ANALOGMIN){
			output = 0L; 
		} else if (newValue >= ANALOGMAX){
			output = 100L; 
		} else {
			output = (newValue*101L)/1024L;
		}
		String outputString;
		switch (switchType) {
			case THROTTLEVAL: {
				outputString = String(THROTTLECODE + output);
			}
			break;
			case PROPELLERVAL: {
				outputString = String(PROPELLERCODE + output);
			}
			break;
			case MIXTUREVAL: {
				outputString = String(MIXTURECODE + output);
			}
			break;
		}
		Serial.println(outputString);
		return newValue;
	}
}

int encoderCheck (int oldValue, int newValueA, int newValueB, int& elevatortrim_pos, int reduce, int multiply) {
	// If the first connector has changed ...
	if ((oldValue == LOW) && (newValueA == HIGH)) {
		// If the second connector is low ...
		if (newValueB == LOW){
			// Decrement the position by 1
			elevatortrim_pos--;
			// If the position % the reducer number is 0, then print the code
			if (elevatortrim_pos % reduce == 0) {
				// Call the print function
				printEncoderCode(ELEVATORTRIMCODES[0], multiply);
			}	
		}
		// If the second connector is high ...
		if (newValueB == HIGH){
			// Increment the position by 1
			elevatortrim_pos++;
			// If the position % the reducer number is 0, then print the code
			if (elevatortrim_pos % reduce == 0) {
				// Call the print function
				printEncoderCode(ELEVATORTRIMCODES[1], multiply);
			}
		}
		return newValueA; // Set the new connector A status to the old connector A status
	} else {
		return oldValue; 
	}
}

void printEncoderCode(String output, int multi){
	// For the number of times of the multiplier, output the code
	for(int i = 0; i < multi; i++){
		Serial.print (output);
		Serial.print ("\n");
	}
}

void checkMultipin(int newValue, const byte PINS[], byte PINCOUNT, const String CODES[], const byte INPUTCODES[], byte INPUTCODECOUNT){
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

void checkToggles(int newValue, int oldValue, const byte PINS[], byte PINCOUNT, const String CODES[][2]){
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

int switchCheck (int oldValue, int newValue, int switchType) {
  // Check if new value is different than previous value
  if(newValue == oldValue) {
     // If it hasn't changed, return the old value
    return oldValue;
    } else {
    // Check if the difference is in the toggle or a multi-pin switch
    switch (switchType) {
		case TOGGLEVAL: {
			checkToggles(newValue, oldValue, TOGGLEPINS, TOGGLEPINCOUNT, TOGGLECODES);
		}
		break;
		case FUELTANKVAL: {
			checkMultipin(newValue, FUELTANKPINS, FUELTANKPINCOUNT, FUELTANKCODES, FUELTANKINPUTCODES, FUELTANKINPUTCODECOUNT);
		}
		break;
		case FLAPVAL: {
			checkMultipin(newValue, FLAPPINS, FLAPPINCOUNT, FLAPCODES, FLAPINPUTCODES, FLAPINPUTCODECOUNT);
		}
		break;
		case COWLFLAPVAL: {
			checkMultipin(newValue, COWLFLAPPINS, COWLFLAPPINCOUNT, COWLFLAPCODES, COWLFLAPINPUTCODES, COWLFLAPINPUTCODECOUNT);
		}
		break;
	}
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
