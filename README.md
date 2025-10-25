# Raccoon
A little raccoon trinket based on an ATtiny and capacitive sensing. The premise is simple, a small circuitboard decorated to be a raccoon on one side has circuitry mounted on the back to illuminate the eyes when a touch is detected on the raccoon. Designed to maximize battery life and run of standard coin cells.

My first foray into making PCB art hardwarewise, and both capacitive sensing and really trying to supress power draw with coding.

All this artwork was drawn by my lovely sister, Dorotea Bajic. Check out their website [dorotea.ca](https://dorotea.ca/), it's far nicer looking than my own.

# Code

## Capacitive Sensing
For this I used the [TinyTouchLib](https://github.com/cpldcpu/TinyTouchLib) library made by [cpldcpu](https://github.com/cpldcpu). I had some issues using it as a conventional library in Arduino, so I opted to simply copy the files I needed directly into the sketch folder and massaged them slightly to work. Other than the file modifications required to get it to compile, I only changed the the sensitivities to suit the final circuit board and pad. Otherwise the code I use related to capacitive sensing in my sketch is taken directly from the library's example.

The code simply checks for if there is a press event and then flashes the eyes.

## Power Reduction
This relies heavily on the `avr/power` library, and some of my own register manipulations to disable features and put the microcontroller into sleep for the majority of the timer. The watchdog timer (WDT) is used to awake the system periodically to perform the sensing routine. 

## ATtiny85 Support
Support for this microcontroller in the Arduino IDE is provided by the [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore) project hosted by [Spence Konde](https://github.com/SpenceKonde).

# Hardware
The electrical design of the system is simple, a microcontroller powered directly from a coin cell battery that is connected to a couple LEDs, a capacitance detection pad, and a select few passive components. The challenge with the board for this was the artwork and making it look nice.

Initially an ATtiny85 was selected but later I switched to ATtiny1617 due to dedicated capacitive sensing hardware and reported lower power consumption.

# Software Used
KiCad is used to design the hardware, version 9 as of writing.

Arduino IDE 1.8.13 was used to program the microcontroller.

There were some libraries downloaded to extend the functionality of these programs as described in the previous sections.
