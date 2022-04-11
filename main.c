#include "ch.h"
#include "hal.h"
#include "menu.h"
#include "TFT_8080.h"
#include "Motor.h"
#include "chprintf.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>
//#include "ff.h"

uint32_t impulseWidth=0 ;
uint8_t flag_start=0;
uint8_t up_flag=0;
uint8_t down_flag=0;
uint8_t tim_flag=0;
uint8_t left_flag=0;
uint8_t right_flag=0;
int16_t holl_speed=0;
int16_t speed = 5;
uint8_t tft_flag=0;

#define BUFFER_SIZE 100

mailbox_t tft_mb;
msg_t tft_mb_buffer[BUFFER_SIZE];

struct regulator Reg1;



void Uart_Init(void);
void dbgprintf( const char* format, ... );
void cbgptfun3(GPTDriver *gptp);
void holl(void* args);
void Init_PID_Reg(void);

GPTDriver *timer3 = &GPTD3;
SerialDriver *uart3 = &SD3;
static BaseSequentialStream *uart3_stream = NULL;

static const SerialConfig uart_conf = {
  .speed = 115200,
  .cr1 = 0,
  .cr2 = 0,
  .cr3 = 0
};


// ����������� ������� ��������� ������� 50_000�� (������������ 4320, ����� �����, ������ ��� 2^16) � ��������� ������ ������� ��� ���������� ����������
GPTConfig gpt3_conf = {
    .frequency = 50000,
    .callback = cbgptfun3,
    .cr2 = 0,
    .dier = 0
};
/*
static THD_WORKING_AREA(tftThread, 256);// 256 - stack size

static THD_FUNCTION(tft, arg)
{
    arg=arg;
    char String[15];
    msg_t my_msg;
    msg_t msg =chMBFetchI (&tft_mb, &my_msg);
    if (msg == MSG_OK)
    {
       sprintf(String,"%d",(int16_t)my_msg);
       TFT_Fill_Screen(10,60,200,240,BLUE);
       TFT_Draw_String(10,200,RED,BLUE,String,2);
    }
}
static THD_FUNCTION(tft_speed, arg)
{
    arg=arg;
    char String[15];
    sprintf(String,"%d",speed);
    TFT_Fill_Screen(10,60,200,240,BLUE);
    TFT_Draw_String(10,200,RED,BLUE,String,2);
}
*/
int main(void)
{
    uint8_t arg = 5;

    //������� ������
    uint8_t flag_key1_press = 1;
    systime_t time_key1_press = 0;
    systime_t time = 0;
    halInit();
    chSysInit();
    Uart_Init();
    palSetPadMode( GPIOC, 6, PAL_MODE_ALTERNATE(2) );
    palSetLineMode(LINE_LED1, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(LINE_LED2, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(LINE_LED3, PAL_MODE_OUTPUT_PUSHPULL);
    //��������� ������� � ������������� ����
    TFT_Init();
    TFT_Fill_Screen(0,320,0,240,BLUE);

    chMBObjectInit(&tft_mb, tft_mb_buffer, BUFFER_SIZE);

    //Menu_Create();
    Menu_GPIO_Init();
    Motor_GPIO_Init();

    //������ ������� 3
    gptStart(timer3, &gpt3_conf);
    //������ � ����������� ������ � �������� 0.1 �
    gptStartContinuous(timer3, 5000);
    //��������� ��� ������� �����
    palSetPadMode(GPIOB, 8u, PAL_MODE_INPUT);
    palEnablePadEvent(GPIOB, 8u, PAL_EVENT_MODE_BOTH_EDGES);
    palSetPadCallback(GPIOB, 8u, holl, &arg);
    palSetPadMode(GPIOB, 9u, PAL_MODE_INPUT);

    Init_PID_Reg();

    while (1)
    {

      if(up_flag==1)
      {
        flag_key1_press = 0;
        if(flag_start==0)
        {
          Motor_Forward();
          pwmEnableChannel( &PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1,10000) );
        }
        time_key1_press = chVTGetSystemTime();
        up_flag=0;
      }

      if(down_flag==1)
      {
        flag_key1_press = 0;
         if(flag_start==1)
         {
           Motor_Stop();
         }
         time_key1_press = chVTGetSystemTime();
         down_flag=0;
      }

      if(right_flag==1)
      {
        flag_key1_press = 0;
        if(flag_start==1 && speed<20)
        {
          speed+=1;
          //chThdCreateStatic(tftThread, sizeof(tftThread), NORMALPRIO+1, tft_speed, NULL);
        }
        time_key1_press = chVTGetSystemTime();
        right_flag=0;
      }

      if(left_flag==1)
      {
        flag_key1_press = 0;
        if(flag_start==1 && speed>1)
        {
          speed-=1;
        }
        time_key1_press = chVTGetSystemTime();
        left_flag=0;
      }

      if(tim_flag==1)
      {
        sdWrite(uart3, (uint8_t *)&holl_speed, 2);
        PID_Reg(Reg1,speed*2,holl_speed);
        /*if(tft_flag)
          chThdCreateStatic(tftThread, sizeof(tftThread), NORMALPRIO+1, tft, NULL);*/
        holl_speed=0;
        tim_flag=0;
      }
      if(!flag_key1_press && (time=chVTGetSystemTime() - time_key1_press) > 300)
      {
        flag_key1_press = 1;
      }

     }


}

void Init_PID_Reg(void)
{
  Reg1.P=72;
  Reg1.D=20 ;
  Reg1.I=210;
  Reg1.Summ_Error=0;
  Reg1.Max_Summ_Error=MAX_I_Reg;
  Reg1.Last_Process_Value=0;

}

void Uart_Init(void)
{
  // ��������� ������� � ������
  sdStart(uart3, &uart_conf);
  // ��������� ���� � ��������� Rx, Tx
  palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );
  // �������������� ��������� �� �����
  uart3_stream = (BaseSequentialStream *)uart3;
}


