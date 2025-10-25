# Raccoon Hardware Files

The folders `raccoon_revX` are the actual KiCad projects, `raccoon-art` is the collection of footprints for the artwork derived from the images in the [media](../media/) folder.

# Revision 1.0

These design files are the lightly adjusted version of the original raccoon hardware, to address the hardware bugs related to the programming interface.

# Revision 2.0

A continuation of the work done for the files shared as "Revision 1". Changes from the previous revision:

- Complete removal of the ATtiny85 in favour of the ATtiny1617 _which is definitely overkill for this task but what I had on hand._
- Moved to using a TagConnect programming header to simplify programming. _This does slightly affect the art on the front._
- Added a jumper to cut for current measurement

# Revision 3.0

Tweaking the design of revision 2.0. Changes from the previous revision:

- Changed the main microcontroller to a 1616 since we don't need many GPIO, but I want flash space to try different eye effects, maybe even changing with each touch
- Removed unnecessary passives; the sense resistor, VREF capacitor, secondary decoupling capacitor (100n)
- Split the LED lines for individual eye control, via PWM
- Moved the programming pads to be closer to the edge so some alignment pins align to the PCB outline instead of needing holes in the PCB
- Moved the LED current limiting resistors to be closer to the microcontroller and out of the path of the coin cell
- Added a keepout zone for pour near the current measurement jumper to avoid cutting and then shorting VCC to GND

# Software Used

KiCad version 9 was used for all project files.
