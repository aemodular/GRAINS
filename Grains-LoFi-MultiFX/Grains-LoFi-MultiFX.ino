/*
'GRAINS LoFi-MultiFX' SinusGenerator/RingModulator/PitchShifter/PhaseShifter/BitCrusher/BitClipper/WaveFolder + Gate
an alternative Firmware for the AE Modular GRAINS module by tangible waves https://www.tangiblewaves.com (using a samplerate-driven framework) for algorithmic generation of melodies. 
https://www.tangiblewaves.com/store/p86/GRAINS.html

* __Demotrack__ for Grains 'Palindrome' is available here: https://soundcloud.com/taitekatto/grains-lofi-multifx

# Usage of this Firmware with GRAINS:

__Inputs__

* A:          Audio input
* IN1 / Pot1: Shift Phase: To get a Phaser-effect please modulate with an LFO (slow triangle for the well know effect)
* IN2 / Pot2: Frequency of sine for RingMod, or sine itself, if no signal on input
* IN3:        BitCrusher/BitClipper: set amount from 0 to 7 (8 to 1 bit)
* Pot3:       Modeselector: Different routings and options of differnet effects, from left to right: 
              Mode 1: Audio->RingMod->PhaseShifter->BitCrusher                    
              Mode 2: Audio->WaveFolder->RingMod->BitCrusher->PhaseShifter
              Mode 3: Audio->PitchShift->BitClip->PhaseShifter                                                  
              Mode 4: SineWave->BitClip->PhaseShifter                                                   
              Mode 5: Audio->BitCrusher->PitchShifter->BitCrusher

__Outputs__

* OUT:        Audio out 
* D:          HIGH if audio-Signal is above zero, LOW if not

## Notes

* We highly recommend to try out Ringmod with the SquareWave from OSC - if you twist the PULSEWIDTH knob all way up you can get interesting mixes of the sine used for RingMod and the ringmodulated signal
* Good results for phasing can be obtained for instance with a slow triangle to IN1, to get fast percussive results you may use a squarewave instead
* Intersting BitCrushing/BitClipping sequences can be applied with an LFO and/or TRIQ164 to IN3
* If you want to turn BitCrushing/BitClipping on/of or manually select the amount of BitCrushing/BitClipping you may use 2ATT/CV for instance
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
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))       // Clearbit for Atmega8/"Arduino" registers
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))        // Setbit for Atmega8/"Arduino" registers
#define PWM_PIN           11      // DAC (PWM actually) output for Grains is fix on pin 11
#define DIGITAL_OUT_PIN   8

class GrainsIO                // This is our main class to encapsulate any GRAINS related stuff, mainly I/O
{
  public:
    // vars altered by interrupt
    static bool div32;
    static bool div16;
    static volatile bool new_sample;
    static volatile byte audio_data;
    static volatile byte pot_cv;
    static volatile byte ibb;
    static volatile byte pot_selector;
    static volatile byte cv[4];
    
    // Main processing "callback" - this is were all user-code for this framework should go if possible.  
    static void GrainsIO::process();

    static byte a_out;      // Analogue output has to go here
    static int d_out;       // Digital output has to go here
};
// --- Instanciate all static variables now, in fact we only use class-variables, this mainly is syntactic sugar to garantee encapsulation
bool GrainsIO::div32 = false;
bool GrainsIO::div16 = false;
volatile bool GrainsIO::new_sample = false;
volatile byte GrainsIO::audio_data = 0;
volatile byte GrainsIO::pot_cv = 0;  // MB 20200314
volatile byte GrainsIO::pot_selector = 0;
volatile byte GrainsIO::cv[4]={1,1,0,0};
byte GrainsIO::a_out = 0;
int GrainsIO::d_out = 0;

static GrainsIO gio;  // Instanciate our Grains-IO-class globally, so it can be accesse via class-members, interupt-funtions and the main Arduino-loop()
static GrainsIO* gp = &gio;    // All access to the class-variables above for convenience is done by this class-pointer

// *******************************************************************************************************
// *** use the variable-names defined below with your process() method!
#define in1_pot1    gp->cv[2]
#define in2_pot2    gp->cv[1]
#define in3         gp->cv[3]
#define pot3        gp->cv[0]
#define audio       gp->audio_data
#define analog_out  gp->a_out
#define digital_out gp->d_out             // Set to HIGH or LOW respectively!

// *** Change any code only here (if possible) to use AE Modular framework for your own GRAINS firmwares ***
static byte sine_table[512];              // We calculate a sine table only once on setput()
void generate_sine_table()
{
const float slice_step = 2*3.141592 /512; // Prepare 2*Pi to fill the 512 byte bufferarry
float step_val = 0; 
float tone_of_step = 0;

  for(int i=0; i <= 511; i++)
  {                                       // with 50 periods sinewawe
    tone_of_step = 127*sin(step_val);     // fundamental tone
    sine_table[i] = 127+tone_of_step;     // write value into array, adding DC-offset
    step_val += slice_step;               // in the range of 0 to 2*Pi  and 1/512 increments
  }
}

byte phaseShifter(byte current_audio, byte pot_cv)
{
static byte phaser_memory[512];
static int phaser_idx1 = 0;
static int phaser_idx2 = 0;

  phaser_memory[phaser_idx1] = current_audio;
  byte phase_out = (current_audio+phaser_memory[phaser_idx2])/2;    // Generate phased signal now, ringmodulated signal alread got fed in
  phaser_idx1 = (phaser_idx1+1)&511;                                // calculete index for next round, limit to 0..511 
  phaser_idx2 = (phaser_idx1-pot_cv/20)&511;                        // apply phasing by turning Knob 2 or sending an LFO to in2, limit to 0..511 

  return(phase_out);
}

byte ringMod(int* counter, byte current_audio, byte pot_cv)
{
  return ((128-current_audio)*(sine_table[(*counter+=(pot_cv+1))&511]-128))/255 + 128; // Stip DC-offset from audio-in and Sine-table-entry, constrain index 0...511, adjust frequency of Sinus by Pot, multiply current bytes of sinus and audio-in, adjust to max byte-size and add DC-offset again 
}

byte sineWave(int* counter, byte pot_cv)
{
  return sine_table[(*counter+=(pot_cv+1))&511] ; // Stip DC-offset from audio-in and Sine-table-entry, constrain index 0...511, adjust frequency of Sinus by Pot, get current byte from sinus, adjust to max byte-size and add DC-offset again 
}

byte bitCrusher(byte current_audio, byte pot_cv)
{
byte shift_val = pot_cv*7/254;                                  // take in3 - for instance in3 may be modulated or fed by another pot giving 0-5V    analog_out = (((128-analog_out) >> shift_val) << shift_val)+128;  // variable bitcrushing

  return (((128-current_audio) >> shift_val) << shift_val)+128;
}

byte bitClip(byte current_audio, byte pot_cv)
{
byte shift_val = pot_cv*6/254;                                  // take in3 - for instance in3 may be modulated or fed by another pot giving 0-5V    analog_out = (((128-analog_out) >> shift_val) << shift_val)+128;  // variable bitcrushing

  return ((128-current_audio) << shift_val)+128;
}

byte waveFolder(byte current_audio)
{
  return (byte) (4.0*(((abs(0.25*(current_audio-128)+0.25 - round(0.25*(current_audio-128)+0.25))-0.25))+128)); // Formula found at https://www.desmos.com/calculator/ge2wvg2wgj
}

byte pitchShifter(byte current_audio, byte pot_cv)
{
static byte audio_save = 0;
static byte octave_count = 0;

  if(((++octave_count)%((pot_cv*23/255)+1)) == 0)               // Pitch down by giving back curent audio-byte every 2 to 24 times and repeating current audio otherwise
    audio_save = current_audio;

  return audio_save;
}

// === The process()-method will be called every time a new sample is read from analogue input, regardless if we use that data ===
void GrainsIO::process()                                  // Programming example: Grains LoFi-MultiFX
{
static int count = 0;                                     // We need a counter to access the SinusWave table that has been calculated on setup() already

  analog_out = audio;                                     // "Bypass": copy current input to output, so that it does not matter in which order we do the routing
    
  switch(pot3*4/255)                                      // Pot3 is a modeselector, to decide on routing, selection of effects and parameters/pots used
  {
    case 0:                                               // Audio->RingMod->PhaseShifter->BitCrusher                    
      analog_out = ringMod(&count, analog_out, in2_pot2);
      analog_out = phaseShifter(analog_out, in1_pot1);
      analog_out = bitCrusher(analog_out, in3);
      break; 

    case 1:                                               // Audio->WaveFolder->RingMod->BitCrusher->PhaseShifter
      analog_out = waveFolder(analog_out);
      analog_out = ringMod(&count, analog_out, in2_pot2);
      analog_out = bitCrusher(analog_out, in3);  
      analog_out = phaseShifter(analog_out, in1_pot1);
      break; 

    case 2:                                               // Audio->PitchShift->BitClip->PhaseShifter                                                  
      analog_out = pitchShifter(analog_out, in2_pot2);
      analog_out = bitClip(analog_out, in3);
      analog_out = phaseShifter(analog_out, in1_pot1);
      break;

    case 3:                                               // SineWave->BitClip->PhaseShifter                                                   
      analog_out = sineWave(&count, in2_pot2);            
      analog_out = bitClip(analog_out, in3);
      analog_out = phaseShifter(analog_out, in1_pot1);
      break;
    
    case 4:                                               // Audio->BitCrusher->PitchShifter->BitCrusher
      analog_out = bitCrusher(analog_out, in3);
      analog_out = pitchShifter(analog_out, in2_pot2);
      analog_out = bitCrusher(analog_out, in1_pot1);
      break;
  }
  analog_out > 128 ? digital_out=HIGH : digital_out=LOW;  // Generate a gate, depending on current threshhold of analoge amplitude    
}
// *******************************************************************************************************


// Sets up pins and configures the samplerate and frequency of the PWM output
void setup()
{
  generate_sine_table();                  // We need this addon to the framework here for the sine / ringmod functionality
  
  Serial.begin(115200);
  pinMode(DIGITAL_OUT_PIN,OUTPUT);

  // set adc prescaler  to 64 for 19kHz sampling frequency
  cbi(ADCSRA, ADPS2);
  sbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);

  sbi(ADMUX,ADLAR);  // 8-Bit ADC in ADCH Register
  sbi(ADMUX,REFS0);  // VCC Reference
  cbi(ADMUX,REFS1);
  cbi(ADMUX,MUX0);   // Clear all Input Multiplexers
  cbi(ADMUX,MUX1);
  cbi(ADMUX,MUX2);
  cbi(ADMUX,MUX3);
  
  // Timer2 PWM Mode set to fast PWM 
  cbi (TCCR2A, COM2A0);
  sbi (TCCR2A, COM2A1);
  sbi (TCCR2A, WGM20);
  sbi (TCCR2A, WGM21);
  cbi (TCCR2B, WGM22);

  // Timer2 Clock Prescaler to : 1 
  sbi (TCCR2B, CS20);
  cbi (TCCR2B, CS21);
  cbi (TCCR2B, CS22);

  // Timer2 PWM Port Enable
  sbi(DDRB,3);                    // set digital pin 11 to output

  //cli();                         // disable interrupts to avoid distortion
  cbi (TIMSK0,TOIE0);              // disable Timer0 !!! delay is off now
  sbi (TIMSK2,TOIE2);              // enable Timer2 Interrupt
  
  sbi(ADMUX,MUX2);               // set multiplexer to channel 2 == Input 4 MB 20200314
}

void loop()
{
  while( !gp->new_sample )                    // wait for Sample Value from ADC, Cycle 15625 KHz = 64uSec 
  { /* NOOP */ }                              // If next sample arrives, we have new CV-Data as well,...
  gp->process();                              // ... process it now ... 
  OCR2A = gp->a_out;                          // ... and send it to DAC via PWM output (PIN 11)
  digitalWrite(DIGITAL_OUT_PIN, gp->d_out);   // Unused here - delete comment-marks if needed: Send out digital value HIGH or LOW, value as set (if any) by gp->process()
  gp->new_sample=false;                       // Inform interupt-processing that we are waiting for the next sample
}