// ������� �������� ������ � ��������
void dbgprintf( const char* format, ... )
{
// ���������, ��� debug_stream_init() ��������
    if ( !uart3_stream )
    return;

// ���������� � chvprintf() ������ � ��� ����
    va_list ap;
    va_start(ap, format);
    chvprintf(uart3_stream, format, ap);
    va_end(ap);
}

// callback ������� �������
void cbgptfun3(GPTDriver *gptp)
{
    (void)gptp;
    palToggleLine(LINE_LED1);
    msg_t msg = chMBPostI(&tft_mb, holl_speed);
    if (msg == MSG_OK)
      tft_flag=1;
    else
      tft_flag=0;
    tim_flag=1;
}

// callback �������, ������� ������ ��������� �� ������������ �������
void holl(void* args)
{
    // �������������� ��������� � ���������� ����, � ������� ������ � uint8_t
    uint8_t arg = *((uint8_t*) args);
    // ��������, ��� �������� ��������� ��������
    if (arg == 5)
    {
      if(palReadPad(GPIOB,8u)==1)
      {
        if(palReadPad(GPIOB,9u)==1)
        {
          holl_speed++;
        }
        if(palReadPad(GPIOB,9u)==0)
        {
          holl_speed--;
        }
      }

      if(palReadPad(GPIOB,8u)==0)
      {
        if(palReadPad(GPIOB,9u)==0)
        {
          holl_speed++;
        }
        if(palReadPad(GPIOB,9u)==1)
        {
          holl_speed--;
        }
      }
    }
}
// callback �������, ������� ������ ��������� �� ������������ �������
void up_button(void* args)
{
    // �������������� ��������� � ���������� ����, � ������� ������ � uint8_t
    uint8_t arg = *((uint8_t*) args);
    // ��������, ��� �������� ��������� ��������
    if (arg == 5)
    {
      up_flag=1;
      /*if(flag_start==0)
      {
        Motor_Forward();

      }*/
      /*palToggleLine(LINE_LED1);
      if(current->prev!=NULL)
      {
        current=current->prev;
        //Menu_Disp();
        //Cursor();
      }*/
    }
}

// callback �������, ������� ������ ��������� �� ������������ �������
void down_button(void* args)
{
    // �������������� ��������� � ���������� ����, � ������� ������ � uint8_t
    uint8_t arg = *((uint8_t*) args);
    // ��������, ��� �������� ��������� ��������
    if (arg == 5)
    {
      down_flag=1;
      /*palToggleLine(LINE_LED1);
      if(current->next!=NULL)
      {
        current=current->next;
        Menu_Disp();
        Cursor();
      }
      */
    }
}

// callback �������, ������� ������ ��������� �� ������������ �������
void right_button(void* args)
{
    // �������������� ��������� � ���������� ����, � ������� ������ � uint8_t
    uint8_t arg = *((uint8_t*) args);
    // ��������, ��� �������� ��������� ��������
    if (arg == 5)
    {
      right_flag=1;
      /*if(current->child!=NULL)
      {
        current=current->child;
        if(current->cmd!=NULL && current->child==NULL)
        {
          switch(current->cmd)
          {
            case 1:
            {
              Motor_Forward();
              TFT_Fill_Screen(0,320,0,240,BLUE);
              TFT_Draw_String(40,110,RED,BLUE,"Motor works",3);
              TFT_Draw_String(40,60,RED,BLUE,"to exit, press LEFT",3);
            }
            break;
            case 2:
              //Motor_Speed();
            break;
            case 3:
              Motor_Stop();
            break;
           }
          }
        }*/
    }
}

// callback �������, ������� ������ ��������� �� ������������ �������
void left_button(void* args)
{
    // �������������� ��������� � ���������� ����, � ������� ������ � uint8_t
    uint8_t arg = *((uint8_t*) args);
    // ��������, ��� �������� ��������� ��������
    if (arg == 5)
    {
      left_flag=1;
      /*if(current->parent!=NULL)
           {
             current=current->parent;
             Menu_Disp();
             Cursor();
           }
    }*/
    }
}

