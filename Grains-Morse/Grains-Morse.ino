/*
  'GRAINS Morse Code' 
  an experimental Firmware for the AE Modular GRAINS module by tangible waves (using a samplerate-driven framework).
  https://www.tangiblewaves.com/store/p86/GRAINS.html
  
  Morsecode playback can be started and then will be played back in a loop or play once.
  On startup the Morsecode for "AEM" (AE Modular) will be sent out as Gate-events via GRAINS' digital output. 
  To hear this, for instance sent digital out to 2VCA and any Oscillator to the same VCA as audio-material.
  To change the initial Morse-Message please search for "###" and adjust the string to your liking!
  To enter new morsecodes connect a MIDI-Keyboard and Connect Bus-Gate with IN1, Bus-CV with IN3.
  As an intial setup: "Shortcut" audio-out with audio-in, Set P1 to IN1 and max position. Set P2 to MAN and max position. Set P3 to min postion.
  The Morsecode itself can be modified and/or overwritten as explained further below.
  
  Of course the Gate-events can be used for any other usecase, like triggering 2ENV, KICK, DRUMKIT or whatever
  Even usage as a Clock-Signal may be considered for creative use!
  
  Thanks for explanations and examples regarding:
  Example-Code for Arduino-realtime-audio-processing from KHM, cologne Germany https://www.khm.de/home
  http://interface.khm.de/index.php/lab/interfaces-advanced/arduino-realtime-audio-processing/index.html
  Backgroundinfos concerning Arduino Muxer, Registervalues etc: 
  http://www.robotplatform.com/knowledge/ADC/adc_tutorial_2.html http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html

  Demotrack available here: https://soundcloud.com/taitekatto/grains-morse

  Usage of this Firmware with GRAINS:
  ===================================
  IN1 / Pot1: Gate to trigger new letter (select IN1 and put P1 to max!)
  IN2 / Pot2: Stop Morsecode if all left, Play once in middle-position, loop Morsecode when turned to max (select MAN for P2!)
  IN3:        Select letter (Note CV)     // Two upper octaves C#: '.', F#: '-', c#: word pause, g#: delete last character
  A:          Clock input (Yes, we use the audio-inut for clock-in, here!)
  Pot3:       Beat Devider (changes speed, max speed [== default] when all left).

  OUT:        Puts out a steady-rate clock-signal, use this to "shortcut" with audio-in, if you don't want to use an LFO or Bus-Clock as your clock!
  D:          Gate to trigger Morsecode on Output (connect to 2VCA for instance)

  IMPORTANT:  Please select IN1 as input and IN2 as manual, Then set Pot1 and Pot2 to their maximum value!
  Otherwise Gate-Tracking and playback of Morse-Code may not work as intended! You may change Pot2 to restart or pause the Morse-Code lateron!
   
  Notes: 
  To learn about Morsecode and see the table of Morse-characters you could put in, please refer to: 
  https://en.wikipedia.org/wiki/Morse_code
  As initial Content the String "AEM" for AE Modular will be played back as morse code
  Similar to putting in real morse code, there will be no error-checking, so if you want you can invent your own, new morse-characters ;-)
  
  Use keys of lowest 2 octaves to change content to be sent out as Morsecode to Digital-Out as Gate-CV:
  -----------------------------------------------------------------------------------------------------
  C# (or C) enter a new DIT (short morse character) - second-highest octave
  F# (or G) enter a new DAH (long morse character) - second-highest octave
  c# (or c) enter a PAUSE - (should not be more than one at a time) highest octave
  f# (or g) delete last morse-character (DIT, DAH or PAUSE) - highest octave

  Please note: in contrast to a Computer-Keyboard we have _no_ autorepeat for entering of keys, Morsedigits or pauses that is!
  -> To better understand what is happening here, we recommend you to enable the Serial Monitor from the Arduino IDE (CTRL/CMD+SHIFT+M)

  Caution! Use at your own risk (according to GNU General Public License '16. Limitation of Liability')
  ----------------------------------------------------------------------------------------------------- 
  This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears!
  Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

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
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

// #define HAIRLESS_MIDI 1            // Currently unused, delete comment-marks if MIDI-out is needed
#ifdef HAIRLESS_MIDI
#include <MIDI.h>
#endif

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

#ifdef HAIRLESS_MIDI                  // Details concerning integrating MIDI via USB with the processor used in GRAINS see: 
MIDI_CREATE_DEFAULT_INSTANCE();       // https://projectgus.github.io/hairless-midiserial
#endif

// *** Change any code only here (if possible) to use AE Modular framework for your own GRAINS firmwares ************************
bool new_gate(byte gate_cv)         // Return true if gate was activated while not active yet, false otherwise
{
static bool gate_active = false;

  if(gate_cv > 127 && !gate_active)
    return(gate_active = true);     // Gate opened, remember it
  if(gate_cv <= 127)
    return(gate_active = false);    // Gate closed, remember it
  return false;                     // Otherwise: No new gate activated
}

bool new_clock(byte clock_cv)       // Return true if clock encountered while not active yet, false otherwise
{
static bool clock_active = false;

  if(clock_cv > 127 && !clock_active)
    return(clock_active = true);    // Clock "opened", remember it
  if(clock_cv <= 127)
    return(clock_active = false);   // Clock "closed", remember it
  return false;                     // Otherwise: No new clock encountered
}

// --- Regarding the logic and terminogy of Morse-code and a Morescode-table please refer to: https://en.wikipedia.org/wiki/Morse_code ---
#define DIT             3           // Short impulse
#define DAH             9           // Long impulse == 3 DITs
#define SYMBOL_PAUSE    3           // one DIT
#define WORD_PAUSE      18          // 7 DITs, but: we add one DIT == SYMBOL_PAUSE after every Symbol by default, so we are on DIT short here ;-)      

#define note_cv     gp->cv[3]       // in3: Note-CV
#define gate_cv     gp->cv[2]       // in1_pot1: Gate to trigger new letter
#define pot2        gp->cv[1]       // in2_pot2: Stop if all left, play once if middle-pos, loop if max.
#define clock_cv    gp->audio_data  // audio-input: Clock signal
#define beatdiv_pot gp->cv[0]       // pot3: Beat Devider

// === This method will be called every time a new sample is read from analogue input, regardless if we use that data ===
void GrainsIO::process() // Programming example: Generate morse-code via Gate out
{
static bool first = true;
static long clock_difference = 0;
static int letter_idx = 0;
static bool process_symbol_pause = false; // we add one DIT as a pause directly after each symbol!
byte note = 0;
static int char_idx=0;
static char buf[512] = ".-_._--_";      // ### Initial content: "AEM" You can overwrite this with the back-key: f# and add any new letters
                                        // ===### Note: you also can change the initial string if you want! ###===
static byte internal_clock = 0;         
static bool pause_playing = false;      // Will be true if looping is off (Pot 2 at middle-position) and we have reached the end of our Morse-Code
static bool stopped = false;            // Will be true if Pot 2 all way left to stop playing of morese-code (middle will play once, max will loop)

  if(first)
  {
    char_idx = strlen(buf);             // Set length or Morsecode to be played back depending on inital content
    Serial.println(buf);                // Display initial content to Serial Monitor
    first = false; 
  }
  // --- Add Dits, Dahs, a Pause or delete last Morse-character
  if( new_gate(gate_cv) )       // new gate experienced? Probably new letter was put in...
  {
    if(note_cv>130 && note_cv<=160)           // C or C# (second-highest octave)
      note='.';           // Dit
    else if(note_cv>160 && note_cv<=190)     // F# or G (second-highest octave)
      note='-';           // Dah
    else if(note_cv>190 && note_cv<=220)     // c or c# (highest octave)
      note='_';           // letter-pause
    else if(note_cv>220)                    // f# or g (highest octave)
      note='#';           // word-pause
    
    if( strchr(".-_", note ) )        // Morse-Character contained in list?
    {
      buf[char_idx] = note;           // Triggered by C#, F#, c#, f# {15,40,80}: '.', '-' or '_' DIT, DAH, WORD_PAUSE
      if( char_idx < sizeof(buf)-2)   // Don't overwrite buffer!
        char_idx++;
    }
    else if(note == '#' && char_idx > 0 )      // Delete last character?
    {
      char_idx--;
      buf[char_idx] = '\0';
    }
    Serial.println(buf);            // Show current Morse code on Serial Monitor
  }
  // --- Play back Morsecode that as available now ---
  if( new_clock(clock_cv) )
  {
    if( clock_difference <= 0 )
    {
      switch(buf[letter_idx])
      {
        case '.':             // DIT
          digital_out = HIGH;
          clock_difference = DIT * (beatdiv_pot/30 +1);
          process_symbol_pause = true;
          break;
  
        case '-':             // DAH
          digital_out = HIGH;
          clock_difference = DAH * (beatdiv_pot/30 +1);
          process_symbol_pause = true;
          break;  
        
        default:
          digital_out = LOW;
          clock_difference = WORD_PAUSE * (beatdiv_pot/30 +1);
          break; 
      }
    }
    if( pot2 > 200 || (pot2>20 && stopped) )                   // About max-position of Pot2: loop! or restart after stop
    {
      pause_playing = false;
      stopped = false;
    }
    if( pot2 <= 20 )          // Pot 2 all left => Stop
    {
      stopped = true;
      letter_idx = 0;
      digital_out = LOW;
    }
    else                      // Not stopped, play once or loop
    {
      if(pause_playing)
        digital_out = LOW;
      else
      {
        clock_difference--;
        if(clock_difference <= 0)
        { 
          if(process_symbol_pause)
          {
            process_symbol_pause = false;
            digital_out = LOW;
            clock_difference = SYMBOL_PAUSE;
          }
          else                  // No pause, we have a Dit or a Dah
          {
            if(letter_idx >= strlen(buf)-1 ) // End of Morse-digits in "text-buffer" reached
            {
              letter_idx = 0;                // Reset / prepare to loop if looping active 
              if( pot2 > 20 && pot2 < 200)   // Middle-position of pot2, play only once!
                pause_playing = true;        // Morse-code played once and no looping expected
            }
            else if( letter_idx <= sizeof(buf)-2 )   // make sure we don't overwrite the max. lenght of the textbuffer for Morse-digits!
              letter_idx++;
          }
        }
      }
    }
  }
  (++internal_clock < 128) ? analog_out = 0 : analog_out = 254;   // Via overflow this byte generate values below and above 127, so that we have a gate on/off like clock-output
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
