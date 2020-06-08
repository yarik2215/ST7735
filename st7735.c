
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_gpio.h"
#include <string.h>
#include "st7735.h"
#include <stdlib.h>

#define DELAY 0x80

static SPI_TypeDef *ST7735_SPI;
static uint16_t ST7735_CS_PIN;
static GPIO_TypeDef*	ST7735_CS_PORT;
static uint16_t ST7735_D_PIN;
static GPIO_TypeDef*	ST7735_D_PORT;
static uint16_t ST7735_RST_PIN;
static GPIO_TypeDef*	ST7735_RST_PORT;


/*
void HAL_Delay(uint32_t x)
{
	volatile uint32_t i = 600 * (x);
	while(i--){
		__NOP();
	}
}
*/

/*** Function definition ***/
#define SWAP(x1,x2) {uint32_t x3 = (x1); (x1) = (x2); (x2) = x3;}
static inline void ST7735_WriteCommand(uint8_t cmd);
static inline void ST7735_WriteData(uint8_t* buff, uint32_t buff_size);


#define TEMP_BUFF_SIZE 128
typedef struct{
	uint8_t buff8t[TEMP_BUFF_SIZE*2];
	uint16_t counter;
}Buffer;

// based on Adafruit ST7735 library for Arduino
static const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay


static inline void ST7735_WriteCommand(uint8_t cmd) {
//	while(!LL_SPI_IsActiveFlag_TXE(ST7735_SPI));
	LL_GPIO_ResetOutputPin(ST7735_D_PORT, ST7735_D_PIN);
	LL_SPI_TransmitData8(ST7735_SPI, cmd);
	while(!LL_SPI_IsActiveFlag_TXE(ST7735_SPI) || LL_SPI_IsActiveFlag_BSY(ST7735_SPI));
}


static inline void ST7735_WriteData(uint8_t* buff, uint32_t buff_size) {
	for(uint16_t i = 0; i < buff_size; i++) {
//		while(!LL_SPI_IsActiveFlag_TXE(ST7735_SPI));
		LL_GPIO_SetOutputPin(ST7735_D_PORT, ST7735_D_PIN);
		LL_SPI_TransmitData8(ST7735_SPI, buff[i]);
		while(!LL_SPI_IsActiveFlag_TXE(ST7735_SPI));
//		while(!LL_SPI_IsActiveFlag_TXE(ST7735_SPI) && LL_SPI_IsActiveFlag_BSY(ST7735_SPI));
//		while(!(ST7735_SPI->SR & SPI_SR_BSY) || ST7735_SPI->SR & SPI_SR_TXE);
	}
	while(LL_SPI_IsActiveFlag_BSY(ST7735_SPI));
}

void ST7735_Select() {
	LL_GPIO_ResetOutputPin(ST7735_CS_PORT, ST7735_CS_PIN);
}


void ST7735_Unselect() {
	LL_GPIO_SetOutputPin(ST7735_CS_PORT, ST7735_CS_PIN);
}


void ST7735_Reset() {
	LL_GPIO_ResetOutputPin(ST7735_RST_PORT, ST7735_RST_PIN);
	HAL_Delay(50);
	LL_GPIO_SetOutputPin(ST7735_RST_PORT, ST7735_RST_PIN);
}


void ST7735_InvertColors(uint8_t invert) {
    ST7735_Select();
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    ST7735_Unselect();
}


static void ST7735_ExecuteCommandList(const uint8_t *addr)
{
	uint8_t numCommands, numArgs;
	    uint16_t ms;

	    numCommands = *addr++;
	    while(numCommands--) {
	        uint8_t cmd = *addr++;
	        ST7735_WriteCommand(cmd);

	        numArgs = *addr++;
	        // If high bit set, delay follows args
	        ms = numArgs & DELAY;
	        numArgs &= ~DELAY;
	        if(numArgs) {
	            ST7735_WriteData((uint8_t*)addr, numArgs);
	            addr += numArgs;
	        }

	        if(ms) {
	            ms = *addr++;
	            if(ms == 255) ms = 500;
	            HAL_Delay(ms);
	        }
	    }
}


static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	/*
	if(x0 < ST7735_XSTART)
    	x0 = ST7735_XSTART;
    if(x1 >= ST7735_WIDTH)
    	x1 = ST7735_WIDTH-1;
    if(y0 < ST7735_YSTART)
    	y0 = ST7735_XSTART;
    if(y1 >= ST7735_HEIGHT)
    	y1 = ST7735_HEIGHT-1;
    */

	// column address set
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_RAMWR);
}


void ST7735_Init(SPI_TypeDef *SPIx, uint16_t csPin, GPIO_TypeDef *csPort, uint16_t dPin,
				GPIO_TypeDef *dPort, uint16_t rstPin, GPIO_TypeDef *rstPort)
{
	ST7735_SPI = SPIx;
	ST7735_CS_PIN = csPin;
	ST7735_CS_PORT = csPort;
	ST7735_D_PIN = dPin;
	ST7735_D_PORT = dPort;
	ST7735_RST_PIN = rstPin;
	ST7735_RST_PORT = rstPort;
	LL_SPI_Enable(ST7735_SPI);
//	ST7735_SPI->CR1 |= SPI_CR1_SPE;
	HAL_Delay(50);
    ST7735_Select();
    ST7735_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
    ST7735_ExecuteCommandList(init_cmds3);
    ST7735_Unselect();
}


