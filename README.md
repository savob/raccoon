# Raccoon

A little raccoon trinket based on an ATtiny and capacitive sensing. The premise is simple, a small circuitboard decorated to be a raccoon on one side has circuitry mounted on the back to illuminate the LED eyes when a touch is detected on the raccoon. Designed to maximize battery life and run of standard coin cells.

My first foray into making PCB art hardwarewise, and both capacitive sensing and really trying to supress power draw with coding.

_All this artwork was drawn by my lovely sister, Dorotea Bajic. Check out their website [dorotea.ca](https://dorotea.ca/), it's far nicer looking than my own._

# Code

The code is pretty simple for the first two revisions given the scope of the projects, and a minor hack was used to get revision 1.0 working. I used the Arduino framework for the first revision but have since moved to using Microchip's own MPLAB to make full use of the chips.

# Hardware

The electrical design of the system is simple, a microcontroller powered directly from a coin cell battery that is connected to a couple LEDs, a capacitance detection pad, and a select few passive components. The challenge with the board for this was the artwork and making it look nice.

Initially an ATtiny85 was selected but later I switched to the tinyAVR 1-series due to dedicated capacitive sensing hardware and reduced power consumption.

# Software Used

KiCad is used to design the hardware, version 9 as of writing.

Arduino IDE 1.8.13 was used to program revision 1.0. MPLAB for revisions 2.0 onwards.
