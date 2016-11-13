#include "tm4c123gh6pm.h"

void INIT_PLL(void) {
    SYSCTL->RCC2 |= 0x80000000;   // Use RCC2
    SYSCTL->RCC2 |= 0x00000800;   // bypass PLL while initializing
    SYSCTL->RCC = (SYSCTL->RCC &~0x000007C0) + 0x00000540;  // XTAL 16 MHz
    SYSCTL->RCC2 &= ~0x00000070;  // Cfg for main OSC
    SYSCTL->RCC2 &= ~0x00002000;  // activate PLL by clearing PWRDN
    SYSCTL->RCC2 = (SYSCTL->RCC2 & ~0x1FC00000) + (4<<23);  // 40 MHz
    while((SYSCTL->RIS & 0x00000040)==0){};  // Wait for PLL to lock
    SYSCTL->RCC2 &= ~0x00000800;  // enable PLL by clearing bypass
}

void GPIO_INIT(void) {
	  SYSCTL->RCGCSSI |= 0x01;     // activate SSI0
    SYSCTL->RCGCGPIO |= 0x001B;  // activate port A, B, D, E 
    while((SYSCTL->PRGPIO&0x001B) == 0){};  // ready?

		// Port A
		// PA2 = SSI0CLK  = SDO, LCD_SCK, pin 23
	  // PA4 = SSI0Rx   = Touchscreen Serial Out, pin 21
    // PA5 = SSI0Tx   = LCD Serial In, pin 19
		GPIOA->AFSEL |= 0x3C;        // Enable alt func on PA2,3,4,5
    GPIOA->DEN |= 0x3C;          // enable digital I/O on PA2,3,4,5

		// Port B
		// PB[0] = Data/CMD Pin for LCD (LCD_RS)
		GPIOB->DIR |= 0x01;          // PB[0] output
		GPIOB->AFSEL |= 0x00;        // disable alt func on PB0
		GPIOB->PUR |= 0x01;          // default to data Tx
    GPIOB->DEN |= 0x01;          // enable digital I/O on PB0

		// Port D
    // PD0 = RED LED
    // PD1 = GREEN LED
    // PD2 = Yellow LED
		GPIOD->DIR |= 0x07;          // PD[2:0] output
		GPIOD->AFSEL |= 0x00;        // disable alt func on PB0
		GPIOD->PUR |= 0x07;          // write 0 to turn on LED
    GPIOD->DEN |= 0x07;          // enable digital I/O on PB0
		GPIOD->DATA |= 0x7;          // turn off all LEDs by default
		
		// Port E
		GPIOE->DIR |= 0xFD;  // PE[1] is input, PE[0], PE[2] outputs
		GPIOE->AFSEL &= 0x00;  
		GPIOE->PUR |= 0x5;   // pull up on output LCD/TS chip select pins
		GPIOE->DEN |= 0x7;   // PE[2:0] enabled
}

void INIT_SSI0(void) {
	SSI0->CR1 &= 0xFFFFFFFD;        // Disable SSI while configuring
	SSI0->CR1 &= 0xFFFFFFFB;        // Set to Master mode
	// Time for ludicrous speed
	// 40 MHz SysClk SSIClk = SysClk / (CPSDVSR * (1 + SCR)) 
	//               10 MHz = 40 MHz / (CPSDVSR * (1 + SCR))
  // CPSDVSR from 2 to 254 (SSICPSR REG) SCR is a value from 0-255
  // from above, 4 = (CPSDVSR * (1 + SCR)) = 2 * (1+SCR)
	// we choose CPSDVSR = 1, then SCR = 1
	// We run this SSI0 @10Mhz as this is max of LCD spec
	// so CPSR = 0x02, SSI0->CR0 = 0x0107
	SSI0->CPSR = 0x02;             // see above
	SSI0->CR0  = 0x0107;           // see above
	SSI0->CR1 |= 0x2;              // Enable SSI
}
