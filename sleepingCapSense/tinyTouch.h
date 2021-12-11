/*
  v0.1  06/10/2013

  TinyTouchLibrary Example

  Hardware:
  - Attiny 13A/25/45/85
  - LED on PB4
  - Touchbutton on PB3
*/

#include <avr/io.h>
#include <util/delay.h>

//////////////////////////////////////////////////////////////////////////
//
// User definable settings
//
//////////////////////////////////////////////////////////////////////////

// Define upper and lower threshold for the touch sensing. You may have to
// change these depending on the geometry of your touch button.
// Setting the "on" value lower will make the touch button more sensitive.
// Setting the "off" value higher will make the touch button less likely
// to be "stuck". Too high values can lead to oscillations.

#define touch_threshold_on 40
#define touch_threshold_off 20

// If the touch button is pressed, the bias value is not updated
// to prevent detection of multiple touches. In some cases this may
// lead to a "stuck button" situation. Therefore an update of the bias
// is forced after a certain time unless this function is deactivated.
//
// The value of timeout corresponds to the number of calls to tinytouch_sense().
// The maximum is 255.
// Setting timeout to 0 will turn the functionality off.

#define touch_timeout 255
//#define touch_timeout 0    // turn off timeout functionality


// Define pins to use for the reference input and the touch button
// The reference pin is used to charge or discharge the internal
// sample&hold capacitor. This pin is used in output mode and should
// not be shorted to VCC or GND externally.
// The sense pin is connected to the touch-button. To improve noise immunity
// a series resistor can be used.

// The pin number must match the corresponding analog input number ADCx.
// Default port is PORTB. (ATtiny 5/10/13/25/45/85)

#define tt_refpin 2   // Use PB2 as reference pin
#define tt_refadc 1   // Use ADC1 as reference ADC input
#define tt_sensepin 3 // Use PB3 as sense pin
#define tt_senseadc 3 // Use ADC3 as sense ADC input

//////////////////////////////////////////////////////////////////////////
//
// Library functions
//
//////////////////////////////////////////////////////////////////////////

// Library initialization
// Call this once to initialize the library functions and the ADC converter
void  tinytouch_init(void);


uint16_t bias;
uint8_t  touch;
#if touch_timeout>0
uint8_t timer;
#endif

enum {tt_off = 0, tt_on, tt_push, tt_release, tt_timeout};
