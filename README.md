# Raccoon
A little raccoon trinket based on an ATtiny85 and capacitive sensing. The premise is simple, a small circuitboard decorated to be a raccoon on one side has circuitry mounted on the back to illuminate the eyes when a touch is detected on the raccoon. Designed to maximize battery life and run of standard coin cells.

My first foray into making PCB art hardwarewise, and both capacitive sensing and really trying to supress power draw with coding.

## Code

### Capacitive Sensing
For this I used the "TinyTouch" library made by XXXXX. I had some issues using it as a conventional library in Arduino, so I opted to simply copy the files I needed directly into the sketch folder and massaged them slightly to work. Other than the file modifications required to get it to compile, I only changed the the sensitivities to suit the final circuit board and pad. Otherwise the code I use related to capacitive sensing in my sketch is taken directly from the library's example.

The code simply checks for if there is a press event and then flashes the eyes.

### Power Reduction
This relies heavily on the avr/power library, and some of my own register manipulations to disable features and put the microcontroller into sleep for the majority of the timer. The watchdog timer (WDT) is used to awake the system periodically to perform the sensing routine. 

### ATtiny85 Support
Support for this microcontroller in the Arduino IDE is provided by the ATtinyCore project hosted by XXXXX.

## Hardware
The electrical design of the system is simple, a microcontroller powered directly from a coin cell battery that is connected to a couple LEDs, a capacitance detection pad, and a select few passive components. The challenge with the board for this was the artwork and making it look nice.



## Software Used
KiCAD 5.1.10 was used to design the hardware.
Arduino IDE 1.8.13 was used to program the microcontroller.

There were some libraries downloaded to extend the functionality of these programs as described in the previous sections.
