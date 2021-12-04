/*
# GRAINS "Bernoulli Gate"

An alternative Firmware for the AE Modular GRAINS module by tangible waves https://www.tangiblewaves.com

To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS

When the module receives a gate on input A, it tosses a (virtual) coin. Depending on the outcome
of the coin toss the gate is set to output _OUT_ or _D_

The probablity can be influenced by setting input IN1/Pot1: When IN1/Pot1 is set to the middle position, the chance of heads/tails is 50/50. The more the input is set to the left or to the right, the higher the probablity of either heads or tails as a result of the coin toss.

When Pot2 is turned to the right latch mode is enabled. In latch mode output _OUT_ or _D_ stay high until the other output gets activated.

* __Demovideo__ for Grains 'Bernoulli Gate' is available here: https://www.youtube.com/watch?v=tPaAOLuh5cg

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: Set the probability of the outcome of the coin toss (heads/tails)
* IN2 / Pot2: Switch between "normal" mode and "latch" mode. Turn Pot2 to the left for normal mode. Turn Pot2 right for latch mode.
* A:          Gate input

__Outputs__

* OUT:        Gate is on if the outcome of the coin toss is heads
* D:          Gate is on if the outcome of the coin toss is tails 

__Caution!__ Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

-------------------------------------------------------------  

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'Bernoulli Gate' an alternative Firmware for the AE Modular GRAINS module by tangible waves

Copyright (C) 2021  Stefan Dudde

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

void setup()
{
  pinMode(A1, INPUT); // IN2 / Pot2
  pinMode(A2, INPUT); // IN1 / Pot1
  pinMode(A4, INPUT); // A
  pinMode(8, OUTPUT); // D
  pinMode(9, OUTPUT); // OUT (Mozzi mode)
  pinMode(11, OUTPUT); // OUT (Grains mode)
  
  // initialize the outport PINs
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(11, LOW);

  // initialize the pseudo-random number generator
  // based on the position of IN1 / Pot1
  randomSeed(analogRead(A2));
}


boolean gate = false;
boolean oldgate = false;

void loop()
{
  // Check for gate on GRAINS input "A" (Arduino Pin A4)
  // "analogRead()" returns values between 0 and 1023
  // If the value is bigger than 512, this is considered as "gate on"
  gate = analogRead(A4) > 512; 

  // React only if the state of input "A" changed
  if (oldgate != gate) {
    if (gate) {
      // Output probability depends on the value of "P1":
      // -> The more P1 is turned to the left, the more "OUT" will be triggered
      // -> The more P1 is turned to the right, the more "D" will be triggered
      if (random(1024) > analogRead(A2)) { 
        digitalWrite(8, HIGH); // send gate to "D"
        digitalWrite(9, LOW); digitalWrite(11, LOW);
      } else {
        // Send gate to "OUT". Switch setting to "G" or "M" does not
        // matter because both outputs are set
        digitalWrite(9, HIGH); digitalWrite(11, HIGH);
        digitalWrite(8, LOW);
      }
    } else {
      // No gate on input "A" -> set all outputs to zero in "normal" mode.
      // In "latch" mode (Pot3 turned to the right) outputs will stay high.
      if (analogRead(A1) < 512) {
        digitalWrite(8, LOW); digitalWrite(9, LOW); digitalWrite(11, LOW);
      }
    }
    oldgate = gate;
  }
}
