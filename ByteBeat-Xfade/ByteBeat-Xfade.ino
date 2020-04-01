/*
  'GRAINS ByteBeats Crossfader' 
  an experimental Firmware for the AE Modular GRAINS module by tangible waves (using a samplerate-driven framework).
  If you want to get serious about using ByteBeats you should consider AE Modular ALGODRONE instead, this is just experemental to show the ByteBeats concept
  https://www.tangiblewaves.com

  To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
  
  Thanks for explanations and examples regarding:
  Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany 
  https://www.khm.de/home
  interface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html
  
  Backgroundinfos concerning Arduino Muxer, Registervalues etc: 
  http://www.robotplatform.com/knowledge/ADC/adc_tutorial_2.html
  http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

  Very helpful and inspiring information regarding ByteBeats by The Tuesday Night Machines:
  http://nightmachines.tv/the-absolute-beginners-guide-to-coding-bytebeats.html

  Demotrack available here: https://soundcloud.com/taitekatto/grains_bytebeat_xfade
  
  Usage of this Firmware with GRAINS:
  ===================================
  IN1 / Pot1: Select ByteBeat A
  IN2 / Pot2: Select ByteBeat B
  IN3:        (unused so far)
  Pot3:       Crossfade between ByteBeat A and B
  A:          (unused so far)
  
  OUT:        Audio out of mixture of ByteBeat A and B
  D:          (unused so far)

  Notes:
  ------
  a)  You can add or modify your own ByteBeats at GrainsIO::process() 
      after where it says: "static byte (*beats[])(long t)  // List of Bytebeats..."
      
  b)  Depending on the complexity of a ByteBeat, playback-speed may vary
      This even can be the case for one ByteBeat (let's say ByteBeat B), when you change the selection of other ByteBeat (let's say ByteBeat A)  
      Even though this feature may be considered an undesired it can be used creatively, but you'll have to find out yourself ;-)  

  c)  Long ByteBeats may not work as expeced, because they also might slow down playback speed.

  Caution! Use at your own risk (according to GNU General Public License '16. Limitation of Liability')
  ----------------------------------------------------------------------------------------------------- 
  This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears!
  Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

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

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <MIDI.h>

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
#define digital_out gp->d_out         // Set to HIGH or LOW respectively!

// #define HAIRLESS_MIDI 1            // Currently unused, delete comment-marks if MIDI-out is needed
#ifdef HAIRLESS_MIDI                  // Details concerning integrating MIDI via USB with the processor used in GRAINS see: 
MIDI_CREATE_DEFAULT_INSTANCE();       // https://projectgus.github.io/hairless-midiserial
#endif

// *** Change any code only here (if possible) to use AE Modular framework for your own GRAINS firmwares ***
// === This method will be called every time a new sample is read from analogue input, regardless if we use that data ===
void GrainsIO::process() // Programming example: Two ByteBeats that can be individually selected and crossfaded
{
static long t = 0;
  
  static byte (*beats[])()  // List of Bytebeats, Modify or add your own ByteBeats below!
  {
    []() -> byte { return t&128; },                       // This is a basic square-wave, toggelling between 0 and 128
    []() -> byte { return t * ((t>>12|t>>8)&63&t>>4); },
    []() -> byte { return t * ((t>>12|t>>8)&61&t>>11); },
    []() -> byte { return t * ((t>>12|t>>8)&59&t>>9); },
    []() -> byte { return ((t&15)*(-t&15)^!(t&16)-1)+32; },
    []() -> byte { return (t%100 >= t/200%100)*255; },
    []() -> byte { return (t%100 >= t/200%100)*255; },
  };
  // --- Calculate crossfade of two ByteBeats, using percentage-integer-math instead of floatingpoint-operations  ---
  // --- IN1/Pot1 selects ByteBeat1, IN2/Pot2 selects ByteBeat2 (both mapped to lenght of list)                   ---
  // --- Pot3 works as crossfader using integer percentage calculation (avoiding slower floatingpoint-operations  ---
  
  analog_out = ((beats[in1_pot1*((sizeof(beats)/sizeof(beats[0]))-1)/255]())*(255-pot3)*100/255) + 
               ((beats[in2_pot2*((sizeof(beats)/sizeof(beats[0]))-1)/255]())*pot3*100/255) / 100;
  t++;
}
// *******************************************************************************************************


// Sets up pins and configures the samplerate and frequency of the PWM output
void setup()
{
#ifdef HAIRLESS_MIDI
MIDI.begin();
#endif
  
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
  // digitalWrite(DIGITAL_OUT_PIN, gp->d_out);   // Unused here - delete comment-marks if needed: Send out digital value HIGH or LOW, value as set (if any) by gp->process()
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
