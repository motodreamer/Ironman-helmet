#define VERSION "3.1.0.2"



#include "DFRobotDFPlayerMini.h"
#include "config.h"
#include <VarSpeedServo.h>
#include <OneButton.h>
#include <SoftwareSerial.h>

SoftwareSerial mp3Serial(8, 7); // RX, TX pins for DFPlayer
DFRobotDFPlayerMini mp3;



// Declare servo objects
VarSpeedServo servo1; // create servo object to control servo 1
VarSpeedServo servo2; // create servo object to control servo 2

#ifdef WALSH85
VarSpeedServo servo3; // create servo object to control servo 3 (Walsh85 chin Control)
#endif

#ifdef MISSILE
VarSpeedServo servo4; // create servo object to control servo 3
VarSpeedServo servo5; // create servo object to control servo 4

// Define object for the missile button
OneButton missileButton = OneButton(MISSILE_BUTTON_PIN, true, true);

// State of the missile bay 1 = open, 0 = closed
#define MISSILE_BAY_CLOSED 0
#define MISSILE_BAY_OPEN 1
int missileBayCurMode = MISSILE_BAY_OPEN; // Keep track if the missile bay is open or closed
#endif

#ifndef MISSILE
boolean auxLedState = false; // Keeps track of the state of the LED on = true, off = false
#endif



// Define object for primary button to handle 
// multiple button press features:
// 1. Single Tap
// 2. Double Tap
// 3. Long Press
OneButton primaryButton = OneButton(BUTTON_PIN, true, true);

// State of the faceplate 1 = open, 0 = closed
#define FACEPLATE_CLOSED 0
#define FACEPLATE_OPEN 1
int facePlateCurMode = FACEPLATE_CLOSED; // Keep track if the faceplate is open or closed

// State of the LED eyes 1 = on, 2 = off
#define LED_EYES_OFF 0
#define LED_EYES_ON 1

// State of the LED eyes for dimming/brightening 1 = brighten, 2 = dim
#define LED_EYES_DIM_MODE 0
#define LED_EYES_BRIGHTEN_MODE 1

int ledEyesCurMode = LED_EYES_DIM_MODE; // Keep track if we're dimming or brightening
int ledEyesCurPwm = 0; // Tracking the level of the LED eyes for dim/brighten feature
const int ledEyesIncrement = 15; // Define the increments to brighten or dim the LED eyes


void simDelay(long period){
  long delayMillis = millis() + period;
  while (millis() <= delayMillis)
  {
    int x = 0; // dummy variable, does nothing
  }
}

/**
 * Simulate the eyes slowly blinking until fully lit
 */ 

void setLedEyes(int pwmValue);
void setAuxLed();
void auxLedOn();

void movieblink() {
    Serial.println(F("Initializing MARK1 .."));

    // pause for effect...
    simDelay(500);

    int maxBrightness = 200;
    int lowValue = 50;
    int mediumValue = 150;
    int delayInterval[] = { 200, 180, 100, 60, 30 };
    int delayVal = 0;

    // First dramatic fade-in and fade-out
    for (int i = 0; i <= maxBrightness; i += 10) {
        setLedEyes(i);
        setAuxLed();
        delayVal = delayInterval[0] / (maxBrightness / 10);
        simDelay(delayVal);
    }
    for (int i = maxBrightness; i >= 0; i -= 10) {
        setLedEyes(i);
        setAuxLed();
        delayVal = delayInterval[0] / (maxBrightness / 10);
        simDelay(delayVal);
    }

    simDelay(300);


  // Second subtle blink with a quick fade-out
  for (int i = 0; i <= lowValue; i++) {
    setLedEyes(i);
    setAuxLed();
    delayVal = delayInterval[1] / lowValue;
    simDelay(delayVal);
  }
  setLedEyes(0);
  setAuxLed();
  simDelay(delayInterval[1]);

  // Third medium intensity fade-in and fade-out
  for (int i = 0; i <= mediumValue; i += 5) {
    setLedEyes(i);
    setAuxLed();
    delayVal = delayInterval[2] / (mediumValue / 5);
    simDelay(delayVal);
  }
  for (int i = mediumValue; i >= 0; i -= 5) {
    setLedEyes(i);
    setAuxLed();
    delayVal = delayInterval[2] / (mediumValue / 5);
    simDelay(delayVal);
  }

  simDelay(200); // Short pause

  // Fourth quick high-intensity blink
  setLedEyes(maxBrightness);
  setAuxLed();
  simDelay(delayInterval[3]);
  setLedEyes(0);
  setAuxLed();
  simDelay(delayInterval[3]);

  // Final dramatic on effect
  for (int i = 0; i <= maxBrightness; i += 15) {
    setLedEyes(i);
    setAuxLed();
    delayVal = delayInterval[4] / (maxBrightness / 15);
    simDelay(delayVal);
  }
  
  // Hold at full brightness
  setLedEyes(maxBrightness);
  auxLedOn();
}

