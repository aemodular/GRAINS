'GRAINS LoFi-MultiFX' SinusGenerator/RingModulator/PitchShifter/PhaseShifter/BitCrusher/BitClipper/WaveFolder + Gate
an alternative Firmware for the AE Modular GRAINS module by tangible waves https://www.tangiblewaves.com (using a samplerate-driven framework) for algorithmic generation of melodies. 
https://www.tangiblewaves.com/store/p86/GRAINS.html

* __Demotrack__ for Grains 'Palindrome' is available here: https://soundcloud.com/taitekatto/grains-lofi-multifx

# Usage of this Firmware with GRAINS:

__Inputs__

* A:          Audio input
* IN1 / Pot1: Shift Phase: To get a Phaser-effect please modulate with an LFO (slow triangle for the well know effect)
* IN2 / Pot2: Frequency of sine for RingMod, or sine itself, if no signal on input or PitchShift
* IN3:        BitCrusher/BitClipper: set amount from 0 to 7 (8 to 1 bit)
* Pot3:       Modeselector: Different routings and options of differnet effects, from left to right:
 
              Mode 1: Audio->RingMod->PhaseShifter->BitCrusher                    
              Mode 2: Audio->WaveFolder->RingMod->BitCrusher->PhaseShifter
              Mode 3: Audio->PitchShift->BitClip->PhaseShifter                                                  
              Mode 4: SineWave->BitClip->PhaseShifter                                                   
              Mode 5: Audio->BitCrusher->PitchShifter->BitCrusher (here IN2/Pot2 will modulate the first BitCrusher)

__Outputs__

* OUT:        Audio out 
* D:          HIGH if audio-Signal is above zero, LOW if not

## Notes

* We highly recommend to try out Ringmod with the SquareWave from OSC - if you twist the PULSEWIDTH knob all way up you can get interesting mixes of the sine used for RingMod and the ringmodulated signal
* Good results for phasing can be obtained for instance with a slow triangle to IN1, to get fast percussive results you may use a squarewave instead
* Intersting BitCrushing/BitClipping sequences can be applied with an LFO and/or TRIQ164 to IN3
* If you want to turn BitCrushing/BitClipping on/off or manually select the amount of BitCrushing/BitClipping you may use 2ATT/CV for instance
* Modulating the Frequency of the SineWave/Ringmod via CV, for instance from an LFO is another interesting option
* (Digital) NOISE can give interesting results, especially with the WaveFolder and PhaseShifter
* When using digital sounds as input you may end up with high frequency aliasing, so adding a lowpass filter on output is suggested in this case
* You may consider using a long reverb on the output of LoFi-MultiFX as well

* If you are a bit familiar in programming, it should be easy for you to add your own combinations of the given effects by having a look at GrainsIO::process()

Thanks for explanations and examples regarding:

Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany
Code parts inluding Timer-callback-handling, Sinewave-calculation, DC-shifting for ringmodulation and more used by kind permission of Martin Nawrath (Kunsthochschule fuer Medien Koeln/Academy of Media Arts Cologne) 
http://interface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html

Backgroundinfos concerning Arduino Muxer, Registervalues etc: 
http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

__Caution!__ Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

-------------------------------------------------------------  

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'GRAINS LoFi-MultiFX' an alternative Firmware for the AE Modular GRAINS module by tangible waves

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
