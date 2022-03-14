#include "ch.h"
#include "hal.h"
#include "menu.h"
#include "TFT_8080.h"
#include "Motor.h"
#include "chprintf.h"
#include <stdio.h>

uint32_t impulseWidth=0 ;
uint16_t speed=5000 ;
uint8_t flag_start=0;
uint8_t up_flag=0;
uint8_t down_flag=0;
uint8_t tim_flag=0;
uint8_t left_flag=0;
uint8_t right_flag=0;
int16_t holl_speed=0;
int16_t holl_speed1=0;


void Uart_Init(void);
void dbgprintf( const char* format, ... );
void cbgptfun3(GPTDriver *gptp);
void holl(void* args);

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


int main(void)
{
    uint8_t arg = 5;
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
    Menu_Create();
    Menu_GPIO_Init();
    Motor_GPIO_Init();

    //������ ������� 3
    gptStart(timer3, &gpt3_conf);
    //������ � ����������� ������ � �������� 1 �
    gptStartContinuous(timer3, 50000);
    //��������� ��� ������� �����
    palSetPadMode(GPIOB, 8u, PAL_MODE_INPUT);
    palEnablePadEvent(GPIOB, 8u, PAL_EVENT_MODE_RISING_EDGE);
    palSetPadCallback(GPIOB, 8u, holl, &arg);
    palSetPadMode(GPIOB, 9u, PAL_MODE_INPUT);


    char String[15]="RiO";  // �����, � ������� ������� �����
    while (1)
    {
      if(up_flag==1)
      {
        chThdSleepMilliseconds(500);
        dbgprintf("��������� ������� ");
        if(flag_start==0)
        {
          Motor_Forward();

        }
        up_flag=0;
      }
      if(down_flag==1)
      {
        chThdSleepMilliseconds(500);
         if(flag_start==1)
         {
           Motor_Stop();
           dbgprintf("��������� ���������� ");
         }
         down_flag=0;
      }
      if(right_flag==1)
      {
        chThdSleepMilliseconds(500);
        if(flag_start==1 && speed<10000)
        {
          speed+=100;
          Motor_Speed();
          dbgprintf("���������� ��� %d %    ", speed);
        }
        right_flag=0;
      }
      if(left_flag==1)
      {
        chThdSleepMilliseconds(500);
        if(flag_start==1 && speed>1000)
        {
          speed-=100;
          Motor_Speed();
          dbgprintf("���������� ��� %d %  ", speed);
        }
        left_flag=0;
      }
      if(tim_flag==1)
      {
        dbgprintf("�������� ����. %d ��/�  ", holl_speed1);
        tim_flag=0;
        sprintf(String,"%d",holl_speed1);
        dbgprintf(" %d   ", String);
        TFT_Fill_Screen(10,200,0,20,BLUE);
        TFT_Draw_String(10,200,RED,BLUE,String,3);
      }

     }


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
    holl_speed1=holl_speed;
    holl_speed=0;
    palToggleLine(LINE_LED1);
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
      if(palReadPad(GPIOB,9u)==1)
      {
        holl_speed++;
      }
      if(palReadPad(GPIOB,9u)==0)
      {
        holl_speed--;
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
      /*if(flag_start==1)
      {
        Motor_Forward();

      }*/
      /*palToggleLine(LINE_LED1);
      if(current->prev!=NULL)
      {
        current=current->prev;
        /*Menu_Disp();
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

