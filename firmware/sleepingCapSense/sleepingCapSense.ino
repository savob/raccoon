// Written for use on the ATtiny85

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "tinyTouch.h"

const int eyesOnTime = 3000; // Time eyes are on in ms after touched
const byte eyesPin = 4;

void setup() {

  for (int x = 0; x < 5; x++) {
    pinMode(x, OUTPUT); digitalWrite(x, LOW);
  }

//  CLKPR = _BV(CLKPCE);
//  CLKPR = 0;    // set clock prescaler to 1 (attiny 25/45/85/24/44/84/13/13A)

  tinytouch_init();


  // Set sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  setup_watchdog();

  // Light on to confirm power on
  digitalWrite(eyesPin, HIGH);
  delay(eyesOnTime);
  digitalWrite(eyesPin, LOW);

    // Trying to reduce power use
  ACSR |= _BV(ACD); // Disable analog comparator
  DIDR0 = _BV(AIN1D) | _BV(AIN0D); // Disable digital input buffers on analog pins (we don't need digital in)
  PRR = _BV(PRTIM1) | _BV(PRTIM0) | _BV(PRUSI) | _BV(PRADC);
}

void loop() {

  // Sleep cycle
  ADCSRA &= ~(_BV(ADEN)); // Disable ADC
  __power_all_disable();
  sleep_mode();
  sleep_disable();
  __power_all_enable();
  ADCSRA |= _BV(ADEN); // Enable ADC
  delay(10); // Needed for ADC warmup/stabilization

  // Check for touch and light up
  if (tinytouch_sense() == tt_push) {
    digitalWrite(eyesPin, HIGH);
    delay(eyesOnTime);
    digitalWrite(eyesPin, LOW);
  }
}

void setup_watchdog() {
  MCUSR &= ~_BV(WDRF); // Reset WDT

  // Start timed sequence to set
  WDTCR |= _BV(WDCE) | _BV(WDE);
  
  // Set new watchdog timeout value 
//  WDTCR = _BV(WDCE) | _BV(WDP3) | _BV(WDP0); // 8 seconds
  WDTCR = _BV(WDCE) | _BV(WDP2) | _BV(WDP1) | _BV(WDP0); // 2 seconds
//  WDTCR = _BV(WDCE) | _BV(WDP2) | _BV(WDP1); // 1 second
//  WDTCR = _BV(WDCE) | _BV(WDP2) | _BV(WDP0); // 0.5 seconds
  
  WDTCR |= _BV(WDIE);
}

ISR (WDT_vect) {
  // Do nothing, just escape sleep
}