//******************************************************************
// Timer2 Interrupt Service at 62.5 KHz
// here the audio and pot signal is sampled in a rate of:  16Mhz / 256 / 2 / 2 = 15625 Hz
// runtime : xxxx microseconds
ISR(TIMER2_OVF_vect) 
{
  gp->div32 = !gp->div32;                      // divide timer2 frequency / 2 to 31.25kHz
  if(gp->div32)
  { 
    gp->div16 = !gp->div16;     // sample channel 0-3 (CV) and 4 (audio) alternately so each channel is sampled with 15.6kHz
    if(gp->div16)               // Get CV-data now
    { 
      gp->pot_cv=ADCH;               // Store CV data detected for main-loop (will be processed with next sample from audio-in)
      cbi(ADMUX,MUX0);               // Clear all CVs... 
      cbi(ADMUX,MUX1);               // ...on input-multiplexer                
      sbi(ADMUX,MUX2);               // set multiplexer to channel 4 for audio
    }
    else                             // Get audio-data in this round
    {
      gp->cv[gp->pot_selector] = gp->pot_cv;  // store CV-data selected in last round for main loop
      gp->audio_data=ADCH;                // get ADC channel 4
      cbi(ADMUX,MUX2);                    // disable analogue input on channel 4
      
      gp->pot_selector++;                // select next Pot or CV input
      if(gp->pot_selector > 3 )          // Round-robin one of 4 possible CVs/pots
        gp->pot_selector = 0;
      ADMUX |= gp->pot_selector;        // Set current CV or Pot selected for non-audio input (round-robin)

      gp->new_sample=true;              // We selected audio and CV data now in interupt to be processed in next main loop
    }
    sbi(ADCSRA,ADSC);                   // ADC start conversion once, will be reset by the Atmega8, we have to set it each round...
  }
}
