// page 238 of the data sheet.
#define white          0xFFFF
#define black          0x0001
#define grey           0xF7DE
#define blue           0x001F
#define red            0xF800
#define magenta        0xF81F
#define green          0x07E0
#define cyan           0x7FFF
#define yellow         0xFFE0

extern unsigned char *PB;
extern unsigned char *SYSCTL;
extern unsigned char *SSI0;
extern volatile unsigned int SSI0_SR;

void writeCmd(unsigned char CMD);
void writeDat(unsigned char DAT);
void writeDat2(unsigned short DAT);
void writeDat4(unsigned int DAT);

void setArea(unsigned short x1, unsigned short x2, unsigned short y1, unsigned short y2);
void writeColor(unsigned short color);

void flash_screen(unsigned short color);
void LCD_Init(void);
