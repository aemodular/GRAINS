/*   
  "Spell or Speak" an alternative Firmware for the AE Modular GRAINS module by tangible waves
  https://www.tangiblewaves.com/store/p86/GRAINS.html
  The synthetic words, as used with a famous toy from the late 70th, can be triggered via Gate-impulses and selcted via CV or a pot on the GRAINS
  
  To set up the environment needed to install this firmware, 
  please refer to the AeManual for GRAINS on the AE Modular Wiki: 
  http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS

  Demotrack available here: https://soundcloud.com/taitekatto/grains-spell-or-speak
  
  This program relies on the Talkie Library by Peter Knight as modified by Jean-Luc Deladrière with "Talko Counter for Grains",
  which is also included in the folder of this firmware. 
  The orinal "TALKO COUNTER" also including talkie.cpp and talkie.h of this version of the Talkie Library can be found here:
  https://www.ginkosynthese.com/grains-codes
  The phonems used with this firmware as provided via "Vocab_US_TI99.c" and others can be found with the talkie-library for Arduino here:
  https://www.arduinolibraries.info/libraries/talkie 
  
  Note: if you want to adjust the way Pitch and Speed of the talking-voice can be modified please have a look at "talkie.cpp" 
  in the current folder and look for the values mapped via "analogRead(0)" and "analogRead(1)".
  
  Usage of this Firmware with GRAINS:
  ===================================
  IN1 / Pot1: Select word from available phonems (via CV or pot)
  IN2 / Pot2: Change the Pitch of the currently selected word (via CV or pot)
  IN3:        GATE to trigger the currently selected word
  Pot3:       Change the Speed of the currently selected word (via pot)
  A:          If analog input is used more or less random values can be applied to modify the current word-selection
  
  OUT:        Audio out of spoken words or letters
  D:          Quantized value (HIGH or LOW) of the input given via analog input, this may be used as GATE/trigger for other AE Modular modules
  
  Caution! Use at your own risk (according to GNU General Public License v2 '12. No Warrenty')
  -------------------------------------------------------------------------------------------- 
  This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears!
  Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

  'Spell or Speak' an experimental Firmware for the AE Modular GRAINS module by tangible waves
    
  Copyright (C) 2020  Mathias Brüssel

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License v2 as published by
  the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
*/

#include <Arduino.h>
#include "Talkie.h"
#include "Vocab_US_TI99.h"      // Texas Instruments phonems, similar to those used with the famous "Speak&Spell" and other devices

Talkie voice;                   // Prepare speach - Use PIN 11 only, because this is connected as audio out with tangible waves GRAINS module

