/*
# GRAINS "Midi out"
an alternative Firmware for the AE Modular GRAINS module by tangible waves (using a samplerate-driven framework) 
providing the option to send out MIDI control values via USB

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

* OUT:        (unused so far)
* D:          (unused so far)

## Notes

The Atmega processor used in GRAINS is not capable to send out MIDI via USB directly, but luckily serial data can be converted to MIDI easily
A popular way to do this is "Hairless MIDI<->Serial Bridge", download and documentary for MacOS/Windows/Linux available here:
https://projectgus.github.io/hairless-midiserial/

* If you want to use this MIDI data as input for your DAW on your PC you will need a MIDI-loopback device in software or route MIDI-out to MIDI-in with your hardware
* Please make sure to close Hairless MIDI when you want to upload this Firmware to GRAINS (uses the same USB-Serial-Port)
* You can use up to 5 pots to send out MIDI CC for instance by setting 2ATT/CV to 5V and sending the votages to IN3 and A (analogue input)
* Attaching controls in your DAW can be a bit tricky: set all knobs to 0 and only dial in one pot to max and back to zero at a time

To avoid the complexity of additional libraries, we use our own midi_sendControlChange() method here instead of the standard MIDI.sendControlChange().
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
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// ### #include <MIDI.h>

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
    
    // Main processing "callback" - this is were all user-code for this framework should go it possible.  
    static void GrainsIO::process();

    static byte a_out;     // Analogue output has to go here
    static int d_out;     // Digital output has to go here
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

// ### #define HAIRLESS_MIDI 1               // Comment out if MIDI is not needed
// ### #ifdef HAIRLESS_MIDI
// ### MIDI_CREATE_DEFAULT_INSTANCE();
// ### #endif

// *** Change any code only here (if possible) to use AE Modular framework for your own GRAINS firmwares ***

void midi_sendControlChange(byte cc, byte value, byte channel)
{
  Serial.write(176);      // 0xB0 == CC on channel 0/1
  Serial.write(cc);       // CC Number
  Serial.write(value);    // CC Value
}

byte interpolate(byte cv_num, byte value, byte idx)
{
static byte interpol[5][30];
int i = 0;
unsigned long int cc_val_sum = 0;
byte result = 0;

  interpol[cv_num][idx] = value;

  for(i=0; i<30; i++)
    cc_val_sum = interpol[cv_num][i]+cc_val_sum;

  result = (byte)(cc_val_sum/30);
  if( result <= 9 )                 // Some pots send odd values at start
    result = 0;
  return( result/2 );
}

// === This method will be called every time a new sample is read from analogue input, regardless if we use that data ===
void GrainsIO::process() // Programming example: Send MIDI via serial over USB to be converted by Hairless-Midi Bridge http://projectgus.github.io/hairless-midiserial/
{             
static unsigned int counter = 0;
static unsigned int pol_counter = 0;

static byte last_in1_pot1 = 0;  // used to send MIDI.sendControlChange(controlNumber, controlValue, channel);
static byte last_in2_pot2 = 0;
static byte last_in3 = 0;
static byte last_pot3 = 0;
static byte last_audio = 0;

byte new_val = 0;
byte pol_idx = 0;

   if((++counter%5) == 0)                       // Slow down midi send rate a bit - change to fit your needs!
   {
      pol_idx = ++pol_counter%30;
      
      new_val = interpolate(0, in1_pot1, pol_idx);
      if( last_in1_pot1 != new_val )
      {
        midi_sendControlChange(21, new_val, 1);     // MIDI.sendControlChange(controlNumber, controlValue, channel);
        last_in1_pot1 = new_val;
      }
      new_val = interpolate(1, in2_pot2, pol_idx);
      if( last_in2_pot2 != new_val )
      {
        midi_sendControlChange(22, new_val, 1);     // MIDI.sendControlChange(controlNumber, controlValue, channel);
        last_in2_pot2 = new_val;
      }
      new_val = interpolate(2, in3, pol_idx);
      if( last_in3 != new_val )
      {
        midi_sendControlChange(23, new_val, 1);     // MIDI.sendControlChange(controlNumber, controlValue, channel);
        last_in3 = new_val;
      }
      new_val = interpolate(3, pot3, pol_idx);
      if( last_pot3 != new_val )
      {
        midi_sendControlChange(24, new_val, 1);     // MIDI.sendControlChange(controlNumber, controlValue, channel);
        last_pot3 = new_val;
      }
      new_val = interpolate(4, audio, pol_idx);
      if( last_audio != new_val )                     // We allow audio-input as CV to be converted to MIDI here as well.
      {
        midi_sendControlChange(25, new_val, 1);     // MIDI.sendControlChange(controlNumber, controlValue, channel);
        last_audio = new_val;
      }
   }
}
// *******************************************************************************************************

// Sets up pins and configures the samplerate and frequency of the PWM output
void setup()
{
// ### #ifdef HAIRLESS_MIDI
// ### MIDI.begin();
// ### #endif
  
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
  digitalWrite(DIGITAL_OUT_PIN, gp->d_out);   // Send out digital value HIGH or LOW, value as set (if any) by gp->process()
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
