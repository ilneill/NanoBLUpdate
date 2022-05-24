// Simple Watchdog Timer tester.
// v1.0, May 2022, ILN 2020

// A countdown to the watchdog reset being disabled... then the watchdog should restart the Arduino.
// Use the WDT hardware test disable if the countdown is set too low and prevents the writing of a new sketch.

#include <Watchdog.h>         // A simple watchdog library (by Peter Polidoro, installed via Library Manager).

// Arduino I/O pin defines.
#define TSTENPIN 2            // Tie this pin LOW at start-up to hardware disable the Watchdog Timer test.
#define HBLEDPIN LED_BUILTIN  // Usually digital I/O pin 13.

// Code loop job defines.
#define JOB1CYCLE 1000        // Job 1 execution cycle: 1.000s - Countdown...
#define JOB2CYCLE 500         // Job 2 execution cycle: 0.500s - Toggle the heartbeat LED and reset the Watchdog Timer.

// Watchdog Timer test hardware enabled.
bool wdtHwTstEnable;
// Watchdog Timer test countdown.
#define WDTCOUNT 10

// Watchdog initialisation.
Watchdog cerberous;

void setup() {
  // Initialise the LED pins.
  pinMode(TSTENPIN, INPUT_PULLUP);       // A hardware disable for the Watchdog Timer test.
  pinMode(HBLEDPIN, OUTPUT);             // Builtin LED, usually on pin 13.
  // Start the serial port.
  Serial.begin(115200);                  // As fast as we can go!
  while(!Serial);                        // Wait for the serial I/O to start.
  // Check the Watchdog Timer test hardware enable.
  wdtHwTstEnable = digitalRead(TSTENPIN); // High = WDT test enabled, LOW = WDT test disabled.
  // Start the watchdog.
  cerberous.enable();                    // The default WDT timeout period is 1000ms.
  // Start up message.
  if (wdtHwTstEnable) {
    Serial.print("\n\nStarting countdown: ");
  }
  else {
    Serial.print("\n\nWatchdog Timer test disabled!");
    Serial.print("\nJust counting down: ");
  }
}

void loop() {
  // Initialise the heartbeat status variable.
  static bool hbStatus = LOW;
  // Enable the watchdog reset.
  static bool wdtReset = true;
  // Countdown variable.
  static byte countdown = WDTCOUNT;
  // Record the current time. When a single timeNow is used for all jobs it ensures they are synchronised.
  unsigned long timeNow = millis();
  // Job variables. Set to timeNow so that jobs do not start immediately - this allows things to settle.
  static unsigned long timeMark1 = timeNow; // Last time Job 1 was executed.
  static unsigned long timeMark2 = timeNow; // Last time Job 2 was executed.
  // Job 1 - Countdown...
  if (timeNow - timeMark1 >= JOB1CYCLE) {
    timeMark1 = timeNow;
    // Do something...
    // Countdown until we let the dog go, or not.
    if (countdown > 0) {
      Serial.print(countdown);
      Serial.print("... ");
      countdown--;
    }
    else {
      if (wdtHwTstEnable) {
        Serial.print("\nBark!");
        wdtReset = false;
      }
      else {
        Serial.print("\nJust counting down: ");
        countdown = WDTCOUNT;
      }
    }
  }
  // Job 2 - Toggle the heartbeat LED and reset the watchdog timer.
  if (timeNow - timeMark2 >= JOB2CYCLE) {
    timeMark2 = timeNow;
    // Do something...
    // Toggle the heartbeat status.
    hbStatus = !hbStatus;
    digitalWrite(HBLEDPIN, hbStatus);
    // Reset the Watchdog Timer.
    if (wdtReset) {
      cerberous.reset();
    }
  }
}

// EOF
