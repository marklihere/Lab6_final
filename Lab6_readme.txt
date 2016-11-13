ILI9341 Datasheet

//---------------------------------
7.1.8. Serial Interface 33
  IM[3:0] = 0110, means
         4-pin serial interface 
3        CSX = "L"
(PB0)    D/CX = "H/L"
2        SCL = rising edge
4        serial data Input/Output (SDA [1 bidirectional pin] or SDI/SDO [2 separate pins for I/O])

         FUNCTION:  R/W cmd, parameter, or data display

4-line/8-bit bi-directional serial interfaces between host & ILI9341

MUST CONNECT DATA BUS[17:0] to GND

//---------------------------------
7.1.9. Write Cycle Sequence  33
"Write" means Host sends CMD or DATA to LCD
  3-pin serial packet  (D/CX + Tx byte)
    D/CX [PB0] == 0, CMD = Tx byte
    D/CX [PB0] == 1, DATA (Display RAM, memory write) = Tx byte, OR as a CMD parameter

  MSB Tx first

  CSX == 1, means Initialize Serial Interface
  CSX == falling edge, means Serial Interface enabled and start of data Tx.

So to Tx a 8-bit CMD,
  CSX = 1, then falling edge
  D/CX = 0 during final LSB of data
  SDA = data byte [7:0], MSB to LSB, TM4C Tx changes on falling edge, LCD Rx latch on rising edge

Example:
    | 8-bit data | 8-bit data | 8-bit data | 

LCD reads on rising edge of SCL  (slave Rx rising edge, master Tx on falling edge)

//---------------------------------
7.1.10. Read Cycle Sequence 36
"Read" means TM4C reads register parameter OR display data from LCD

1. Master sends CMD (Read ID or register CMD)
2. LCD sends 1-byte data

LCD latches SDA [input data] on rising edge, sends SDA [output data] on falling edge.

Read CMDs:
    8-bit:   [ LCD Tx 8-bits IMMEDIATELY following cmd LSB ]
        RDID1/RDID2/RDID3/0Ah/0Bh/0Ch/0Dh/0Eh/0Fh
    24-bit:  [ dummy clock cycle before LCD Tx all 24-bits ]
        RDDID
    32-bit:  [ dummy clock cycle before LCD Tx all 32-bits ]
        RDDST

//---------------------------------
7.1.11. Data Transfer Break and Recovery 40
RESX = reset LCD, Active LOW  (0 means reset, 1 normal operation)
  Why do I need to read this section?  Interrupts?

//---------------------------------
7.6.2. 4-line Serial Interface 63
WRITING
  Two color depths supported:  65k color [RGB 5,6,5 bit, total 16-bit] and 262k color [RGB 6,6,6 bit, total 18-bit]
  Different data formats for each mode
    for 65k, 1 pixel = 2 bytes, then the 16-bits go through a lookup table -> 18-bits, which controls the pixel RGB in Frame memory
    for 262k, 1 pixel = 3 bytes, R[5:0], 2-empty cycles padding, then G[5:0], 2 cycles empty, etc

READING
  3-bytes send, 8-clk dummy

//---------------------------------
9 Display Data RAM 202
MADCTRL  Memory Address Data CTRL
  some sort of translation from the display memory to actual physical display

//---------------------------------
19.3.4. Display Serial Interface Timing Characteristics (4-line SPI system)  237

//---------------------------------
Hardware wiring
PA2 = SSI0CLK  = LCD_SCK/TS_CLK, pin 23    0
PE2 = LCD Chip select = CSX, LCD_CS, pin 24  1   (change when combining, we use PE2)
PA5 = SSI0Tx   = LCD_SI/TS_SI, pin 19      2

PA4 = SSI0Rx   = TP_SO, pin 21
PE0 = TP_CS, active low, pin 26  
PE1 = TP_IRQ, pin 11

PB0 = D/C pin for LCD, LCD_RS, pin 15

PD0 = RED LED
PD1 = GREEN LED
PD2 = Yellow LED



IRQ when it goes low, send Tx to touchscreen to start a read
  a. getX()
  b. getY()

A2 A1 A0
x-coord: 11010000 (D0)
y-coord: 10010000 (90)

int i = 0;

upon interrupt:

enable touchscreen chip select
writeCmd(0xD0);  // read x-coord
writeData2(0);   // sends 16-bits of 0
data = SSI0->DR;  // get data
realdata = (data & 7FFF) >> 3;

unsigned short xarray[100];

xarray[i%100] = realdata;
i++

// crappy calibration
highest corner:
x: E96  (3734)
y: C7D  (3197)

Lowest corner:
x: 266  (614)
y: 256  (598)



