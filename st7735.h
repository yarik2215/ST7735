/* vim: set ai et ts=4 sw=4: */
#ifndef __ST7735_H__
#define __ST7735_H__

#include "fonts.h"
#include "stm32f1xx.h"	//add header for your mcu series
//#include <stdbool.h>

typedef struct {
	uint32_t width;
	uint32_t heigth;

	SPI_TypeDef *ST7735_SPI;
	uint16_t ST7735_CS_PIN;
	GPIO_TypeDef*	ST7735_CS_PORT;
	uint16_t ST7735_D_PIN;
	GPIO_TypeDef*	ST7735_D_PORT;
	uint16_t ST7735_RST_PIN;
	GPIO_TypeDef*	ST7735_RST_PORT;

} Display;


/******* Display list *********/
#define ST7735_160X128

/******************************/


#define ST7735_INVERTED_COLORS 	1
#define ST7735_NORMAL_COLORS	0

#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04


/*** Display type define ***/

// AliExpress/eBay 1.8" display, default orientation
#ifdef ST7735_160X128
	//#define ST7735_IS_160X128 1
	#define ST7735_WIDTH  128
	#define ST7735_HEIGHT 160
	#define ST7735_XSTART 0
	#define ST7735_YSTART 0
	#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB)
#endif
/*****************************/

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	ST7735_COLOR_BLACK   0x0000
#define	ST7735_COLOR_BLUE    0x001F
#define	ST7735_COLOR_RED     0xF800
#define	ST7735_COLOR_GREEN   0x07E0
#define ST7735_COLOR_CYAN    0x07FF
#define ST7735_COLOR_MAGENTA 0xF81F
#define ST7735_COLOR_YELLOW  0xFFE0
#define ST7735_COLOR_WHITE   0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

/*** Function definition ***/
void ST7735_Init(SPI_TypeDef *SPIx, uint16_t csPin, GPIO_TypeDef *csPort, uint16_t dPin,
				GPIO_TypeDef *dPort, uint16_t rstPin, GPIO_TypeDef *rstPort);
void ST7735_Select();
void ST7735_Unselect();	// call before initializing any SPI devices
void ST7735_Reset();
void ST7735_InvertColors(uint8_t invert);
void ST7735_DrawPixel(int16_t x, int16_t y, uint16_t color);
void ST7735_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, uint16_t line);
void ST7735_DrawHLine(int16_t x1, int16_t x2, int16_t y, uint16_t color);
void ST7735_DrawVLine(int16_t x, int16_t y1, int16_t y2, uint16_t color);
void ST7735_DrawRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t line);
void ST7735_DrawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint16_t line);
void ST7735_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void ST7735_FillRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7735_FillScreen(uint16_t color);
//void ST7735_Print(uint16_t x, uint16_t y, char *str, ...);	// Print formated string on display
void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);

#endif // __ST7735_H__
