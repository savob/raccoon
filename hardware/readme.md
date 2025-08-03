# Raccoon Hardware Files

The folders `raccoon_revX` are the actual KiCad projects, `raccoon-art` is the collection of footprints for the artwork.

## Revision 1.0

These design files are the lightly adjusted version of the original raccoon hardware, to address the hardware bugs related to the programming interface.

## Revision 2.0

A continuation of the work done for the files shared as "Revision 1". Changes from the previous revision:

- Complete removal of the ATtiny85 in favour of the ATtiny1617 _which is definitely overkill for this task_
- Moved to using a TagConnect programming header to simplify programming. _This does slightly affect the art on the front._
- Added a jumper to cut for current measurement

# Software Used

KiCad version 9 was used for all project files.
