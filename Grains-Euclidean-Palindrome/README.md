'GRAINS Euclidean-Palindrome' 
an alternative Firmware for the AE Modular GRAINS module by tangible waves https://www.tangiblewaves.com (using a samplerate-driven framework) for algorithmic generation of melodies. 
https://www.tangiblewaves.com/store/p86/GRAINS.html
  
* __Demotrack__ for Grains 'Palindrome' is available here: https://soundcloud.com/taitekatto/grains-euclidean-palindrome

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: Select note-sequence pattern from a list (mainly palindromic numbers) to select pitch of (bamboo)samples
* IN2 / Pot2: Select rhythm pattern from a list (mainly Euclidean/Borklund rhythms) to trigger the sample
* IN3:        Select rhythm pattern from a list (mainly Euclidean/Borklund rhythms) to trigger separate gate events
* Pot3:       Select scale for scale-correction from list for pitch of (bamboo) samples
* A:          Clock input - be sure to use BEAT DEVIDER for decent results, for arbitrary speed a Sqarewave-LFO could be used instead

__Outputs__

* OUT:        Audio out of (bamboo)samples
* D:          Gate out, providing the identical or an additional rhythm to those used by the samples

## Notes

* The main source for rhythm- and note-patterns used here are Eucledian rhythms and panlindromic numbers
* For the Bjorklund/Eucledian rhythm algorithm please refer to the paper "The Euclidean Algorithm Generates Traditional Musical Rhythms" by Godfried Toussaint: http://cgm.cs.mcgill.ca/~godfried/publications/banff.pdf
* For palindrome numbers please have a look at: https://en.wikipedia.org/wiki/Palindromic_number
* To add your own sequence of notes, please search for "note_pattern[]" and change or add the content there
* To add your own sequence of gates, please search for "gate_pattern[]" and change or add the content there
* To add your own sequence scale-correction, please search for "scale_pattern[]" and change or add the content there
* The _Bamboo Samples_ used in this Firmware are taken from the Sample-collection that comes with the Mozzi Library, 
* the GNU General Public License v3 "or newer" is applied here, too. https://sensorium.github.io/Mozzi
* The 'b'-note had to be substitued with 'bb', you could use your own samples instead, please search for "bamboo[12]" and change the contents there.
* In order to generate your own sample-content, please use raw-audio files with signed 8bit content 
* You can convert these to the format used here with the Python script "char2mozzi.py" that is part of the Mozzi-library.

The notation for Notevalues is '0'-'9' and 'a'-'z' to provide notes over 3 Octaves from C to b'  
So, for instance "047" would be C,E,G to be played in a loop.  
The notation for gatevalues is a pattern of 'x' and '.', where 'x' would mark an open gate (the beat) and '.' a closed gate (a pause)  
For instance "x." would be a steady rhythm with the beat on every second note to be played in a loop.  
The two patterns combined would result in the equivalent of "C.E.G.C.E.G" to be played as sample-notes and so on and on.  
As another example "07470" and "x..x." would give "C..G.E..G.C..C." and so on.  
So, even though the main idea here is to combine Björklung/Euclidian patterns with numeric panidromes, the notation can be used for many other purposes  

__Thanks__ for explanations and examples regarding:

Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany
http://interface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html

Backgroundinfos concerning Arduino Muxer, Registervalues etc: 
http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

__Caution!__ Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

-------------------------------------------------------------  

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'Euclidean-Palindrome' an alternative Firmware for the AE Modular GRAINS module by tangible waves

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
