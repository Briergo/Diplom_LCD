#include "TFT_8080.h"
#include "Font.h"
#include <stdlib.h>

//#define  uint8_t unsigned char
//#define  uint16_t unsigned int

uint16_t MAX_X=0;
uint16_t MAX_Y=0;


void TFT_GPIO_Init(void)
{
  palSetPadMode(DB8_GPIO_Port,DB8_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(DB9_GPIO_Port,DB9_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(DB10_GPIO_Port,DB10_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(DB11_GPIO_Port,DB11_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(DB12_GPIO_Port,DB12_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(DB13_GPIO_Port,DB13_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(DB14_GPIO_Port,DB14_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(DB15_GPIO_Port,DB15_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(CS_GPIO_Port,CS_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(RESET_GPIO_Port,RESET_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(RS_GPIO_Port,RS_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(WR_GPIO_Port,WR_Pin,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(RD_GPIO_Port,RD_Pin,PAL_MODE_OUTPUT_PUSHPULL);	
}

void TFT_Send_Cmd(uint8_t cmd)
{
  DC_low;
  RD_Hi;
  CS_low;
  WR_low;
  Data_Port=cmd;
  chThdSleepMicroseconds(1);

  WR_Hi;
  CS_Hi;
}

void TFT_Write_Data(uint8_t data)
{
  DC_Hi;
  RD_Hi;
  CS_low;
  WR_low;
  Data_Port=data;
  chThdSleepMicroseconds(1);
  WR_Hi;
  CS_Hi;
}

void TFT_Init(void)
{
    TFT_GPIO_Init();
    RESET_Hi;

	TFT_Send_Cmd(0x01);        //Software Reset
	chThdSleepMilliseconds(1000);
	

	//Power Control 1
	TFT_Send_Cmd(0xC0);		//?????? ???????? ?????? ?????
	TFT_Write_Data(0x25);	


	//Power Control 2
	TFT_Send_Cmd(0xC1);		//????????? step up ???????????????
	TFT_Write_Data(0x11);


	//VCOM Control 1
	TFT_Send_Cmd(0xC5);		//????????????? ???????????? ????????? VCOMH - VCOML = 5.2V
	TFT_Write_Data(0x2B);	        //VCOMH = 3.825
	TFT_Write_Data(0x2B);  	//VCOML = -1.375
	
	//VCOM Control 2
	TFT_Send_Cmd(0xC7);		//?? Vcom ?? ???? ???, ? ??? ?? ?????? offset ??? ????
	TFT_Write_Data(0x86);        //VML=58 VMH=58

	//Memory Access Control
	TFT_SetOrientation(1); //???????? ?????????? ???????
	
	//COLMOD: Pixel Format Set
	TFT_Send_Cmd(0x3A);		//???? ??????? ????? ???????????? 16 ??????
	TFT_Write_Data(0x05);
	
	//Frame Rate Control 
	TFT_Send_Cmd(0xB1);
	TFT_Write_Data(0x00);
	TFT_Write_Data(0x18);	 //Frame Rate 79Hz


	//Display Function Control
	TFT_Send_Cmd(0xB6);
	TFT_Write_Data(0x0A);
	TFT_Write_Data(0x82);//??????? ??? ?????????? ?????????? ???? ???????? ????? - 1, ?????? - 0,
	TFT_Write_Data(0x27);

	// Sleep Out
	TFT_Send_Cmd(0x11);

	chThdSleepMilliseconds(120);

	//Display On
	TFT_Send_Cmd(0x29);
}

void TFT_SetOrientation(uint8_t orient)
{
  TFT_Send_Cmd(0x36);
  switch (orient)
	{
		case 0: TFT_Write_Data(0x48);
				break;
		case 1: TFT_Write_Data(0x28);
				break;
		case 2: TFT_Write_Data(0x88);
				break;
		case 3: TFT_Write_Data(0xE8);
				break;
	}
  if (orient == 0 || orient == 2)
	{
		MAX_X = 239;
		MAX_Y = 319;
	}
	else
	{
		MAX_X = 319;
		MAX_Y = 239;
	}
}

/* Otpravka 2 baty*/
void TFT_Send_Data(uint16_t data)
{
	uint8_t data1 = data>>8;
	uint8_t data2 = data&0xff;
	TFT_Write_Data(data1);
	TFT_Write_Data(data2);
}
/* Ogran. kookrd po X*/
void TFT_Set_Column(uint16_t start_column,uint16_t end_colunm)
{
	TFT_Send_Cmd(0x2A);                                                     
	TFT_Send_Data(start_column);
	TFT_Send_Data(end_colunm);
}
/* Ogran. kookrd po Y*/
void TFT_Set_Page(uint16_t start_page,uint16_t end_page)
{
	TFT_Send_Cmd(0x2B);                                                      
	TFT_Send_Data(start_page);
	TFT_Send_Data(end_page);
}
/* Ogran. raboch. oblasti*/
void TFT_Set_XY(uint16_t x, uint16_t y)
{
	TFT_Set_Column(x, x);
	TFT_Set_Page(y, y);
	TFT_Send_Cmd(0x2c);
}
/* Risov. pix*/
void TFT_Draw_Pixel(uint16_t x, uint16_t y,uint16_t color)
{
	TFT_Set_XY(x, y);
	TFT_Send_Data(color);
}

uint16_t constrain(uint16_t a, uint16_t b, uint16_t c)
{
	if (a < b)
	{
		return b;
	}
	if (c < a)
	{
		return c;
	}
	else return a;
}
/* Otris. color */
void TFT_Fill_Screen(uint16_t x_left, uint16_t x_right, uint16_t y_up, uint16_t y_down, uint16_t color)
{
	unsigned long  xy=0;
	unsigned long i=0;
	if(x_left > x_right)
	{	
		x_left = x_left^x_right;       
		x_right = x_left^x_right;      
		x_left = x_left^x_right;       
	}
	if(y_up > y_down)
	{
		y_up = y_up^y_down;									
		y_down = y_up^y_down;		
		y_up = y_up^y_down;		
	}
	
	
	x_left = constrain(x_left, MIN_X,MAX_X);
	x_right = constrain(x_right, MIN_X,MAX_X);
	y_up = constrain(y_up, MIN_Y,MAX_Y);
	y_down = constrain(y_down, MIN_Y,MAX_Y);

	xy = (x_right - x_left+1);		
	xy = xy*(y_down - y_up+1);		

	TFT_Set_Column(x_left,x_right);	        
	TFT_Set_Page(y_up, y_down);		
	TFT_Send_Cmd(0x2c);			
	
	for(i=0; i < xy; i++)
	{
		TFT_Send_Data(color);	
	}
}
/* Kvadrat*/
void TFT_Fill_Rectangle(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t color)
{	
	if(length  && width)
	{
		TFT_Fill_Screen(x, x+length - 1, y, y+width - 1, color);
	}
	
}
/*Simvol*/
void TFT_Draw_Char(uint16_t x, uint16_t y, uint16_t color, uint16_t phone, uint8_t ascii, uint8_t size)
{
	for (int i = 0; i < FONT_Y; i++ )
	{
		for(uint8_t f = 0; f < FONT_X; f++)
		{
			if(((simbols[ascii-0x20][i])>>(7-f))&0x01)
			{
				TFT_Fill_Rectangle(x+f*size, y+i*size, size, size, color);
			}
			else
			{
				TFT_Fill_Rectangle(x+f*size, y+i*size, size, size, phone);
			}

		}

	}
}
/*Stroka*/
void TFT_Draw_String(uint16_t x, uint16_t y, uint16_t color,uint16_t phone,char *string, uint8_t size)
{
        
	while(*string)
	{      
                
		if((x + FONT_X) > MAX_X)
		{
			x = 1;
			y = y + FONT_X*size;
		}
		TFT_Draw_Char(x, y, color, phone,*string, size);
		x += FONT_X*size;     
		*string++;           
	}
}
/* Line*/
void TFT_Draw_Line( uint16_t x0,uint16_t y0,uint16_t x1, uint16_t y1,uint16_t color)
{

	int x = x1-x0;
	int y = y1-y0;
	int dx = abs(x), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2;
	for (;;){
		TFT_Draw_Pixel(x0,y0,color);
		e2 = 2*err;
		if (e2 >= dy) {
			if (x0 == x1) break;
			err += dy; x0 += sx;
		}
		if (e2 <= dx) {
			if (y0 == y1) break;
			err += dx; y0 += sy;
		}
	}
}
/* Horizontal Line*/
void  TFT_Draw_Horizontal_Line( uint16_t x, uint16_t y, uint16_t length,uint16_t color)
{
    int i;
	TFT_Set_Column(x,x + length);
	TFT_Set_Page(y,y);
	TFT_Send_Cmd(0x2c);
	for( i=0; i<length; i++)
	TFT_Send_Data(color);
}
/* Vertical Line*/
void TFT_Draw_Vertical_Line( uint16_t x, uint16_t y, uint16_t length,uint16_t color)
{
    int i;
	TFT_Set_Column(x,x);
	TFT_Set_Page(y,y+length);
	TFT_Send_Cmd(0x2c);
	for(i=0; i<length; i++)
	TFT_Send_Data(color);
}

/*Treugol*/
void TFT_Draw_Triangle( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
	TFT_Draw_Line( x1, y1, x2, y2, color);
	TFT_Draw_Line( x2, y2, x3, y3, color);
	TFT_Draw_Line( x3, y3, x1, y1, color);
}
/*Krug*/
void TFT_Draw_Circle(uint16_t pos_x, uint16_t pos_y, uint8_t r,uint16_t color)
{
	int x = -r, y = 0, err = 2-2*r, e2;
	do {
		TFT_Draw_Pixel(pos_x-x, pos_y+y,color);
		TFT_Draw_Pixel(pos_x+x, pos_y+y,color);
		TFT_Draw_Pixel(pos_x+x, pos_y-y,color);
		TFT_Draw_Pixel(pos_x-x, pos_y-y,color);
		e2 = err;
		if (e2 <= y) {
			err += ++y*2+1;
			if (-x == y && e2 <= x) e2 = 0;
		}
		if (e2 > x) err += ++x*2+1;
	} while (x <= 0);
}
/*Zakrash krug*/
void TFT_Fill_Circle(uint16_t pos_x, uint16_t pos_y, uint8_t r,uint16_t color)
{
	int x = -r, y = 0, err = 2-2*r, e2;
	do {

		TFT_Draw_Vertical_Line(pos_x-x, pos_y-y, 2*y, color);
		TFT_Draw_Vertical_Line(pos_x+x, pos_y-y, 2*y, color);

		e2 = err;
		if (e2 <= y) {
			err += ++y*2+1;
			if (-x == y && e2 <= x) e2 = 0;
		}
		if (e2 > x) err += ++x*2+1;
	} while (x <= 0);

}



void TFT_Image (uint16_t x, uint16_t y, uint16_t width ,uint16_t height,const unsigned char *p) 
{ 
	    uint32_t picH,picL;
	    uint32_t size, n;
        size=width*height;    
        TFT_Set_Column(x,width+x-1);	        
        TFT_Set_Page(y, height+y-1);
        TFT_Send_Cmd(0x2C);
        for(n=0;n<size;n++)
	{	
	 	picL=*(p+n*2);	//??????
		picH=*(p+n*2+1);				
		TFT_Send_Data(picH<<8|picL);  						
	}	
		
}


