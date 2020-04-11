# GRAINS "BufferShuffler" an alternative Firmware for the AE Modular GRAINS module by tangible waves https://www.tangiblewaves.com
(using a samplerate-driven framework) reading audio in and filling a buffer with it that can be manipulated in realtime.
To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS

Thanks for explanations and examples regarding:

Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany 
https://www.khm.de/homeinterface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html

Backgroundinfos concerning Arduino Muxer, Registervalues etc: 
http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

* __Demotrack__ for Grains 'BufferShuffler' is available here: https://soundcloud.com/taitekatto/grains-buffershuffler

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: Set stepwidth of slices of the audio-buffer to sizes from 1 (default) to 16
* IN2 / Pot2: Repeat-factor: used to slow down playback of manipulated audio-buffer by factor 0 (normal speed) to 31
* IN3:        Decide on direction of reading the audio-buffer for manipulation: set to max to reverse, else set to min 
* Pot3:       Set upper boundary of audio-buffer: max to have full range, set to any other value to reduce range
* A:          Audio input which will be send to the audio-buffer for manipulation

__Outputs__

* OUT:        Audio-result from manipulated audio-buffer
* D:          Gate is on if wave in manipulated audio-buffer is positive or off when negative. Results are quite arbitrary, but can be interesting

## Notes

* Because the size of the audio-buffer available with the ATmega-chip used for GRAINS is rather small, results should be best with percussive material, but of course experimentation is encouraged

__Caution!__ Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

-------------------------------------------------------------  

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'BufferShuffler' an alternative Firmware for the AE Modular GRAINS module by tangible waves

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
