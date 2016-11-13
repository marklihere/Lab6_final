#include "tm4c123gh6pm.h"
#include "init.h"  // Initialization routines
#include "LCD.c"   // LCD CMD/DATA / INIT routines

#define SSI_SR_RNE 0x00000004 // SSI Rx FIFO Not Empty
#define SSI_SR_TFE 0x00000001 // SSI Tx FIFO Empty
// hardcoded boundary points for red/green/yellow buttons
#define rxmax 0xBC9
#define rxmin 0x516
#define rymax 0x5E2
#define rymin 0x315
#define yxmax 0xB34
#define yxmin 0x426
#define yymax 0xC89
#define yymin 0xA65
#define gxmax 0xBA0
#define gxmin 0x5E1
#define gymax 0x7FD
#define gymin 0x670

// running store of most recent 100 presses, averaged to increase accuracy
unsigned short xarray[100];
unsigned short yarray[100];
int xtotal = 0;  // actual x-coordinate after summing and dividing by valid number of array entries if less than 100
int ytotal = 0;  // actual y-coordinate after summing and dividing by valid number of array entries if less than 100
int r = 0;  // status of red button, 0 = off, 1 = on
int g = 0;  // status of green button, 0 = off, 1 = on
int y = 0;  // status of yellow button, 0 = off, 1 = on
int i = 0;  // array index

//----------------------------------------
// Toggle LED functions
//----------------------------------------
void yellowLED(void) {
  if (y==1) {  // LED is on, turn it off, yellow is PD2
	  GPIOD->DATA |= 0x4;  // X1XX
	} else {
		GPIOD->DATA &= 0xFB; // 1011  Turns it on
	}
}

void redLED(void) {
  if (r==1) {  // LED is on, turn it off, red is PD0
	  GPIOD->DATA |= 0x1;  // XXX1
	} else {
		GPIOD->DATA &= 0xFE; // 1110  Turns it on
	}
}

void greenLED(void) {
  if (g==1) {  // LED is on, turn it off, red is PD1
	  GPIOD->DATA |= 0x2;  // XX1X
	} else {
		GPIOD->DATA &= 0xFD; // 1101  Turns it on
	}
}

//----------------------------------------
// LCD functions
//----------------------------------------
void setArea(unsigned short x1, unsigned short x2, unsigned short y1, unsigned short y2) {
  // Column Address Set 0x2A
	// columns range from 0 to 239
	writeCmd(0x2A);
	writeDat2(x1);
	writeDat2(x2);
	
	// Page Address Set 0x2B
	// pages range from 0 to 319
	writeCmd(0x2B);
	writeDat2(y1);
	writeDat2(y2);
}


void writeColor(unsigned short color) {
	int i;
	int cols;
	int rows;
	cols = 240;
	rows = 320;

  writeCmd(0x2C);
	for(i = 0; i < cols*rows; i++)
	  writeDat2(color);
}

	

// Toggle Button draw Fill functions
void fillGreen(void) {
  setArea(67, 137, 128, 193);
  if (g==0)
		writeColor(green);
	else
		writeColor(black);
}

void fillRed(void) {
  setArea(67, 137, 45, 110);
	if (r==0)
	  writeColor(red);
	else
		writeColor(black);
}
void fillYellow(void) {
  setArea(67, 137, 211, 277);
	if (y==0)
  	writeColor(yellow);
	else
		writeColor(black);
}


void GPIO_INT_INIT(void) {
	// enable interrupts
	GPIOE->IM |=0x2;             // allow pin[1] to interrupt
	NVIC->IP[4] = 0x40;          // interrupt priority 2
	NVIC->ISER[0] = 1 << 4;      // Enable interrupt 4 [GPIOE]
}

//----------------------------------------
// Touchscreen functions
//----------------------------------------
// Gets an x-coordinate from the LCD, puts into xarray[]
void getX(void) {
	// Temporary variable to store data as we read
	unsigned short data = 0;

	GPIOE->DATA &= 0xFE; // Touchscreen Chip Select, enable Tx/Rx
  sendAfterWaiting(0xD0);  // read x-coord
  data = sendAfterWaiting(0) << 5;   // sends 16-bits of 0
	data += sendAfterWaiting(0) >> 3;  // sends 16-bits of 0
	GPIOE->DATA |= 0x1; // Touchscreen Chip Select, disenable Tx/Rx
	xarray[i%100] = data;
}

