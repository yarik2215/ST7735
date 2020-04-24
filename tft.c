/*
 * tft.c
 *
 *  Created on: Mar 23, 2020
 *      Author: yaroslav
 */

#include "tft.h"

static uint16_t TFT_CS_PIN;
static GPIO_TypeDef*	TFT_CS_PORT;
static uint16_t TFT_D_PIN;
static GPIO_TypeDef*	TFT_D_PORT;
static uint16_t TFT_RST_PIN;
static GPIO_TypeDef*	TFT_RST_PORT;


void DELAY(uint32_t x){
	volatile uint32_t i = 600 * (x);
	while(i--){
		__NOP();
	}
}

void TFT_Reset()
{
	LL_GPIO_ResetOutputPin(TFT_RST_PORT, TFT_RST_PIN);
	DELAY(50);
	LL_GPIO_SetOutputPin(TFT_RST_PORT, TFT_RST_PIN);
}

void TFT_Select()
{
	LL_GPIO_ResetOutputPin(TFT_CS_PORT, TFT_CS_PIN);
}


void TFT_Unscelect()
{
	LL_GPIO_SetOutputPin(TFT_CS_PORT, TFT_CS_PIN);
}


void TFT_WriteCommand(const uint8_t command)
{
	LL_GPIO_ResetOutputPin(TFT_D_PORT, TFT_D_PIN);
	while(!LL_SPI_IsActiveFlag_TXE(TFT_SPI));
	LL_SPI_TransmitData8(TFT_SPI, command);
	while(LL_SPI_IsActiveFlag_BSY(TFT_SPI));
}


void TFT_WriteData(const uint8_t data)
{
	LL_GPIO_SetOutputPin(TFT_D_PORT, TFT_D_PIN);
	while(!LL_SPI_IsActiveFlag_TXE(TFT_SPI));
	LL_SPI_TransmitData8(TFT_SPI, data);
	while(LL_SPI_IsActiveFlag_BSY(TFT_SPI));
}


void TFT_WriteDataArr(const uint8_t *data, uint16_t len)
{
	for(uint16_t i = 0; i < len; i++) {
		TFT_WriteData(data[i]);
	}
}


void TFT_SelectAddress(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
{
	TFT_SelectCollums(x1, x2);
	TFT_SelectRows(y1, y2);
}


void TFT_SelectCollums(uint8_t x1, uint8_t x2)
{
	TFT_WriteCommand(TFT_CASET);
	TFT_WriteData(0x00);
	TFT_WriteData(x1);
	TFT_WriteData(0x00);
	TFT_WriteData(x2);
}


void TFT_SelectRows(uint8_t y1, uint8_t y2)
{
	TFT_WriteCommand(TFT_RASET);
	TFT_WriteData(0x00);
	TFT_WriteData(y1);
	TFT_WriteData(0x00);
	TFT_WriteData(y2);
}

void TFT_Init(uint16_t csPin, GPIO_TypeDef *csPort, uint16_t dPin, GPIO_TypeDef *dPort, uint16_t rstPin, GPIO_TypeDef *rstPort)
{
	TFT_CS_PIN = csPin;
	TFT_CS_PORT = csPort;
	TFT_D_PIN = dPin;
	TFT_D_PORT = dPort;
	TFT_RST_PIN = rstPin;
	TFT_RST_PORT = rstPort;

	LL_SPI_Enable(TFT_SPI);
	TFT_Reset();
	TFT_Select();
	//______________________________>
	// Display initialization code
	TFT_WriteCommand(TFT_SWRESET);
	DELAY(100);
	TFT_WriteCommand(TFT_SLPOUT);
	DELAY(500);
	TFT_WriteCommand(TFT_COLMOD);
	TFT_WriteData(0x05);
	DELAY(100);
	TFT_WriteCommand(TFT_FRMCTR1);
	TFT_WriteData(0x00);
	TFT_WriteData(0x06);
	TFT_WriteData(0x03);
	DELAY(100);
	TFT_WriteCommand(TFT_MADCTL);
	TFT_WriteData(0x08);
	TFT_WriteCommand(TFT_DISSET5);
	TFT_WriteData(0x15);
	TFT_WriteData(0x02);
	TFT_WriteCommand(TFT_INVCTR);
	TFT_WriteData(0x00);
	TFT_WriteCommand(TFT_PWRCTR1);
	TFT_WriteData(0x02);
	TFT_WriteData(0x70);
	DELAY(50);
	TFT_WriteCommand(TFT_PWRCTR2);
	TFT_WriteData(0x05);
	TFT_WriteCommand(TFT_PWRCTR3);
	TFT_WriteData(0x01);
	TFT_WriteData(0x02);
	TFT_WriteCommand(TFT_VMCTR1);
	TFT_WriteData(0x01);
	TFT_WriteData(0x02);
	DELAY(50);
	TFT_WriteCommand(TFT_PWRCTR6);
	TFT_WriteData(0x01);
	TFT_WriteCommand(TFT_NORON);
	DELAY(50);
	TFT_WriteCommand(TFT_DISPON);
	DELAY(500);
	/*
	TFT_WriteCommand(TFT_MADCTL);
	TFT_WriteData(0x80);
	*/

	//______________________________<
	TFT_Unscelect();
}

void TFT_SetPixel(uint8_t x, uint8_t y, uint32_t color)
{
	TFT_Select();
	TFT_SelectAddress(x, x+1, y, y+1);
	TFT_WriteCommand(TFT_RAMWR);
	TFT_WriteData(color >> 8);
	TFT_WriteData(color);
	TFT_Unscelect();
}


void TFT_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color)
{
	TFT_Select();
	if((x+w-1) < ST7735_WIDTH && (y+h-1) < ST7735_HEIGHT) {
		for(;y < h; y++){
			for(;x < w; x++) {
				TFT_SetPixel(x, y, color);
			}
		}
	}
	TFT_Unscelect();
}

