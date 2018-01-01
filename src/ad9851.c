#include <stdio.h>
#include <math.h>
#include <wiringPi.h>

/*     DDS generator for the AD9851 using Raspberry Pi
       Use with the Multiplier Activated
       Alfredo Mendiola
		   License: MIT
*/

// Power 5V supplied from PIN 2
// GND supplied from PIN 6
// Use a Level converter to connect the AD9851 with the RaspBerry Pi GPIO pins.

#define DDS_CLOCK	180000000UL
#define FREQ_MAX	70000000UL

#define	FQ_UD	25	//GPIO 26 (pin 37)
#define CLOCK	28	//GPIO 20 (pin 38)
#define DATA	29	//GPIO 21 (pin 40)

unsigned char W0 = 0x01; //0x01; //0x05 power down

unsigned long freqValue = 10000000UL;

void outZero (void)
{
  digitalWrite (CLOCK, LOW);
  digitalWrite (DATA, LOW);
  digitalWrite (CLOCK, HIGH);
}

void outOne (void)
{
  digitalWrite(CLOCK, LOW);
  digitalWrite(DATA, HIGH);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(DATA, LOW);
}

void byte_out(unsigned char byte)
{
  int i;
  for (i=0; i<8; i++)
  {
    if ((byte&1) == 1) 
      outOne();
    else 
      outZero();
    
    byte = byte >> 1;
  }
}

void sendFrequency(unsigned long frequency)
{
  unsigned long tuning_word = (frequency * pow(2, 32)) / DDS_CLOCK;
  int i;
  
  digitalWrite(FQ_UD, LOW);
  
  for (i=0; i < 32; i++)
	{
    if ((tuning_word & 1)==1) 
      outOne();
    else 
      outZero();
    
    delay(1);
    tuning_word = tuning_word >> 1;
	}
  
  byte_out(W0);  
  
  digitalWrite(FQ_UD, HIGH); // Take FQ_UD pin high again
}

void setup(void)
{
  pinMode (DATA, OUTPUT);
  pinMode (CLOCK, OUTPUT);
  pinMode (FQ_UD, OUTPUT);
  
	digitalWrite(FQ_UD, LOW);
	digitalWrite(CLOCK, LOW);
	digitalWrite(DATA, LOW);

  //Initialize the DDS chip (Figure 17. Serial Load Enable Sequence)
	delay(2);
	digitalWrite(CLOCK, HIGH);  //Strobe Clock to get hardware bits D0, D1 and D2 into the input register (Figure 17 DataSheet)
	digitalWrite(CLOCK, LOW);
	digitalWrite(FQ_UD, HIGH);   //And raise FQ_UD  to get them into the control register  
  
  //Clear DDS registers and set 6x multiplier mode by write 32 zeros and 0x01
	delay(2);
	digitalWrite(FQ_UD,LOW);     //Drop the FQ_UD to start the programming sequence 
  
	digitalWrite(DATA, LOW);    //Write 32 zeros
	
  int i;
  for (i = 0; i < 32; i++)  
	{
		digitalWrite(CLOCK, HIGH);
		digitalWrite(CLOCK, LOW);
		delay(2);
	}
  
  byte_out(W0);            //Write 0x01 to set 6x Multiplier and complete the init sequence
  digitalWrite(FQ_UD,HIGH);  //Raise the FQ_UD line to load the DDS registers and complete the process  
  
}

int main (int argc, char *argv[])
{
  if (argc < 2) 
  { 
    printf ("Include the Frequency Parameter.\n"); 
    return -1; 
  }
  sscanf (argv[1], "%lu", &freqValue);
  
  if (freqValue < 0 || freqValue > FREQ_MAX) 
  { 
    printf ("Error: Invalid Frequency range\n"); 
    return -1; 
  }
  printf ("Sending frequency: %lu\n", freqValue);
 
  if (wiringPiSetup () == -1) 
  { 
    printf ("Error: Wiring Pi Setup.\n"); 
    return -1; 
  }
  
  setup();
  
  sendFrequency(freqValue);
  
  return 0;
}
