#ifndef _TFT_8080_H_
#define _TFT_8080_H_

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "hal.h"
#include "hal_pal.h"



/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define  uint8_t unsigned char
#define  uint16_t unsigned int
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DB8_Pin 0u
#define DB8_GPIO_Port GPIOD
#define DB9_Pin 1u
#define DB9_GPIO_Port GPIOD
#define DB10_Pin 2u
#define DB10_GPIO_Port GPIOD
#define DB11_Pin 3u
#define DB11_GPIO_Port GPIOD
#define DB12_Pin 4u
#define DB12_GPIO_Port GPIOD
#define DB13_Pin 5u
#define DB13_GPIO_Port GPIOD
#define DB14_Pin 6u
#define DB14_GPIO_Port GPIOD
#define DB15_Pin 7u
#define DB15_GPIO_Port GPIOD
#define CS_Pin 4u
#define CS_GPIO_Port GPIOB
#define RESET_Pin 5u
#define RESET_GPIO_Port GPIOB
#define RS_Pin 6u
#define RS_GPIO_Port GPIOB
#define WR_Pin 1u
#define WR_GPIO_Port GPIOB
#define RD_Pin 2u
#define RD_GPIO_Port GPIOB

#define DC_low palClearPad(RS_GPIO_Port, RS_Pin)
#define DC_Hi palSetPad(RS_GPIO_Port, RS_Pin)
#define RESET_Hi palSetPad(RESET_GPIO_Port, RESET_Pin)
#define RESET_low palClearPad(RESET_GPIO_Port, RESET_Pin)
#define WR_Hi palSetPad(WR_GPIO_Port, WR_Pin)
#define WR_low palClearPad(WR_GPIO_Port, WR_Pin)
#define RD_Hi palSetPad(RD_GPIO_Port, RD_Pin)
#define RD_low palClearPad(RD_GPIO_Port, RD_Pin)
#define CS_Hi palSetPad(CS_GPIO_Port, CS_Pin)
#define CS_low palClearPad(CS_GPIO_Port, CS_Pin)
#define Data_Port  GPIOD->ODR

//Basic Colors
#define CYAN	  0x07FF
#define MAGNETA   0xF81F
#define RED	0xf800
#define GREEN	0x07e0
#define BLUE	0x001f
#define BLACK	0x0000
#define YELLOW	0xffe0
#define WHITE	0xffff

#define MIN_X	0
#define MIN_Y	0

#define FONT_X 8
#define FONT_Y 8
#define LCD_W 240
#define LCD_H 320

#define LCD_PIXEL_COUNT	LCD_WIDTH * LCD_HEIGHT



/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
void SystemClock_Config(void);
void TFT_GPIO_Init(void);
void TFT_Send_Cmd(uint8_t cmd);
void TFT_Write_Data(uint8_t data);
void TFT_Init(void);
void TFT_SetOrientation(uint8_t orient);
void TFT_Send_Data(uint16_t data);
void TFT_Set_Column(uint16_t start_column,uint16_t end_colunm);
void TFT_Set_Page(uint16_t start_page,uint16_t end_page);
void TFT_Set_XY(uint16_t x, uint16_t y);
void TFT_Draw_Pixel(uint16_t x, uint16_t y,uint16_t color);
uint16_t constrain(uint16_t a, uint16_t b, uint16_t c);
void TFT_Fill_Screen(uint16_t x_left, uint16_t x_right, uint16_t y_up, uint16_t y_down, uint16_t color);
void TFT_Fill_Rectangle(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t color);
void TFT_Draw_Char(uint16_t x, uint16_t y, uint16_t color, uint16_t phone, uint8_t ascii, uint8_t size);
void TFT_Draw_String(uint16_t x, uint16_t y, uint16_t color,uint16_t phone,char *string, uint8_t size);
void TFT_Draw_Line( uint16_t x0,uint16_t y0,uint16_t x1, uint16_t y1,uint16_t color);
void TFT_Draw_Horizontal_Line( uint16_t x, uint16_t y, uint16_t length,uint16_t color);
void TFT_Draw_Vertical_Line( uint16_t x, uint16_t y, uint16_t length,uint16_t color);
void TFT_Draw_Triangle( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void TFT_Draw_Circle(uint16_t pos_x, uint16_t pos_y, uint8_t r,uint16_t color);
void TFT_Fill_Circle(uint16_t pos_x, uint16_t pos_y, uint8_t r,uint16_t color);
void TFT_Image(uint16_t x, uint16_t y, uint16_t width ,uint16_t height,const unsigned char *p);

/* USER CODE END PFP */

#endif 
