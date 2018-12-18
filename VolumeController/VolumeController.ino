/*************************************************************
    Dual Digital Pot (100K)
     <https://www.dfrobot.com/wiki/index.php/Dual_Digital_Pot_(100K)_SKU:_DFR0520>
 *************************************************************
   This example serves as a template to control the MCP42100 dual
   digital pot through 3-wire SPI.

   Created 2017-8-31
   By Henry Zhao <Henry.zhao@dfrobot.com>

   GNU Lesser Genral Public License.
   See <http://ww.gnu.org/licenses/> for details.
   All above must be included in any redistribution.
 ************************************************************/

/********************Device Inctrduction**********************
   The MCP42100 has dual potentiometer x (x=0,1).
     Ax - Potenriometer terminal Ax
     Wx - Potenriometer Wiper
     Bx - Potenriometer terminal Bx

     SI - Serial Data Input
     SCK - Serial Clock
     CS - Chip Select

     The MCP42100 is SPI-compatible,and two bytes should be sent to control it.
     The first byte specifies the potentiometer (POT0: 0x11, POT1: 0x12, both: 0x13).
     The second byte specifies resistance value for the pot (0 - 255).
 ************************************************************/

/***********************Circuit Connections*******************
     Digital Pot | Arduino UNO R3
         CS      |    D10 (SS)
         SI      |    D11 (MOSI)
         CLK     |    D13 (SCK)
         VCC     |      VCC
         GND     |      GND
 ************************************************************/

/***********************Resistances Calculation**************
   Rwa(Dn) =Rab*(256 - Dn) / 256 + Rw
   Rwb(Dn) =Rab*Dn / 256 + Rw

   Rwa - resistance between Terminal A and wiper W
   Rwb - resistance between Terminal B and wiper W
   Rab - overall resistance for the pot （=100KΩ, typical）
   Rw - wiper resistance （=125Ω,typical; =175Ω max）
   Dn - 8-bit value in data register for pot number n （= 0 - 255）
 ************************************************************/

/***********************Notice********************************
   1.Resistor terminals A, B and W have no restrictions on 
     polarity with respect to each other.
   2.Current through terminals A, B and W should not excceed ±1mA.
   3.Voltages on terminals A, B and W should be within 0 - VCC.
************************************************************/

#include <SPI.h>

/***********************PIN Definitions*************************/
// set pin 4 as the slave select (SS) for the digital pot:
// for using the SD SPI interface of Gravity IO Expansion Shield for Arduino V7.1 
//const int CS_PIN = 4;

// set pin 10 as the slave select (SS) for the digital pot 
// for using Arduino UNO
const int CS_PIN = 10;

const int coil = 7;  //Digital
const int buttonIncrease = 9;  //Digital
const int buttonDecrease = 8;  //Digital

uint32_t debounceMillis = 500;

uint8_t outputLowerThreshold = 0;
uint8_t outputUpperThreshold = 250;

/***********************MCP42XXX Commands************************/
//potentiometer select byte
const int POT0_SEL = 0x11;
const int POT1_SEL = 0x12;
const int BOTH_POT_SEL = 0x13;

//shutdown the device to put it into power-saving mode.
//In this mode, terminal A is open-circuited and the B and W terminals are shorted together.
//send new command and value to exit shutdowm mode.
const int POT0_SHUTDOWN = 0x21;
const int POT1_SHUTDOWN = 0x22;
const int BOTH_POT_SHUTDOWN = 0x23;

/***********************Customized Varialbes**********************/
//resistance value byte (0 - 255)
//The wiper is reset to the mid-scale position upon power-up, i.e. POT0_Dn = POT1_Dn = 128
int POT0_Dn = 255;
int POT1_Dn = 255;
int BOTH_POT_Dn = 255;

uint8_t output = 250;

//Function Declaration
void DigitalPotTransfer(int cmd, int value);     //send the command and the resistance value through SPI

void setup()
{
  pinMode(coil, OUTPUT);
  pinMode(buttonIncrease, INPUT);
  pinMode(buttonDecrease, INPUT);
  pinMode(CS_PIN, OUTPUT);
  SPI.begin();     // initialize SPI:
  DigitalPotWrite(BOTH_POT_SHUTDOWN, BOTH_POT_Dn);
  DigitalPotWrite(POT1_SEL , output);
}

// 255 - 450ohm
// 225 - 10k
// Step: 10
// 0 - 25 - 50 - 75 - 100 - 125 - 150 - 175 - 200 - 225 - 250

void loop()
{
  // Increase Volume
  if((digitalRead(buttonIncrease) == 0)){
    if((output <= outputUpperThreshold) && (output > outputLowerThreshold)){
      output -= 25;
      if(output != outputUpperThreshold)
        digitalWrite(coil, HIGH);
      DigitalPotWrite(POT1_SEL , output);
      delay(debounceMillis);
    }
  }
  // Decrease Volume
  if(digitalRead(buttonDecrease) == 0){
    if((output < outputUpperThreshold) && (output >= outputLowerThreshold)){
      output += 25;
      DigitalPotWrite(POT1_SEL , output);
      if(output == outputUpperThreshold)
        digitalWrite(coil, LOW);
      delay(debounceMillis);
    }
  }
}

void DigitalPotWrite(int cmd, int val)
{
  // constrain input value within 0 - 255
  val = constrain(val, 0, 255);
  // set the CS pin to low to select the chip:
  digitalWrite(CS_PIN, LOW);
  // send the command and value via SPI:
  SPI.transfer(cmd);
  SPI.transfer(val);
  // Set the CS pin high to execute the command:
  digitalWrite(CS_PIN, HIGH);
}
