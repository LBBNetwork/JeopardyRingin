Code for Jeopardy Ring-in Device Mk. II

Useful code if you have a Raspberry Pi and would like to use it as a ring-in device for game shows,
or use it as a starting point to perhaps figure out how GPIO works on the RPi.

Hardware configuration:
* Raspberry Pi (Please let me know if the GPIO pins
  differ from each revision - I checked with bcm2835.h
  and tried as best I could but I only have a Rev2
  Model B)
* SPST switch on pin 18
* Three momentary contact switches on pins 23, 17 and 24
* One LED (plus appropriate resistor) on pin 4
* Three LEDs (plus appropriate resistors) on pins 22, 25
  and 21 (for Rev1 units) or 27 (for Rev2 units)

Software configuration:
* Requires the bcm2835 library for GPIO. Get it from:
  http://www.airspayce.com/mikem/bcm2835/

Compiling:
* Ensure you have gcc and the bcm2835 library installed.
  To compile, simply issue this command:

  make

Running:
* Since this program uses GPIO pins, you must run the
  program as root.

Have fun!
