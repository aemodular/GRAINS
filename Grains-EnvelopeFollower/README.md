# 'GRAINS Evelope Follower' / 'VU-Meter'  
an experimental Firmware for the AE Modular GRAINS module by tangible waves https://www.tangiblewaves.com (using a samplerate-driven framework). The goal is to mimic the dynamics on audio-in as CV via the audio-out port. This can be used as a kind of envelope to drive a Filter and/or VCA for instance.
https://www.tangiblewaves.com/store/p86/GRAINS.html
  
* __Demotrack__ for Grains 'Palindrome' is available here: https://soundcloud.com/taitekatto/grains-envelopefollower

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: (unused so far)
* IN2 / Pot2: Select quantisation for audio-in measurement
* IN3:        (unused so far)
* Pot3:       amplification of CV measured at audio-in
* A:          audio-in

__Outputs__
  
* OUT:        CV as "sampled" and "quantized" from audio-in, to be used as a kind of VU-Meter / Envelope-Follower output
* D:          (unused so far)

## Notes

* The algorithm used here to do the envelope-following is optimized to be as easy and thus fast as possible (no prefiltering via software is done)
* In fact the way the envelope-detection is working here, is more like a VU-meter
* It works quite well with rhythmical content, for slowly evolving sounds prefiltering may be helpful, please experiment ;-)

For details on the concept of envelope-detection please refert to:
https://en.wikipedia.org/wiki/Envelope_detector

Thanks for explanations and examples regarding:

Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany 
https://www.khm.de/homeinterface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html

Backgroundinfos concerning Arduino Muxer, Registervalues etc: 
http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

__Caution!__ Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

-------------------------------------------------------------  

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'GRAINS Evelope Follower' an experimental Firmware for the AE Modular GRAINS module by tangible waves

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
