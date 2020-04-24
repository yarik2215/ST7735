/*
 * tft.h
 *
 *  Created on: Mar 22, 2020
 *      Author: yaroslav
 */

#ifndef INC_TFT_H_
#define INC_TFT_H_

#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_gpio.h>

//#define USE_FULL_LL_DRIVER

// select which SPI is using for communication with LCD
#define TFT_SPI SPI1

// Display types define
#define ST7735_128x160


void TFT_Reset();
void TFT_Select();
void TFT_Unscelect();
void TFT_WriteCommand(const uint8_t command);
void TFT_WriteData(const uint8_t data);
void TFT_WriteDataArr(const uint8_t *data, uint16_t len);
void TFT_SelectAddress(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2);
void TFT_SelectCollums(uint8_t x1, uint8_t x2);
void TFT_SelectRows(uint8_t y1, uint8_t y2);
void TFT_Init(uint16_t csPin, GPIO_TypeDef *csPort, uint16_t dPin, GPIO_TypeDef *dPort, uint16_t rstPin, GPIO_TypeDef *rstPort);
void TFT_SetPixel(uint8_t x, uint8_t y, uint32_t color);
void TFT_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color);

#define TFT_NOP	     	0x00
#define TFT_SWRESET	 	0x01
#define TFT_RDDID   	0x04
#define TFT_RDDST   	0x09

#define TFT_SLPIN		0x10
#define TFT_SLPOUT		0x11
#define TFT_PTLON		0x12
#define TFT_NORON		0x13

#define TFT_INVOFF		0x20
#define TFT_INVON		0x21
#define TFT_DISPOFF		0x28
#define TFT_DISPON		0x29

#define TFT_CASET 		0x24
#define TFT_RASET 		0x25
#define TFT_RAMWR 		0x28

#define TFT_RGBSET		0x2D
#define TFT_PTLAR		0x30
#define TFT_SCRLAR		0x33
#define TFT_MADCTL		0x36
#define TFT_IDMOFF		0x38	// IDLE MODE OFF
#define TFT_IDMON		0x39	// IDLE MODE ON
#define TFT_COLMOD		0x3A	// Interface Pixel Format	Mods:0x3 12bit, 0x5 16bit. 0x6 18bit


#define TFT_FRMCTR1		0xB1	// Frame rate control (In normal mode, full colors)
#define TFT_FRMCTR2		0xB2	// Frame rate control (In idle mode, 8-colors)
#define TFT_FRMCTR3		0xB3	// Frame rate control (In Partial mode, full colors)
#define TFT_INVCTR		0xB4
#define TFT_DISSET5		0xB6


#define TFT_PWRCTR1		0xC0	// Power control 1
#define TFT_PWRCTR2		0xC1	// Power control 2
#define TFT_PWRCTR3		0xC2	// Power control 3 (In normal mode/ Full color)
#define TFT_PWRCTR4		0xC3	// Power control 3 (In IDLE mode/ 8bit color)
#define TFT_PWRCTR5		0xC4	// Power control 3 (In partial mode/ Full color)
#define TFT_VMCTR1		0xC5

#define TFT_RDID1   	0xDA
#define TFT_RDID2   	0xDB
#define TFT_RDID3   	0xDC
#define TFT_RDID4   	0xDD

#define TFT_PWRCTR6  	0xFC

// 5-6-5 bits colors samples
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF


// Display types
#ifdef ST7735_128x160
#define ST7735_WIDTH 128
#define ST7735_HEIGHT 160
#endif


#endif /* INC_TFT_H_ */
