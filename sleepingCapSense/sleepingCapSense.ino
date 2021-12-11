// Written for use on the ATtiny85

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "tinyTouch.h"


void setup() {

  for (int x = 0; x < 5; x++) {
    pinMode(x, OUTPUT); digitalWrite(x, LOW);
  }

  CLKPR = _BV(CLKPCE);
  CLKPR = 0;    // set clock prescaler to 1 (attiny 25/45/85/24/44/84/13/13A)

  tinytouch_init();

  // Trying to reduce power use
  ACSR &= ~(_BV(ACD)); // Disable analog comparator
  DIDR0 = _BV(AIN1D) | _BV(AIN0D); // Disable digital input buffers on analog pins (we don't need digital in)

  // Set sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  setup_watchdog();
}



void loop() {

  ADCSRA &= ~(_BV(ADEN)); // Disable ADC
  sleep_enable();
  sleep_mode();
  sleep_disable();
  power_all_enable();

  ADCSRA |= _BV(ADEN); // Disable ADC
  delay(10);
  if (tinytouch_sense() == tt_push) {
    PORTB |= _BV(PB4); // Blink LED on touch event
    delay(3000);
    PORTB &= ~(_BV(PB4));
  }


}

ISR (WDT_vect) {
  // Do nothing, just escape sleep
}

void setup_watchdog() {
  MCUSR &= ~(1 << WDRF); // Reset WDT

  // Start timed sequence to set
  WDTCR |= _BV(WDCE) | _BV(WDE);
  // Set new watchdog timeout value (2 seconds)
  WDTCR = _BV(WDCE) | _BV(WDP2) | _BV(WDP1) | _BV(WDP0);
  WDTCR |= _BV(WDIE);
}
