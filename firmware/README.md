# Firmware

With each change of hardware, the firmware needed to change too. This contains the firmware for all revisions.

## Software Used

Arduino IDE 1.8.13 was used to program the microcontroller for the first deign, but in later revisions I moved to MPLAB to try it out and was pleased with the results.

# Revision 1.0 - `sleepingCapSense`

I used the [TinyTouchLib](https://github.com/cpldcpu/TinyTouchLib) library made by [cpldcpu](https://github.com/cpldcpu). I had some issues using it as a conventional library in Arduino, so I opted to simply copy the files I needed directly into the sketch folder and modified them slightly to work. Other than the file modifications required to get it to compile, I only changed the the sensitivities to suit the final circuit board and pad. Otherwise the code I use related to capacitive sensing in my sketch is taken directly from the library's example.

The code simply checks for if there is a press event and then flashes the eyes.

## Power Reduction

This relies heavily on the `avr/power` library, and some of my own register manipulations to disable features and put the microcontroller into sleep for the majority of the timer. The watchdog timer (WDT) is used to awake the system periodically to perform the sensing routine. 

## ATtiny85 Support

Support for this microcontroller in the Arduino IDE is provided by the [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore) project hosted by [Spence Konde](https://github.com/SpenceKonde).

# Revision 2.0 - `raccoon_rev_2_0.X`

I didn't intend to change the behaviour of the system, I just wanted to port what working in Rev. 1.0 to the ATtiny1617 from the ATtiny85.

Since I was moving to the to the tinyAVR 1-series of chips with the ATtiny1617, I would need to rewrite the entire firmware for the new hardware, especially to make use of the 1617's built in Peripheral Touch Controller (PTC) peripheral for capacitive sensing rather than the ADC trickery used in revision 1.0. 

Since I was basically starting from scratch, I took the opprotunity to try out Microchip's own toolchain and pivoted to using MPLAB. Once I learned my way though their project configuration system **I had the firmware complete in a single evening!**

## Power Reduction

Using the generated project files from MPLAB and following their example after specifying "low-power" for my capacitive sensing pad the performance was what I hoped for! I measured an average current draw of 15&nbsp;uA when sensing which was better than revision 1.0, and it was also more responsive! Kudos to their team! 

# Revision 3.0 - `raccoon_rev_3_0.X`

I've prepared the core of the project in MPLAB, but I haven't actually written code yet to drive the LEDs with PWM (I previously just used conventional digital outputs).

With revision 3.0 I would like to take the time to try out more effects to make it seem a bit more lifelike and amusing to interact with compared to earlier revisions. Some ideas:

- Diming them instead of hard on/offs
- Having the LED brightness not be synchronized between eyes, as though its "eye"s are opening cartoonishly out of slumber
- Using an intermediate level of brightness and then repeated touches make it go full power