void ST7735_DrawPixel(int16_t x, int16_t y, uint16_t color) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT) || x < 0 || y < 0)
        return;

    ST7735_Select();

    ST7735_SetAddressWindow(x, y, x, y);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    ST7735_Unselect();
}


void ST7735_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
					uint16_t color, uint16_t line)
{
	uint8_t step = (abs(x2 - x1) < abs(y2 - y1));
	if(step) {
		SWAP(x1,y1);
		SWAP(x2,y2);
	}

	if(x1 > x2) {
		SWAP(x1,x2);
		SWAP(y1,y2);
	}

	uint16_t deltax = (x2 - x1) + 1;
	uint16_t deltay = abs(y2 - y1) + 1;
	uint16_t error = 0;
	int8_t diry = (y2 - y1) > 0 ? 1 : -1;

	for(uint16_t x = x1; x <= x2; x++) {
		error+=deltay;
		if(error >= deltax) {
			if(!step)
				ST7735_FillRectangle(x1, y1, (x-x1), line, color);
			else
				ST7735_FillRectangle(y1, x1, line ,(x-x1) , color);
			x1 = x;
			y1+= diry;
			error -= deltax;
		}
	}
	//ST7735_FillRectangle(x1, y1, (x2-x1), 1, color);
}

void ST7735_DrawHLine(int16_t x1, int16_t x2, int16_t y, uint16_t color)
{
	if(x1 > x2) SWAP(x1,x2);
	ST7735_FillRectangle(x1, y, x2-x1+1, 1, color);
}

void ST7735_DrawVLine(int16_t x, int16_t y1, int16_t y2, uint16_t color)
{
	if(y1 > y2) SWAP(y1,y2);
	ST7735_FillRectangle(x, y1, 1, y2-y1+1, color);
}

void ST7735_DrawRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t line)
{
	// vertical lines
	ST7735_FillRectangle(x, y, line, h, color);
	ST7735_FillRectangle(x+w-line, y, line, h, color);
	// horizontal lines
	ST7735_FillRectangle(x, y, w, line, color);
	ST7735_FillRectangle(x, y+w-line, w, line, color);
}


void ST7735_DrawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint16_t line)
{
	int x1 = 0;
	int y1 = r;
	int delta = 1 - 2 * r;
	int error = 0;

	while (y1 >= 0) {
	    ST7735_DrawPixel(x + x1, y + y1, color);
		ST7735_DrawPixel(x + x1, y - y1, color);
		ST7735_DrawPixel(x - x1, y + y1, color);
		ST7735_DrawPixel(x - x1, y - y1, color);
	    error = 2 * (delta + y1) - 1;
	    if ((delta < 0) && (error <= 0)) {
	    	delta += 2 * ++x1 + 1;
	        continue;
	    }
	    if ((delta > 0) && (error > 0)) {
	        delta -= 2 * --y1 + 1;
	        continue;
	    }
	    delta += 2 * (++x1 - y1--);
	}
}


void ST7735_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int x = 0;
	int y = r;
	int delta = 1 - 2 * r;
	int error = 0;

	while (y >= 0) {
		/*
	    ST7735_DrawPixel(x0 + x, y0 + y, color);
		ST7735_DrawPixel(x0 + x, y0 - y, color);
		ST7735_DrawPixel(x0 - x, y0 + y, color);
		ST7735_DrawPixel(x0 - x, y0 - y, color);
	    */
	    error = 2 * (delta + y) - 1;
	    if ((delta < 0) && (error <= 0)) {
	    	ST7735_DrawVLine(x0 + x, y0 + y, y0 - y, color);
	    	ST7735_DrawVLine(x0 - x, y0 - y, y0 + y, color);
	    	delta += 2 * ++x + 1;
	        continue;
	    }
	    if ((delta > 0) && (error > 0)) {
	    	/*
	    	ST7735_DrawVLine(x0 + x, y0 + y, y0 - y, color);
	    	ST7735_DrawVLine(x0 - x, y0 - y, y0 + y, color);
	    	*/
	    	delta -= 2 * --y + 1;
	        continue;
	    }
	    ST7735_DrawVLine(x0 + x, y0 + y, y0 - y, color);
	    ST7735_DrawVLine(x0 - x, y0 - y, y0 + y, color);
	    delta += 2 * (++x - y--);
	}
}

void ST7735_FillRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
	if(x < 0) {
		if((x + w - 1) < 0) return;
		x = 0;
	}
	if(y < 0) {
		if((y + h - 1) < 0) return;
		y = 0;
	}
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) w = ST7735_WIDTH - x;
    if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
//    Buffer data;
   uint8_t invColor[] = { color >> 8, color & 0xFF };
   uint16_t data[128] = {};
   uint8_t counter = 0;

    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
        	data[counter] = *((uint16_t*)invColor);
        	counter ++;
        	if(counter == 128) {
            	ST7735_WriteData((uint8_t*)data, counter*2);
            	counter = 0;
        	}
        }
    }
    if(counter)
    	ST7735_WriteData((uint8_t*)data, counter*2);
    ST7735_Unselect();
}


//void ST7735_FillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);


void ST7735_FillScreen(uint16_t color) {
    ST7735_FillRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}


//void ST7735_Print(uint16_t x, uint16_t y, char *str, ...);	// Print formated string on display


static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    ST7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
}


void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    ST7735_Select();

    while(*str) {
        if(x + font.width >= ST7735_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ST7735_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
    ST7735_Unselect();
}


void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) return;
    if((y + h - 1) >= ST7735_HEIGHT) return;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    ST7735_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ST7735_Unselect();
}