/*
 * Simulate LED eyes slowly brightening until fully lit
 */
 void fadeEyesOn(){
  ledEyesCurMode = LED_EYES_BRIGHTEN_MODE;

  // loop until fully lit
  while (ledEyesCurPwm < 255){
    setLedEyes(ledEyesCurPwm);
  
    simDelay(200);
    ledEyesBrighten();
  }  
 }


/**
 * Method to open face plate
 */
 void facePlateOpen(){
  Serial.println(F("Servo Up!")); 

  // Re-attach the servos to their pins
  servo1.attach(SERVO1_PIN, PWM_LOW, PWM_HIGH);
  servo2.attach(SERVO2_PIN, PWM_LOW, PWM_HIGH);
  

  #ifdef WALSH85
  servo3.attach(SERVO3_PIN, PWM_LOW, PWM_HIGH);
  #endif

  // Send data to the servos for movement
    
  servo1.write(SERVO1_OPEN_POS, SERVO1_OPEN_SPEED); 
  servo2.write(SERVO2_OPEN_POS, SERVO2_OPEN_SPEED);
  
  #ifdef WALSH85
  simDelay(200);
  servo3.write(SERVO3_OPEN_POS, CHIN_OPEN_SPEED);
  //simDelay(1000); // wait doesn't wait long enough for servos to fully complete...
  #endif
  
  simDelay(1000); // wait doesn't wait long enough for servos to fully complete...

  // Detach so motors don't "idle"
  #if (SERVO1_DISABLE_AFTER_OPEN)
  servo1.detach();
  #endif

  #if (SERVO1_DISABLE_AFTER_OPEN)
  servo2.detach();
  #endif

  #ifdef WALSH85
  servo3.detach();
  #endif

  facePlateCurMode = FACEPLATE_OPEN;
 }

 /**
  * Method to close face plate
  */
 void facePlateClose(){
  Serial.println(F("Servo Down"));  

  // Re-attach the servos to their pins
  servo1.attach(SERVO1_PIN, PWM_LOW, PWM_HIGH);
  servo2.attach(SERVO2_PIN, PWM_LOW, PWM_HIGH);

  #ifdef WALSH85
  servo3.attach(SERVO3_PIN, PWM_LOW, PWM_HIGH);
  #endif

  // Send data to the servos for movement 

//  #ifdef WALSH85
//  servo3.write(SERVO3_CLOSE_POS, CHIN_CLOSE_SPEED);
//  simDelay(0); // Delay to allow chin to fully close before Faceplate closes
//  #endif
  
  servo1.write(SERVO1_CLOSE_POS, SERVO1_CLOSE_SPEED);
  servo2.write(SERVO2_CLOSE_POS, SERVO2_CLOSE_SPEED);

  simDelay(500); // wait doesn't wait long enough for servos to fully complete...

  // Detach so motors don't "idle"
  servo1.detach();
  servo2.detach();

  #ifdef WALSH85
  servo3.detach();
  #endif

  facePlateCurMode = FACEPLATE_CLOSED;
 }