static uint8_t* words[323] =    // List of words to be select from via GRAINS CV ins and/or pots
{
  spt_A,          // 0
  spt_ABOUT,
  spt_AFTER,
  spt_AGAIN,
  spt_ALL,
  spt_AM,
  spt_AN,
  spt_AND,
  spt_ANSWER,
  spt_ANY,
  spt_ARE,
  spt_AS,
  spt_ASSUME,
  spt_AT,
  spt_B,            // 14
  spt_BACK,
  spt_BASE,
  spt_BE,
  spt_BETWEEN,
  spt_BLACK,
  spt_BLUE,
  spt_BOTH,
  spt_BOTTOM,
  spt_BUT,
  spt_BUY,
  spt_BY,
  spt_C,            // 26
  spt_CAN,
  spt_CASSETTE,
  spt_CENTER,
  spt_CHECK,
  spt_CHOICE,
  spt_CLEAR,
  spt_COLOR,
  spt_COME,
  spt_COMES,
  spt_COMMA,
  spt_COMMAND,
  spt_COMPLETE,
  spt_COMPUTER,
  spt_CONNECTED,
  spt_CONSOLE,
  spt_CORRECT,
  spt_COURSE,
  spt_CYAN,
  spt_D,            // 45
  spt_DATA,
  spt_DECIDE,
  spt_DEVICE,
  spt_DID,
  spt_DIFFERENT,
  spt_DISKETTE,
  spt_DO,
  spt_DOES,
  spt_DOING,
  spt_DONE,
  spt_DOUBLE,
  spt_DOWN,
  spt_DRAW,
  spt_E,            // 59
  spt_EACH,
  spt_EIGHT,
  spt_EIGHTY,
  spt_ELEVEN,
  spt_ELSE,
  spt_END,
  spt_ENTER,
  spt_ERROR,
  spt_EXACTLY,
  spt_EYE,
  spt_F,            // 70
  spt_FIFTEEN,
  spt_FIFTY,
  spt_FIGURE,
  spt_FIND,
  spt_FINE,
  spt_FINISH,
  spt_FINISHED,
  spt_FIRST,
  spt_FIT,
  spt_FIVE,
  spt_FOR,
  spt_FORTY,
  spt_FOUR,
  spt_FOURTEEN,
  spt_FOURTH,
  spt_FROM,
  spt_FRONT,
  spt_G,            // 88
  spt_GAMES,
  spt_GET,
  spt_GETTING,
  spt_GIVE,
  spt_GIVES,
  spt_GO,
  spt_GOOD,
  spt_GOODBYE,
  spt_GOT,
  spt_GRAY,
  spt_GREEN,
  spt_GUESS,
  spt_H,            // 101
  spt_HAD,
  spt_HAND,
  spt_HAS,
  spt_HAVE,
  spt_HEAD,
  spt_HEAR,
  spt_HELLO,
  spt_HELP,
  spt_HERE,
  spt_HIGHER,
  spt_HIT,
  spt_HOME,
  spt_HOW,
  spt_HUNDRED,
  spt_HURRY,
  spt_I,            // 117
  spt_IF,
  spt_IN,
  spt_INCH,
  spt_INSTRUCTION,
  spt_IS,
  spt_IT,
  spt_J,            // 124
  spt_JOYSTICK,
  spt_JUST,
  spt_K,            // 127
  spt_KEY,
  spt_KEYBOARD,
  spt_KNOW,
  spt_L,            // 131
  spt_LARGE,
  spt_LAST,
  spt_LEARN,
  spt_LEFT,
  spt_LESS,
  spt_LET,
  spt_LIKE,
  spt_LINE,
  spt_LOAD,
  spt_LONG,
  spt_LOOK,
  spt_LOWER,
  spt_M,            // 144
  spt_MADE,
  spt_MAGENTA,
  spt_MAKE,
  spt_ME,
  spt_MEAN,
  spt_MEMORY,
  spt_MESSAGE,
  spt_MIDDLE,
  spt_MIGHT,
  spt_MODULE,
  spt_MORE,
  spt_MOST,
  spt_MOVE,
  spt_MUST,
  spt_N,            // 159
  spt_NAME,
  spt_NEAR,
  spt_NEED,
  spt_NEGATIVE,
  spt_NEXT,
  spt_NINE,
  spt_NINETY,
  spt_NO,
  spt_NOT,
  spt_NOW,
  spt_NUMBER,
  spt_O,            // 171
  spt_OF,
  spt_OFF,
  spt_OH,
  spt_ON,
  spt_ONE,
  spt_ONLY,
  spt_OR,
  spt_ORDER,
  spt_OTHER,
  spt_OUT,
  spt_OVER,
  spt_P,            // 183
  spt_PART,
  spt_PARTNER,
  spt_PARTS,
  spt_PERIOD,
  spt_PLAY,
  spt_PLEASE,
  spt_POINT,
  spt_POSITION,
  spt_POSITIVE,
  spt_PRESS,
  spt_PRINT,
  spt_PRINTER,
  spt_PROBLEM,
  spt_PROGRAM,
  spt_PUT,
  spt_Q,            // 199
  spt_R,            // 200
  spt_RANDOMLY,
  spt_READ,
  spt_RECORDER,
  spt_RED,
  spt_REFER,
  spt_REMEMBER,
  spt_RETURN,
  spt_REWIND,
  spt_RIGHT,
  spt_ROUND,
  spt_S,            // 211
  spt_SAID,
  spt_SAVE,
  spt_SAY,
  spt_SCREEN,
  spt_SECOND,
  spt_SEE,
  spt_SEES,
  spt_SET,
  spt_SEVEN,
  spt_SEVENTY,
  spt_SHAPE,
  spt_SHAPES,
  spt_SHIFT,
  spt_SHORT,
  spt_SHORTER,
  spt_SHOULD,
  spt_SIDE,
  spt_SIX,
  spt_SIXTY,
  spt_SMALL,
  spt_SO,
  spt_SOME,
  spt_SORRY,
  spt_SPACE,
  spt_SPELL,
  spt_SQUARE,
  spt_START,
  spt_STEP,
  spt_STOP,
  spt_SUM,
  spt_SUPPOSED,
  spt_SURE,
  spt_T,             // 244
  spt_TAKE,
  spt_TEEN,
  spt_TELL,
  spt_TEN,
  spt_THAN,
  spt_THAT,
  spt_THE,
  spt_THEIR,
  spt_THEN,
  spt_THERE,
  spt_THESE,
  spt_THEY,
  spt_THING,
  spt_THINGS,
  spt_THINK,
  spt_THIRD,
  spt_THIRTEEN,
  spt_THIRTY,
  spt_THIS,
  spt_THREE,
  spt_THREW,
  spt_THROUGH,
  spt_TIME,
  spt_TO,
  spt_TOGETHER,
  spt_TONE,
  spt_TOO,
  spt_TOP,
  spt_TRY,
  spt_TURN,
  spt_TWELVE,
  spt_TWENTY,
  spt_TWO,
  spt_TYPE,
  spt_U,            // 279
  spt_UHOH,
  spt_UNDER,
  spt_UNDERSTAND,
  spt_UNTIL,
  spt_UP,
  spt_UPPER,
  spt_USE,
  spt_V,            // 287
  spt_VARY,
  spt_VERY,
  spt_W,            // 290
  spt_WAIT,
  spt_WANT,
  spt_WANTS,
  spt_WAY,
  spt_WE,
  spt_WEIGH,
  spt_WEIGHT,
  spt_WELL,
  spt_WERE,
  spt_WHAT,
  spt_WHEN,
  spt_WHERE,
  spt_WHICH,
  spt_WHITE,
  spt_WHO,
  spt_WHY,
  spt_WILL,
  spt_WITH,
  spt_WON,
  spt_WORD,
  spt_WORK,
  spt_WORKING,
  spt_WRITE,
  spt_X,              // 314
  spt_Y,              // 315
  spt_YELLOW,
  spt_YES,
  spt_YET,
  spt_YOU,
  spt_YOUR,
  spt_Z,              // 321
  spt_ZERO
};

