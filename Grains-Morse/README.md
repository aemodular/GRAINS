# GRAINS Morse-code

Morsecode playback can be started and then will be played back in a loop or play once.

On startup the Morsecode for "AEM" (AE Modular) will be sent out as Gate-events via GRAINS' digital output. 
To hear this, for instance sent digital out to 2VCA and any Oscillator to the same VCA as audio-material.

To change the initial Morse-Message please search for "###" and adjust the string to your liking!

To enter new morsecodes (in realtime!) connect a MIDI-Keyboard and Connect Bus-Gate with IN1, Bus-CV with IN3.

As an intial setup: "Shortcut" audio-out with audio-in, Set P1 to IN1 and max position. Set P2 to MAN and max position. Set P3 to min postion.
The Morsecode itself can be modified and/or overwritten as explained further below.
  
Of course the Gate-events can be used for any other usecase, like triggering 2ENV, KICK, DRUMKIT or whatever.
Even usage as a Clock-Signal may be considered for creative use!
  
Thanks for explanations and examples regarding:
Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany https://www.khm.de/home
interface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html
Backgroundinfos concerning Arduino Muxer, Registervalues etc: 
http://www.robotplatform.com/knowledge/ADC/adc_tutorial_2.html http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

Demotrack available here: https://soundcloud.com/taitekatto/grains-morse

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: Gate to trigger new letter (select IN1 and put P2 to max!)
* IN2 / Pot2: Stop Morsecode if all left, Play once in middle-position, loop Morsecode when turned to max (select MAN for P2!)
* IN3:        Select letter (Note CV)     // Two upper octaves C#: '.', F#: '-', c#: word pause, g#: delete last character
* A:          Clock input (Yes, we use the audio-inut for clock-in, here!)
* Pot3:       Beat Devider (changes speed, max speed [== default] when all left).

__Outputs__

* OUT:        Puts out a steady-rate clock-signal, use this to "shortcut" with audio-in, if you don't want to use an LFO or Bus-Clock as your clock!
* D:          Gate to trigger Morsecode on Output (connect to 2VCA for instance)


## IMPORTANT:  

Please select IN1 as input and IN2 as manual, Then set Pot1 and Pot2 to their maximum value!
Otherwise Gate-Tracking and playback of Morse-Code may not work as intended! You may change Pot2 to restart or pause the Morse-Code lateron!
   

## Notes

To learn about Morsecode and see the table of Morse-characters you could put in, please refer to: 
https://en.wikipedia.org/wiki/Morse_code
As initial Content the String "AEM" for AE Modular will be played back as morse code
Similar to putting in real morse code, there will be no error-checking, so if you want you can invent your own, new morse-characters ;-)
  
Use keys of lowest 2 octaves to change content to be sent out as Morsecode to Digital-Out as Gate-CV:

C# (or C) enter a new DIT (short morse character) - second-highest octave

F# (or G) enter a new DAH (long morse character) - second-highest octave

c# (or c) enter a PAUSE - (should not be more than one at a time) highest octave

f# (or g) delete last morse-character (DIT, DAH or PAUSE) - highest octave

Please note: in contrast to a Computer-Keyboard we have _no_ autorepeat for entering of keys, Morsedigits or pauses that is!
-> To better understand what is happening here, we recommend you to enable the Serial Monitor from the Arduino IDE (CTRL/CMD+SHIFT+M)

__Caution!__ Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

-------------------------------------------------------------  

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'GRAINS Morse Code' an experimental Firmware for the AE Modular GRAINS module by tangible waves to enter and playback Morsecode as Gate events

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