#ifdef MISSILE
/**
 * Method to open the missile bay
*/
 void missileBayOpen(){
  Serial.println(F("Missile bay opening..."));
  servo4.attach(SERVO4_PIN, PWM_LOW, PWM_HIGH);
  servo5.attach(SERVO5_PIN, PWM_LOW, PWM_HIGH);

  servo4.write(SERVO4_OPEN_POS, MISSILE_BAY_OPEN_SPEED);
  simDelay(MISSILE_BAY_DELAY);
  servo5.write(SERVO5_OPEN_POS, MISSILE_OPEN_SPEED);

  simDelay(MISSILE_BAY_DELAY);

  servo4.detach();
  servo5.detach();

  missileBayCurMode = MISSILE_BAY_OPEN;
 }

/**
 * Method to close the missile bay
*/
 void missileBayClose(){
  Serial.println(F("Missile bay closing..."));
  servo4.attach(SERVO4_PIN, PWM_LOW, PWM_HIGH);
  servo5.attach(SERVO5_PIN, PWM_LOW, PWM_HIGH);

  servo5.write(SERVO5_CLOSE_POS, MISSILE_CLOSE_SPEED);
  simDelay(1000);
  servo4.write(SERVO4_CLOSE_POS, MISSILE_BAY_CLOSE_SPEED);

  simDelay(1000);

  servo4.detach();
  servo5.detach();

  missileBayCurMode = MISSILE_BAY_CLOSED;
 }
 #endif

/**
 * Set the brightness of the LED eyes
 * 
 * @param[out] pwmValue - the PWM value (0-255) for the LED brightness
 */
void setLedEyes(int pwmValue){
  // Set the right and left eye LEDs with the given pwmValue
  analogWrite(RIGHT_EYE_PIN, pwmValue);
  analogWrite(LEFT_EYE_PIN, pwmValue);
  

  // Invert the behavior for BLUE_LED_PIN (opposite of pwmValue)
  int blueLedValue = 255 - pwmValue;
  analogWrite(BLUE_LED_PIN, blueLedValue);

  ledEyesCurPwm = pwmValue;
}

/**
 * Method to turn on LED eyes
 */
void ledEyesOn(){
  Serial.println(F("HUD ACTIVE.."));
  
  setLedEyes(255);
  
  ledEyesCurMode = LED_EYES_DIM_MODE;
}

/**
 * Method to turn off LED eyes
 */
void ledEyesOff(){
  Serial.println(F("HUD DEACTIVATED.."));
  
  setLedEyes(0);

  ledEyesCurMode = LED_EYES_BRIGHTEN_MODE;
}

/**
 * Method to turn LED eyes on/off
 */
void ledEyesOnOff(){
  // LED eyes stay off when faceplate is open
  if(facePlateCurMode == FACEPLATE_CLOSED){
    if (ledEyesCurPwm > 0){
      ledEyesOff();
    } else {
      ledEyesOn();
    }
  }

  // Make sure LED on pin 11 stays off
  digitalWrite(11, LOW);  // Turn off LED at pin 11
}

void ledEyesDim(){
  Serial.println(F("Dimming LED eyes..."));

  ledEyesCurPwm = ledEyesCurPwm - ledEyesIncrement; // Decrease the brightness

  // Make sure we don't go over the limit
  if(ledEyesCurPwm <= 0){
    ledEyesCurPwm = 0;
  }

  // Ensure LED at pin 11 stays off
  digitalWrite(11, LOW);  // Turn off LED at pin 11
}

void ledEyesBrighten(){
  Serial.println(F("Brightening LED eyes..."));

  ledEyesCurPwm = ledEyesCurPwm + ledEyesIncrement; // Increase the brightness

  // Make sure we don't go over the limit
  if(ledEyesCurPwm >= 255){
    ledEyesCurPwm = 255;
  }

  // Ensure LED at pin 11 stays off
  digitalWrite(11, LOW);  // Turn off LED at pin 11
}