// Gets an y-coordinate from the LCD, puts into yarray[]
void getY(void) {
	// Temporary variable to store data as we read
	unsigned short data = 0;
	
	GPIOE->DATA &= 0xFE; // Touchscreen Chip Select, enable Tx/Rx
  sendAfterWaiting(0x90);  // read y-coord
  data = sendAfterWaiting(0) << 5;   // sends 16-bits of 0
	data += sendAfterWaiting(0) >> 3;  // sends 16-bits of 0
	GPIOE->DATA |= 0x1; // Touchscreen Chip Select, disenable Tx/Rx
	yarray[i%100] = data;
}



// Triggers on touchscreen press
// PE1 == 0 causes this interrupt
void GPIOE_Handler(void) {
	int x = 0;
	int divideby = 0;

	//-----------------------------------------
	// GET TOUCHED COORDINATES
  // while PENIRQ is low, continuously load touchscreen values into our array
	while ((GPIOE->DATA & 0x2) == 0x0) {  // polling PE[1], the interrupt
	  getX();   
	  getY();   
	  i++;     // array index for our read X and Y coordinates
	}
	// return chip select to high
	// because this means we have already released the touchscreen and should
	// be calculating a coordinate that was touched

	//-----------------------------------------
	// PROCESS TOUCH
	// sum all array values, divide by divideby variable
	
	// check if i > 100, if so, we change our divide by value when averaging the actual position
	// because this means someone touched and held for longer than 100 read cycles
	// in which case we need to keep filling array (loop around) and then average
	// the entire array, Ex: touch & hold & drag, then we want to read the final release coordinate
	// and not the initial touch point
	if (i > 100) divideby = 100;
		else divideby = i;
	
  if (divideby!=100) {
		for(x = 0; x < i; x++) {
				xtotal += xarray[x];
				ytotal += yarray[x];
	  }
		xtotal /= i;
		ytotal /= i;
	} else {
				for(x = 0; x < 100; x++) {
				xtotal += xarray[x];
				ytotal += yarray[x];
	  }
		xtotal /= 100;
		ytotal /= 100;
	}
	// at this point xtotal and ytotal represent the actual x and y coordinates
	
	// reset array index at the end of reading an actual coordinate
	// invalidates entire temporary xarray and yarray read tables
	i = 0;
	
	//-----------------------------------------
	// update LCD
	GPIOE->DATA &= 0xFB;  // LCD ChipSelect, enable LCD Tx/Rx
		if(xtotal > rxmin && xtotal < rxmax && ytotal > rymin && ytotal < rymax) {
				fillRed();
  			redLED();
				r = !r;
		} else if (xtotal > yxmin && xtotal < yxmax && ytotal > yymin && ytotal < yymax) {
				fillYellow();
				yellowLED();
				y = !y;
		} else if (xtotal > gxmin && xtotal < gxmax && ytotal > gymin && ytotal < gymax) {
				fillGreen();
				greenLED();
				g = !g;
		}
	GPIOE->DATA |= 0x4;  // LCD ChipSelect, disable LCD Tx/Rx
	GPIOE->ICR |= 0x2;   // clear interrupt on pin [1]
}

int main(void)
{
	INIT_PLL();
	GPIO_INIT();
	INIT_SSI0();

	// Enable LCD
	GPIOE->DATA &= 0xFB; // LCD ChipSelect, enable LCD Tx/Rx
	LCD_Init();
	
	GPIO_INT_INIT();  // Enable interrupts
	
	// Draw Initial screen
	setArea(0, 239, 0, 319);
	writeColor(black);	
	setArea(62, 142, 40, 115);
	writeColor(red);
  setArea(67, 137, 45, 110);
	writeColor(black);
	setArea(62, 142, 123, 198);
	writeColor(green);
  setArea(67, 137, 128, 193);
	writeColor(black);
	setArea(62, 142, 206, 281);
	writeColor(yellow);
  setArea(67, 137, 211, 277);
	writeColor(black);
	GPIOE->DATA |= 0x4; // LCD ChipSelect, disable LCD Tx/Rx
	while(1);

 }
