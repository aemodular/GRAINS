# GRAINS
Firmware for the GRAINS module from tangible waves made by the members of the AE Modular Community. 

__IMPORTANT__ 

This is a community project and __NOT__ official firmware from tangible waves! tangible waves makes no promise that the firmwares will work on the hardware that tangible waves produces and sells. Further tangible waves accepts no responsibility for any damage caused to the any product where these firmwares will be installed!

__How to__

To use any of the firmwares, to download all firmwares at once please click the 'clone or download'-button at https://github.com/aemodular/GRAINS and then select the 'download'-option.  

Regarding how to upload new firmware to your AE Modular GRAINS please refer to:    
http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS

You can find more information about the AE Modular community at http://forum.aemodular.com/aemodular/GRAINS

The GRAINS module can be ordered directly from the tangible waves website at https://tangiblewaves.com/aemodular/GRAINS

List of Firmwares
==================
__For more details on the individual firmwares, please refer to the equivalent README-files that come with them.__

__ByteBeat-Xfade__                                                               
Two different ByteBeats can be selected and crossfaded. Editing of ByteBeats is possible in the .ino-File.   

__Spell or Speak__                                                               
Synthetic words, as used with a famous toy from the late 70th, can be triggered via Gate-impulses and selcted via CV or a pot.  

__Grains Morse__  
This firmware sends gate triggers out of the D output of the GRAINS module according to the international Morse standard. The sentence that is encoded can be changed in the source code or you can enter morse code via an attached MIDI keyboard.

__Grains MIDI out__  
This Firmware provides the option to send out CVs and/or POTs as MIDI-CC values via MIDI-out over USB.
As a bonus also MIDI-in over USB can be used to convert any MIDI-Key on/off to Gate and any MIDI-CC to CV for GRAINS.
Please be aware that along with this Firmware you will also need extra, yet free software on your Windows/MACos/Linux System to route the MIDI-events correctly. 

__Grains BufferShuffler__  
The audio-in of GRAINS is used to fill an audio-buffer. Parts of the audio in this buffer can be looped, sliced, reversed and slowed down, giving various 8bit LoFi effects.  

__Grains EnvelopeFollower__   
Tries to mimic the dynamics on audio-in as CV via the audio-out port. This can be used as a kind of envelope to drive a Filter and/or VCA for instance.  

__Grains LoFi-MultiFX__ 
SinusGenerator/RingModulator/PitchShifter/PhaseShifter/BitCrusher/BitClipper/WaveFolder + Gate. There are 5 preset combinations of these effects to select from. Many parameters can be modulated.