//Function for combact mode LED


void ledcombact() {
    // Always ensure LED at D11 remains off
    digitalWrite(11, LOW); // Forcefully turn off the LED at D11
    Serial.println(F("LED at D11 forced OFF."));
    static bool auxLedState = false;

    // Toggle the auxiliary LED at D12
    if (auxLedState) {
        analogWrite(RED_LED_PIN, 0); // Turn off the auxiliary LED at D12
        auxLedState = false;
        Serial.println(F("Auxiliary LED at D12 turned OFF."));
    } else {
        analogWrite(RED_LED_PIN, 255); // Set auxiliary LED at D12 to full brightness
        auxLedState = true;
        Soundfx4();
        Serial.println(F("Auxiliary LED at D12 turned ON at full brightness."));
    }

    // Maintain the LED eyes' current state (Ensure the current PWM state for the LED eyes)
    setLedEyes(ledEyesCurPwm);
    // Add a delay for timing
    simDelay(5);
}

/*
 * Sets the Aux LED
 */
void setAuxLed(){
#ifndef MISSILE
  if (AUX_LED_ENABLED) {
    if (auxLedState == false){
      auxLedOn();
    } else {
      auxLedOff();
    }
  } else {
    auxLedOff();
  }
#endif
}

/*
 * Turn the Aux LED on
 */
void auxLedOn(){
#ifndef MISSILE
  digitalWrite(AUX_LED_PIN, HIGH);
  auxLedState = true;
#endif
}

/*
 * Turn the Aux LED off
 */
void auxLedOff(){
#ifndef MISSILE
  digitalWrite(AUX_LED_PIN, LOW);
  auxLedState = false;
#endif
}

/**
 * Method to run sequence of sppecial effects when system first starts or sets up
 */
void startupFx(){
  //facePlateClose();
#ifdef MISSILE
  missileBayClose(); // Start out with the missile bay in the closed position
#endif


  //facePlateClose();

  switch(SETUP_FX){
    case EYES_NONE:
      ledEyesOn();
      auxLedOn();
      break;
    case EYES_MOVIE_BLINK:
      movieblink();
      break;
    case EYES_FADE_ON:
      fadeEyesOn();
      auxLedOn();
      break;
  }


}

//Function for LED eyes dimming

void ledEyesFade(){
  if(ledEyesCurPwm == 255){
    ledEyesCurMode = LED_EYES_DIM_MODE;
  } else if(ledEyesCurPwm == 0){
    ledEyesCurMode = LED_EYES_BRIGHTEN_MODE;
  }
  
  if(ledEyesCurMode == LED_EYES_BRIGHTEN_MODE){
    ledEyesBrighten();
  } else {
    ledEyesDim();
  }

  setLedEyes(ledEyesCurPwm);

  simDelay(200);
}

// Initialise DFPlayer once
void initDFPlayer() {
  mp3Serial.begin(9600);

  if (!mp3.begin(mp3Serial)) {
    Serial.println(F("DFPlayer initialization failed! Check wiring and SD card."));
    while (true); // Stay here if initialization fails
  }

  mp3.volume(25);  // Set default volume
  Serial.println(F("DFPlayer ready."));
}

// --- Sound Functions ---
// Open sfx
void Soundfx1() {
  mp3.volume(35);
  mp3.play(2);   // Track 2
}

// Close sfx
void Soundfx2() {
  mp3.volume(30);
  mp3.play(3);   // Track 3
}

// Booting sfx
void Soundfx3() {
  mp3.volume(25);
  mp3.play(1);   // Track 1
}

// Combat mode
void Soundfx4() {
  mp3.volume(30);
  mp3.play(4);   // Track 4
}


void facePlateOpenFx(){
  ledEyesOff();
  Soundfx1();        // Play open sound immediately
  simDelay(600);    // <-- Add delay here before servos move
  facePlateOpen();   // Then move servos
}

/**
 * Method to execute special effects when the faceplate closes
 */