void setup() 
{
  // Serial.begin(115200);            // Only needed if you want to use Serial.print() to put out values to the Arduino IDE's Serial Window
}

void loop() 
{
static bool last_trigger = true;

bool new_trigger = false;
int say_word = 0;
int last_read = 0;
int chaos = 0;
  
  if(last_trigger)                                                    // If word was triggered already look for low gate  
  {
    if( analogRead(A3) < 512 )                                        // CV-in 3 of GRAINS-module "LOW"?
      last_trigger = new_trigger = false;                             // No new gate, remember condition 
  }
  else                                                                // If no word is currently triggered we can look for new trigger
  {
    if( analogRead(A3) >= 512 ) // CV-in 3 of GRAINS-module "HIGH"?
      last_trigger = new_trigger = true;                              // We have a gate now, remember the condition                             
  }
  chaos = analogRead(A4);                                             // Analogue input, can also be used as CV-in with this Firmware of GRAINS
  if( chaos > 4 )                                                     // Value should be >= 5 if input is connected
    chaos = map( chaos, 0, 1023, 1, 160 );                            // This is a more or less random value we get via audio or CV input to be added or substracted to the index of the selected word lateron

  // --- Note: IN2/Pot2:Pitch and Pot3:Speed are adjusted in talkie.cpp via "map(analogRead(...))" as modified by Jean-Luc Deladrière for "Talko Counter for Grains" ---
  if( new_trigger )                                                   // Speak now! 
  {                                                                   
    say_word = map( analogRead(A2), 0, 1023, 0, 323 );                // CV IN 1 / Pot 1 - used to select words
    if( say_word > 161 )
      voice.say(words[constrain(say_word-chaos,0,323)]);              // Random-factor read from analog input higher than middle of available words, substract
    else
      voice.say(words[constrain(say_word+chaos,0,323)]);              // Random-factor read from analog input lowe than middle of available words, add
  }
  chaos > 80 ? digitalWrite(8, HIGH) : digitalWrite(8, LOW);          // Set digital out to low or high, depending on level of analogue input
}
