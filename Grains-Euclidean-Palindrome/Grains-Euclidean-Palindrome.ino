/*
'GRAINS Euclidean-Palindrome' 
an alternative Firmware for the AE Modular GRAINS module by tangible waves https://www.tangiblewaves.com (using a samplerate-driven framework) for algorithmic generation of melodies. 
https://www.tangiblewaves.com/store/p86/GRAINS.html
  
* __Demotrack__ for Grains 'Palindrome' is available here: https://soundcloud.com/taitekatto/grains-euclidean-palindrome

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: Select note-sequence pattern from a list (mainly palindromic numbers) to select pitch of (bamboo)samples
* IN2 / Pot2: Select rhythm pattern from a list (mainly Euclidean/Borklund rhythms) to trigger the sample
* IN3:        Select rhythm pattern from a list (mainly Euclidean/Borklund rhythms) to trigger separate gate events
* Pot3:       Select scale for scale-correction from list for pitch of (bamboo) samples (or special mode as below)
              Special-mode for Pot3: if IN1/Pot1 is set to min (only one note) Pot3 will be used to change the index of rhythm patterns 1 and 2
              -> This can be useful to reset those patterns, when set to min or to combine the rhythms of samples and gate-out interactively!
* A:          Clock input - be sure to use BEAT DEVIDER for decent results, for arbitrary speed a Sqarewave-LFO could be used instead

__Outputs__

* OUT:        Audio out of (bamboo)samples
* D:          Gate out, providing the identical or an additional rhythm to those used by the samples

## Notes

* The main source for rhythm- and note-patterns used here are Eucledian rhythms and palindromic numbers
* For the Bjorklund/Eucledian rhythm algorithm please refer to the paper 
* "The Euclidean Algorithm Generates Traditional Musical Rhythms" by Godfried Toussaint: http://cgm.cs.mcgill.ca/~godfried/publications/banff.pdf
* For palindrome numbers please have a look at: https://en.wikipedia.org/wiki/Palindromic_number
* To add your own sequence of notes, please search for "note_pattern[]" and change or add the content there
* To add your own sequence of gates, please search for "gate_pattern[]" and change or add the content there
* To add your own sequence scale-correction, please search for "scale_pattern[]" and change or add the content there
* The _Bamboo Samples_ used in this Firmware are taken from the Sample-collection that comes with the Mozzi Library, 
* the GNU General Public License v3 "or newer" is applied here, too. https://sensorium.github.io/Mozzi
* The 'b'-note had to be substitued with 'bb', you could use your own samples instead, please search for "bamboo[12]" and change the contents there.
* In order to generate your own sample-content, please use raw-audio files with signed 8bit content 
* You can convert these to the format used here with the Python script "char2mozzi.py" that is part of the Mozzi-library.

* Special mode for Pot3 as explained above, can behave strangely with long patterns, due to limited processing power, but you may use this creatively ;-)

__Tables used for the mentioned patterns__ please have a look directly after this header-section and the #defines and #includes

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
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

// --- Include Sample data, as supplied with the Mozzi Audio Library under GNU licence
#include "bamboo_00_2048_int8.h"
#include "bamboo_01_2048_int8.h"
#include "bamboo_02_2048_int8.h"
#include "bamboo_03_2048_int8.h"
#include "bamboo_04_2048_int8.h"
#include "bamboo_05_2048_int8.h"
#include "bamboo_06_2048_int8.h"
#include "bamboo_07_2048_int8.h"
#include "bamboo_08_2048_int8.h"
#include "bamboo_09_2048_int8.h"
#include "bamboo_10_2048_int8.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))       // Clearbit for Atmega8/"Arduino" registers
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))        // Setbit for Atmega8/"Arduino" registers
#define PWM_PIN           11      // DAC (PWM actually) output for Grains is fix on pin 11
#define DIGITAL_OUT_PIN   8

// --- Eucledian/Björklund rhythms - you can add you own here! ---
static char* gate_pattern[] =         // Euclidan rhythms - for further explanations please refer to: http://cgm.cs.mcgill.ca/~godfried/publications/banff.pdf
{ 
  // --- Non-Eucledian "standard patterns" ---
  "x",                                // Play each sample - please be aware: this will result in a constant Gate on
  "x.",                               // Alternate notes with pauses
  // --- The following Euclidean rhythms are Euclideanstrings --- // Comments as found in the before-mentioned paper by Godfried Toussaint (School of Computer Science, Mc Gill University Montreal, Quebec, Canada)
  "x.x..",                            // E(2,5)=[x. x . .] = (23)(classical, jazz and Persian).
  "x.x.x..",                          // E(3,7)=[x. x . x . .] = (223)(Bulgarian folk).
  "x.x.x.x..",                        // E(4,9)= [x. x . x . x . .] = (2223)(Turkey). 
  "x.x.x.x.x..",                      // E(5,11)=[x. x . x . x . x . .] = (22223)(classical).
  "x..x..x..x..x....",                // E(5,16)= [x. . x . . x . . x . . x . . . .] = (33334)(Brazilian necklace).
  // --- The following Euclidean rhythms are reverse Euclidean strings ---
  "x.x",                              // E(2,3)= [x. x]= (21)(West Africa,Latin America).
  "x.xx",                             // E(3,4)=[x. x x]= (211)(Trinidad,Persia).
  "x.x.x",                            // E(3,5)=[x. x . x]= (221)(Rumanian and Persian necklaces).
  "x..x..x.",                         // E(3,8)=[x. . x . . x .] = (332)(West Africa or Cuban tresillo also called "Habanero").
  "x.x.x.x",                          // E(4,7)=[x. x . x . x]= (2221)(Bulgaria).
  "x..x..x..x.",                      // E(4,11)= [x. . x . . x . . x .] = (3332)(Frank Zappa).
  "x.xxxx",                           // E(5,6)=[x. x x x x]= (21111)(Arab).
  "x.xx.xx",                          // E(5,7)=[x. x x . x x]= (21211)(Arab).
  "x.x.x.x.x",                        // E(5,9)=[x. x . x . x . x]= (22221)(Arab rhythm, South African and Rumanian necklaces).
  "x..x.x..x.x.",                     // E(5,12)= [x. . x . x . . x . x .] = (32322)(South Africa).
  "x.xxxxxx",                         // E(7,8)= [x. x x x x x x]= (2111111)(Tuareg rhythm of Libya).
  "x..x.x.x..x.x.x.",                 // E(7,16)= [x. . x . x . x . . x . x . x .] = (3223222)(Brazilian necklace).
  "x..x.x.x.x.x..x.x.x.x.x.",         // E(11,24)= [x. . x . x . x . x . x . . x . x . x . x . x .] = (32222322222)(Central Africa).
  // --- The following Euclidean rhythms are neither Euclidean nor reverse Euclidean strings ---
  "x.xx.xx.",                         // E(5,8)=[x. x x . x x .] = (21212)(West Africa).
  "x.xx.x.xx.x.",                     // E(7,12)= [x. x x . x . x x . x .] = (2122122)(West Africa).
  "x.xx.x.x.xx.x.x.",                 // E(9,16)= [x. x x . x . x . x x . x . x .] = (212221222)(West and Central African and Brazilian necklaces).
  "x.xx.x.x.x.x.xx.x.x.x.x."          // E(13,24)= [x. x x . x . x . x . x . x x . x . x . x . x .] = (2122222122222)(Central African necklace)
};

// --- Mainly Palindromic numbers with base 10 or 16 and some palindromic sentenses - you can add you own here! ---
static char* note_pattern[] = 
{ 
  "1",                      // Steady tone, yet palindromic
  "1357dhkquxxuqkhd7531",   // A palindronic sequence, made up with no real logic to it
  // All palindromic primes with 5 digits according to: https://oeis.org/A002385
  "10301", "10501", "10601", "11311", "11411", "12421", "12721", "12821", "13331", "13831", "13931", 
  "14341", "14741", "15451", "15551", "16061", "16361", "16561", "16661", "17471", "17971", "18181", 
  // Mutiplications giving numbers devidable by 11, up to 111111111 * 111111111
  "121", "12321", "1234321", "123454321", "12345654321", "1234567654321", "123456787654321",  "12345678987654321",  
  // Hex palindromes starting with 07 and 6 digits
  "070070", "071170", "072270", "073370", "074470", "075570", "076670", "077770", "078870", "079970", 
  "07aa70", "07bb70",  "07cc70", "07dd70", "07ee70",  "07ff70",  
  // Famous palindromic sentences
  "SoreWasIereISawEros",
  "AmanAplanAcanalPanama", 
  "NoWordNoBondRowOn", 
  "StressedWasIereIsawDesserts",
  "DoGoodsDeedsLiveonNoEvilsDeedsDoOgod",
  "DocNoteIdissentAfastNeverPreventsAfatnessIdietOnCod",
  "MadamInEdenImAdam"
};   

// --- List of patterns for scale-correction, you can add your own here --- 
static byte scale_pattern[][12] =     
{ 
  {0,1,2,3,4,5,6,7,8,9,10,11},        // Chromatic (no change to original notes pattern)
  {0,0,2,2,4,5,5,7,7,9,9,11},         // Major
  {0,0,2,2,5,5,7,7,7,9,9,9},          // Slendro
  {0,0,2,3,3,5,5,7,7,9,9,11},         // Minor
  {2,2,2,3,3,5,5,8,8,9,10,10},        // Pelog
  {0,0,2,2,3,3,5,5,7,7,8,9},          // Dorian
  {0,0,2,2,2,4,4,6,7,7,9,11},         // Lydian
  {0,1,1,3,3,5,5,7,8,8,10,10},        // Phrygian
  {0,0,0,0,5,5,5,5,5,9,9,9},          // Added forth and sixth       
  {0,0,0,0,0,7,7,7,7,7,0,0},          // Octaves and fifth only
  {9,9,9,9,9,9,9,9,9,9,9,9},          // Sixth only
  {0,0,2,2,4,4,4,7,7,7,9,9}           // Pentatonic
}; 

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
int GrainsIO::d_out = LOW;

static GrainsIO gio;  // Instanciate our Grains-IO-class globally, so it can be accesse via class-members, interupt-funtions and the main Arduino-loop()
static GrainsIO* gp = &gio;    // All access to the class-variables above for convenience is done by this class-pointer

// *******************************************************************************************************
// *** use the variable-names defined below with your GrainsIO::process() method!
#define in1_pot1    gp->cv[2]
#define in2_pot2    gp->cv[1]
#define in3         gp->cv[3]
#define pot3        gp->cv[0]
#define audio       gp->audio_data
#define analog_out  gp->a_out
#define digital_out gp->d_out         // Set to HIGH or LOW respectively!

// *** Change any code starting here including GrainsIO::process() (if possible) to use AE Modular framework for your own GRAINS firmwares ************************
bool new_clock(byte clock_cv)       // Return true if clock encountered while not active yet, false otherwise
{
static bool clock_active = false;

  if(clock_cv > 127 && !clock_active)
    return(clock_active = true);    // Clock "opened", remember it
  if(clock_cv <= 127)
    return(clock_active = false);   // Clock "closed", remember it
  return false;                     // Otherwise: No new clock encountered
}

// === This method will be called every time a new sample is read from analogue input, regardless if we use that data ===
void GrainsIO::process()  // Generate (scale-corrected) meolodies as combination of Gate-rhythms and notes from two lists, plus an extra rythm as gate...
{
static const byte note_pattern_size = sizeof(note_pattern)/sizeof(note_pattern[0]);
static const byte gate_pattern_size = sizeof(gate_pattern)/sizeof(gate_pattern[0]);
static const byte scale_pattern_size = sizeof(scale_pattern)/sizeof(scale_pattern[0]);
static const byte note_pattern_max_idx = note_pattern_size-1;
static const byte gate_pattern_max_idx = gate_pattern_size-1;
static const byte scale_pattern_max_idx = scale_pattern_size-1;
                                                                                           
static byte note_pat_idx = 0;   // Index for one entry from note-patterns, will be selected via Pot later!
static byte gate_pat_idx = 0;   // Index for one entry from gate-patterns to trigger samples, will be selected via Pot later!
static byte gate2_pat_idx = 0;  // Index for one entry from gate-patterns for Gate on/off, will be selected via Pot later!

static byte scale_pat_idx = 0;  // Index for one entry from scale-patterns, will be selected via Pot later!

static byte notes_len = min(255, strlen(note_pattern[note_pat_idx]) );     // will be selected via Pot later!
static byte gates_len = min(255, strlen(gate_pattern[gate_pat_idx]) );     // Gate for sample-notes, will be selected via Pot later! 
static byte gates2_len = min(255, strlen(gate_pattern[gate2_pat_idx]) );   // Extra gate for Gate HIGH/LOW

static byte note_idx = 0;       // Index of "Beat to be played" currently
static byte gate_idx = 0;       // Index of "Sample-Note to be played" currently
static byte gate2_idx = 0;      // Index of "Gate to be played" currently

static int sample_idx = 9999;   // we will play the current sample if index is below max size of sample array, which is up to 4096
static byte bamboo_idx = 0;     // Bamboo-sample to be selected via note ('0'-'a') from pattern
static char* bamboo[12] = { &BAMBOO_00_2048_DATA[0], &BAMBOO_01_2048_DATA[0], &BAMBOO_02_2048_DATA[0], &BAMBOO_03_2048_DATA[0],   // List of 11 bamboo-samples
                            &BAMBOO_04_2048_DATA[0], &BAMBOO_05_2048_DATA[0], &BAMBOO_06_2048_DATA[0], &BAMBOO_07_2048_DATA[0],
                            &BAMBOO_08_2048_DATA[0], &BAMBOO_09_2048_DATA[0], &BAMBOO_10_2048_DATA[0], &BAMBOO_10_2048_DATA[0]    };
static char* bamboo_ptr = bamboo[0][0];             // Pointer to beginning of currently selected sample, will change with each "note" of pattern

static unsigned int counter = 0;  // In combinatin with octave_factor we decide how often per loop (about 16khz frequency) we playback one sample byte
static byte octave_factor = 1;    // Lowest octave: every 3 loops, middle octave, every two loops, upper octave every loop one samply-byte is processed
static byte last_gate_idx = 0;    // We need this for the alternative "change circle-index of Björklund-Pattern 1" of in3_pot3
static byte last_gate2_idx = 0;   // We need this for the alternative "change circle-index of Björklund-Pattern 2" of in3_pot3

  // --- This is where we actually generate the audio of the mallet samples (if to be played) ---
  if( sample_idx < 2047 )         // Playback next byte of sample until end of sample reached
  {
    if(++counter%octave_factor == 0)  // We select the octave (upper, mid or lower) to be played by either playing every round, every second or every first          
      analog_out = pgm_read_byte(bamboo_ptr+(sample_idx++))+127;  // Playback next byte of sample selected per note before (on clock-change)
  }
  else
    analog_out = 128;             // Sample ended, but maybe next clock not reached yet, so playback silence (DC-shifted 0)

  // --- Play back gate pattern as ring-loop and switch to next note from ring when required ---
  if( new_clock(audio) )                            // Clock has to get in via audio-in
  {                                                 // We have a new note or pause to play (high or low gate) with each clock!
    note_pat_idx = min( note_pattern_max_idx, in1_pot1 * note_pattern_size / 255);  // Select note-pattern depending on setting of Pot1 
    gate_pat_idx = min( gate_pattern_max_idx, in2_pot2 * gate_pattern_size / 255);  // Select gate-pattern depending on setting of Pot2 
    gate2_pat_idx = min( gate_pattern_max_idx, in3 * gate_pattern_size / 255);      // Select second gate-pattern depending on setting of in2 
    scale_pat_idx = min( scale_pattern_max_idx, pot3 * scale_pattern_size / 255);   // Select scale-pattern depending on setting of Pot3
    gates_len = min(255, strlen(gate_pattern[gate_pat_idx]) );    // We have to calculate the number of steps in the gates pattern to calculate ring-index!
    gates2_len = min(255, strlen(gate_pattern[gate2_pat_idx]) ); // Also calculate lenght of second active gate-pattern
    notes_len = min(255, strlen(note_pattern[note_pat_idx]) );  // We only calculate number of notes for pattern with new clock to gain performance

    // --- We decide here if we use Pot3 for scale selection or to set the index of the Björklund-patterns, in other words shift the circle of a string of Euclidean rhythm-events ---
    if(!note_pat_idx)  // in1_pot1 is "low"/all way left, we switch to select index of Björklund-string instead of scale, which is not needed, because only one note will be played
    {
      byte cur_gate_idx = pot3 * (gates_len-1) / 255;                       // Calculate potential setting of Gate-pattern 1 depending on Pot3
      byte cur_gate2_idx = pot3 * (gates_len-1) / 255;                      // Calculate potential setting of Gate-pattern 2 depending on Pot3

      if( cur_gate_idx!=last_gate_idx && cur_gate2_idx!=last_gate2_idx )    // Check if Pot3 has a different setting
      {
        last_gate_idx = gate_idx = cur_gate_idx;                            // Set new index of Gate-pattern 1, twist pot3 all way left to reset index
        last_gate2_idx = gate2_idx = cur_gate2_idx;                         // Set new index of Gate-pattern 2, twist pot3 all way left to reset index
      }               // "else": we leave the incrementing of the gate-patterns as it is normally, so no resetting of the index of those patterns!
    }                 // "else": we stick with Pot3 changing the scales... - because Note-Pattern one consists of one note only we actually don't have to care about scales, here!

    // --- Here we mainly find out which note to play ---
    if( tolower(gate_pattern[gate_pat_idx][gate_idx]) == 'x' )  // Note to be played, "active" gate pattern event
    {
      isdigit(note_pattern[note_pat_idx][note_idx]) ? bamboo_idx = note_pattern[note_pat_idx][note_idx]-'0' : bamboo_idx = (note_pattern[note_pat_idx][note_idx]-'a')+10;
      sample_idx = 0;                           // start to Playback sample now, else we'll have silence (maybe after playing last sample to end or
      counter = 0;                              // Reset counter that decides on octaves during playback loop (see above)
      octave_factor  = constrain(3-bamboo_idx/12, 1, 3);  // Will be 1, 2 or 3, depending on octave (3: lowest, 2: middle, 1: upper);
      analog_out = pgm_read_byte(bamboo_ptr=bamboo[scale_pattern[scale_pat_idx][bamboo_idx%12]])+127; // Read first byte of scale-corrected sample, sample is given as -127 to 128, apply DC-offset for our Arduino-logic
      note_idx = ++note_idx % notes_len;          // Set new note to be played now, use index within ring of note-events for next round
    }
    else
      analog_out = 128;                           // No sample to be played, set to relative 0 (DC offset is 127 for "Arduino-Logic") also valid for CV mode

    // --- Decide if we have a low or high gate - please note, that in contrast to sample a gate is only opened if a trigger event is not followed by another one ---  
    if( tolower(gate_pattern[gate2_pat_idx][gate2_idx]) == 'x' )  // Separate Gate to be HIGH or LOW from gate2 pattern event
      digital_out = HIGH;                         // Open gate (Gate is closed on init) Note: we can play samples without Gate-trigger, but we need it for the optional CV-out for sure! 
    else                                          // "Inactive" gate pattern event, normally a '.'
      digital_out = LOW;                          // Close Gate

    // --- Increment the gate-counters for next round! ---
    gate_idx = ++gate_idx % gates_len;            // Remember position for next round, use index within ring of gate-events for (Sample-)notes 
    gate2_idx = ++gate2_idx % gates2_len;         // Remember position for next round, use index within ring of gate-events for Gate-events
  }
}
// *******************************************************************************************************


// Sets up pins and configures the samplerate and frequency of the PWM output
void setup()
{
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
      gp->audio_data=ADCH;              // get ADC channel 4
      cbi(ADMUX,MUX2);                  // disable analogue input on channel 4
      
      gp->pot_selector++;               // select next Pot or CV input
      if(gp->pot_selector > 3 )         // Round-robin one of 4 possible CVs/pots   // reduced to 3 inputs, we need in3 as CV out, here! 
        gp->pot_selector = 0;
      ADMUX |= gp->pot_selector;        // Set current CV or Pot selected for non-audio input (round-robin)

      gp->new_sample=true;              // We selected audio and CV data now in interupt to be processed in next main loop
    }
    sbi(ADCSRA,ADSC);                   // ADC start conversion once, will be reset by the Atmega8, we have to set it each round...
  }
}
