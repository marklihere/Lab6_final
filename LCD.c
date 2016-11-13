#include "LCD.H"  // we only use for #define colors

#define SSI_SR_RNE 0x00000004 // SSI Rx FIFO Not Empty
#define SSI_SR_TFE 0x00000001 // SSI Tx FIFO Empty

// SPI code for Tx 1 byte/Rx 1 byte
// pulled from textbook, example 7.3
uint8_t sendAfterWaiting(uint8_t code) {
  while((SSI0->SR & SSI_SR_TFE)==0){};  // wait until FIFO empty
	SSI0->DR = code;
	while((SSI0->SR & SSI_SR_RNE)==0){};  // wait until response received
	return SSI0->DR;
}

// write cmd to LCD
void writeCmd(unsigned char CMD) {
	GPIOB->DATA = 0x0;   // DATA/CMD pin, command is 0
	sendAfterWaiting(CMD);
}

// write one bytes to LCD
void writeDat(unsigned char DAT) {
	GPIOB->DATA = 0x1;  // DATA/CMD pin, data is 1
	sendAfterWaiting(DAT);
}

// write two bytes to LCD
void writeDat2(unsigned short DAT) {
	GPIOB->DATA = 0x1;  // DATA/CMD pin, data is 1
	sendAfterWaiting(DAT>> 8);
  sendAfterWaiting(DAT);
}

void LCD_Init()
{
	int i;
    writeCmd(0xCB);
    writeDat(0x39);
    writeDat(0x2C);
    writeDat(0x00);
    writeDat(0x34);
    writeDat(0x02);

    writeCmd(0xCF);
    writeDat(0x00);
    writeDat(0XC1);
    writeDat(0X30);

    writeCmd(0xE8);
    writeDat(0x85);
    writeDat(0x00);
    writeDat(0x78);

    writeCmd(0xEA);
    writeDat(0x00);
    writeDat(0x00);

    writeCmd(0xED);
    writeDat(0x64);
    writeDat(0x03);
    writeDat(0X12);
    writeDat(0X81);

    writeCmd(0xF7);
    writeDat(0x20);

    writeCmd(0xC0);    //Power control
    writeDat(0x23);   //VRH[5:0]

    writeCmd(0xC1);    //Power control
    writeDat(0x10);   //SAP[2:0];BT[3:0]

    writeCmd(0xC5);    //VCM control
    writeDat(0x3e); //¶Ô±È¶Èµ÷½Ú
    writeDat(0x28);

    writeCmd(0xC7);    //VCM control2
    writeDat(0x86);  //--

    writeCmd(0x36);    // Memory Access Control
    writeDat(0x48); //C8       //48 68ÊúÆÁ//28 E8 ºáÆÁ

    writeCmd(0x3A);
    writeDat(0x55);

    writeCmd(0xB1);
    writeDat(0x00);
    writeDat(0x18);

    writeCmd(0xB6);    // Display Function Control
    writeDat(0x08);
    writeDat(0x82);
    writeDat(0x27);

    writeCmd(0xF2);    // 3Gamma Function Disable
    writeDat(0x00);

    writeCmd(0x26);    //Gamma curve selected
    writeDat(0x01);

    writeCmd(0xE0);    //Set Gamma
    writeDat(0x0F);
    writeDat(0x31);
    writeDat(0x2B);
    writeDat(0x0C);
    writeDat(0x0E);
    writeDat(0x08);
    writeDat(0x4E);
    writeDat(0xF1);
    writeDat(0x37);
    writeDat(0x07);
    writeDat(0x10);
    writeDat(0x03);
    writeDat(0x0E);
    writeDat(0x09);
    writeDat(0x00);

    writeCmd(0XE1);    //Set Gamma
    writeDat(0x00);
    writeDat(0x0E);
    writeDat(0x14);
    writeDat(0x03);
    writeDat(0x11);
    writeDat(0x07);
    writeDat(0x31);
    writeDat(0xC1);
    writeDat(0x48);
    writeDat(0x08);
    writeDat(0x0F);
    writeDat(0x0C);
    writeDat(0x31);
    writeDat(0x36);
    writeDat(0x0F);

    writeCmd(0x11);    //Exit Sleep
		for( i = 0; i < 20000; i++) { i++;}

    writeCmd(0x29);    //Display on
}