void facePlateCloseFx(){
  ledEyesOn();
  Soundfx2();        // Play close sound immediately
  simDelay(1000);    // <-- Add delay here before servos move
  facePlateClose();  // Then move servos

  switch(EYES_FX){
    case EYES_NONE:
      ledEyesOn();
      auxLedOn();
      break;
    case EYES_MOVIE_BLINK:
      movieblink();
      break;
    case EYES_FADE_ON:
      fadeEyesOn();
      auxLedOn();
      break;
  }
}

/**
 * Handle faceplate special effects
 */
void facePlateFx(){
  if (facePlateCurMode == FACEPLATE_OPEN){
    facePlateCloseFx();
  } else {
    facePlateOpenFx();
  }
}

#ifdef MISSILE
void missileOpenFx(){
  missileBayOpen();
}

void missileCloseFx(){
  missileBayClose();
}

void missileFx(){
  if (missileBayCurMode == MISSILE_BAY_OPEN){
    missileCloseFx();
  } else {
    missileOpenFx();
  }
}
#endif

/**
 * Event handler for when the primary button is tapped once
 */
void handlePrimaryButtonSingleTap(){
  facePlateFx();
}

/**
 * Event handler for when the primary button is double tapped
 */
void handlePrimaryButtonDoubleTap(){
  ledEyesOnOff();
}

/**
 * Event handler for when the primary button is pressed and held
 */
void handlePrimaryButtonLongPress(){
  ledEyesFade();
   
  
}

#ifdef MISSILE
void handleMissileButtonSingleTap(){
  missileFx();
}
#endif

/**
 * Event handler for when the primary button is pressed multiple times
*/
void handlePrimaryButtonMultiPress(){
  ledcombact();
  }


/**
 * Initializes the primary button for multi-functions
 */
void initPrimaryButton(){
  primaryButton.attachClick(handlePrimaryButtonSingleTap);
  primaryButton.attachDoubleClick(handlePrimaryButtonDoubleTap);
  primaryButton.attachDuringLongPress(handlePrimaryButtonLongPress);
  primaryButton.attachMultiClick(handlePrimaryButtonMultiPress);
}

#ifdef MISSILE
/**
 * Initializes the missile button for multi-functions
 */
void initMissileButton(){
  missileButton.attachClick(handleMissileButtonSingleTap);
}
#endif

/**
 * Monitor for when the primary button is pushed
 */
void monitorPrimaryButton(){
  primaryButton.tick();
}

#ifdef MISSILE
/**
 * Monitor for when the missile button is pushed
 */
void monitorMissileButton(){
  missileButton.tick();
}
#endif

/**
 * Initialization method called by the Arduino library when the board boots up
 */
void setup() {
  // Initialize serial monitor
  Serial.begin(9600);
  simDelay(1000); // Give the serial service time to stabilize

  Serial.print(F("Initializing Iron Man Helmet sequence: "));
  Serial.println(VERSION);

  // --- Initialize DFPlayer Mini only once ---
  mp3Serial.begin(9600);   // Begin software serial for DFPlayer
  if (!mp3.begin(mp3Serial)) {
    Serial.println(F("DFPlayer initialization failed! Check wiring and SD card."));
    while (true); // Halt execution if DFPlayer not found
  }
  mp3.volume(25); // Set default volume

  // --- Boot effects ---
  Soundfx3();      // Play boot-up sound
  startupFx();     // Run LED/servo startup effects

  // --- Initialize primary button ---
  initPrimaryButton();

#ifdef MISSILE
  initMissileButton(); // Initialize missile button if enabled
#else
  pinMode(AUX_LED_PIN, OUTPUT); // Aux LED output
#endif
}



/**
 * Main program exeucution
 * This method will run perpetually on the board
 */
void loop() {
  monitorPrimaryButton(); // Since all features currently are tied to the one button...

#ifdef MISSILE
  monitorMissileButton(); // Monitor when the missile button is pushed...
#endif

  // Room for future features ;)
}
