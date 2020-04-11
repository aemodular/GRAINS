# GRAINS "Midi out"
an alternative Firmware for the AE Modular GRAINS module by tangible waves (using a samplerate-driven framework) 
providing the option to send out CVs and/or POTs as MIDI-CC values via MIDI-out over USB. 

As a bonus also MIDI-in over USB can be used to convert any MIDI-Key on/off to Gate and any MIDI-CC to CV for GRAINS

To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS

Thanks for explanations and examples regarding:

Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany 
https://www.khm.de/homeinterface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html

Backgroundinfos concerning Arduino Muxer, Registervalues etc: 
http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

* __Demotrack__ for Grains 'ByteBeat-Xfade' is available here: https://soundcloud.com/taitekatto/grains-midi-out

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: Send out as MIDI CC 21
* IN2 / Pot2: Send out as MIDI CC 22
* IN3:        Send out as MIDI CC 23
* Pot3:       Send out as MIDI CC 24
* A:          Send out as MIDI CC 25

__Outputs__

* OUT:        CV value - will reflect the intensity of any controller data sent to GRAINS' MIDI in via USB (Hairless MIDI see below)
* D:          Gate on/off - will be on for any key pressed, will be off for any key released sent to GRAINS' MIDI in via USB (Hairless MIDI see below)

## Notes

The Atmega processor used in GRAINS is not capable of sending out MIDI via USB directly, but luckily serial data can be converted to MIDI easily.
A popular way to do this is the free software "Hairless MIDI<->Serial Bridge". Software and documentation for MacOS/Windows/Linux can be downloaded here:
https://projectgus.github.io/hairless-midiserial

* If you want to use this MIDI data as input for your DAW on your PC you will need a MIDI-loopback device in software or route MIDI-out to MIDI-in with your hardware
* Please make sure to close Hairless MIDI when you want to upload this Firmware to GRAINS (uses the same USB-Serial-Port)
* You can use up to 5 pots to send out MIDI CC for instance by setting 2ATT/CV to 5V and sending the votages to IN3 and A (analogue input)
* Attaching controls in your DAW can be a bit tricky: set all knobs to 0 and only dial in one pot to max and back to zero at a time

* As an extra also MIDI-in will be processed: any key pressed will trigger a Gate via digital out
* Any Controller-Data sent to GRAINS (via Hairless MIDI or similar) will generate CV via analog out 
* In terms of performance please avoid unnecessary MIDI-data, for instance clock-signals should be avoided to be sent via Hairless Midi, here

* The so called Midi "running status" is recognized on input, but not used on output, so this firmware should be compatible with any MIDI-hardware or software.

To avoid the complexity of additional libraries, we use our own midi_sendControlChange() method here instead of the standard MIDI.sendControlChange(). MIDI-inut is processed directly as incoming serial data.
If you want to extend MIDI-capabilities you may want to change this by installing the Arduino MIDI-Library and change this accordingly!
https://playground.arduino.cc/Main/MIDILibrary/
For Details regarding integration with "Hairless MIDI<->Serial Bridge" please refer to the site of the project as already mentioned above.

__Caution!__ Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

-------------------------------------------------------------  

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'GRAINS MIDI out' an alternative Firmware for the AE Modular GRAINS module by tangible waves

Copyright (C) 2020  Mathias Brüssel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
