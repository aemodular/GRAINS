# GRAINS ByteBeats Crossfader

an experimental Firmware for the AE Modular GRAINS module by tangible waves (using a samplerate-driven framework).

If you want to get serious about using ByteBeats you should consider AE Modular ALGODRONE instead, this is just experimental to show the ByteBeats concept
https://www.tangiblewaves.com

To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS

Thanks for explanations and examples regarding:

Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany 
https://www.khm.de/homeinterface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html

Backgroundinfos concerning Arduino Muxer, Registervalues etc: 

http://www.robotplatform.com/knowledge/ADC/adc_tutorial_2.html

http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

Very helpful and inspiring information regarding ByteBeats by The Tuesday Night Machines:

http://nightmachines.tv/the-absolute-beginners-guide-to-coding-bytebeats.html

Demotrack available here: https://soundcloud.com/taitekatto/grains_bytebeat_xfade

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: Select ByteBeat A
* IN2 / Pot2: Select ByteBeat B
* IN3:        (unused so far)
* Pot3:       Crossfade between ByteBeat A and B
* A:          (unused so far)

__Outputs__

* OUT:        Audio out of mixture of ByteBeat A and B
* D:          (unused so far)

## Notes

1. You can add or modify your own ByteBeats at GrainsIO::process() after where it says: "static byte (*beats[])(long t)  // List of Bytebeats..."
  
2. Depending on the complexity of a ByteBeat, playback-speed may vary This even can be the case for one ByteBeat (let's say ByteBeat B), when you change the selection of other ByteBeat (let's say ByteBeat A) Even though this feature may be considered an undesired it can be used creatively, but you'll have to find out yourself ;-)  

3. Long ByteBeats may not work as expeced, because they also might slow down playback speed.

__Caution!__ Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

-------------------------------------------------------------  

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'GRAINS ByteBeats Crossfader' an experimental Firmware for the AE Modular GRAINS module by tangible waves

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

