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


// Настраиваем частоту третьегоы таймера 50_000Гц (предделитель 4320, целое число, меньше чем 2^16) и указывает первую функцию как обработчик прерываний
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
    //настройка дисплея и инициализация меню
    TFT_Init();
    TFT_Fill_Screen(0,320,0,240,BLUE);
    Menu_Create();
    Menu_GPIO_Init();
    Motor_GPIO_Init();

    //запуск таймера 3
    gptStart(timer3, &gpt3_conf);
    //запуск в непрерывном режиме с периодом 1 с
    gptStartContinuous(timer3, 50000);
    //Настройка ног датчика холла
    palSetPadMode(GPIOB, 8u, PAL_MODE_INPUT);
    palEnablePadEvent(GPIOB, 8u, PAL_EVENT_MODE_RISING_EDGE);
    palSetPadCallback(GPIOB, 8u, holl, &arg);
    palSetPadMode(GPIOB, 9u, PAL_MODE_INPUT);


    char String[15]="RiO";  // буфер, в которую запишем число
    while (1)
    {
      if(up_flag==1)
      {
        chThdSleepMilliseconds(500);
        dbgprintf("Двигатель запущен ");
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
           dbgprintf("Двигатель остановлен ");
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
          dbgprintf("Скважность ШИМ %d %    ", speed);
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
          dbgprintf("Скважность ШИМ %d %  ", speed);
        }
        left_flag=0;
      }
      if(tim_flag==1)
      {
        dbgprintf("Скорость двиг. %d об/с  ", holl_speed1);
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
  // запускаем драйвер в работу
  sdStart(uart3, &uart_conf);
  // Переводим ноги в состояние Rx, Tx
  palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );
  // Переопределяем указатель на поток
  uart3_stream = (BaseSequentialStream *)uart3;
}


// Функция отправки строки в терминал
void dbgprintf( const char* format, ... )
{
// Проверяем, что debug_stream_init() случился
    if ( !uart3_stream )
    return;

// Отправляем в chvprintf() данные и ждём чуда
    va_list ap;
    va_start(ap, format);
    chvprintf(uart3_stream, format, ap);
    va_end(ap);
}

// callback функция таймера
void cbgptfun3(GPTDriver *gptp)
{
    (void)gptp;
    holl_speed1=holl_speed;
    holl_speed=0;
    palToggleLine(LINE_LED1);
    tim_flag=1;
}

// callback функция, которая должна сработать по настроенному событию
void holl(void* args)
{
    // Преобразование аргумента к требуемому типу, в данному случае к uint8_t
    uint8_t arg = *((uint8_t*) args);
    // Проверка, что передача аргумента работает
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
// callback функция, которая должна сработать по настроенному событию
void up_button(void* args)
{
    // Преобразование аргумента к требуемому типу, в данному случае к uint8_t
    uint8_t arg = *((uint8_t*) args);
    // Проверка, что передача аргумента работает
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

// callback функция, которая должна сработать по настроенному событию
void down_button(void* args)
{
    // Преобразование аргумента к требуемому типу, в данному случае к uint8_t
    uint8_t arg = *((uint8_t*) args);
    // Проверка, что передача аргумента работает
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

// callback функция, которая должна сработать по настроенному событию
void right_button(void* args)
{
    // Преобразование аргумента к требуемому типу, в данному случае к uint8_t
    uint8_t arg = *((uint8_t*) args);
    // Проверка, что передача аргумента работает
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

// callback функция, которая должна сработать по настроенному событию
void left_button(void* args)
{
    // Преобразование аргумента к требуемому типу, в данному случае к uint8_t
    uint8_t arg = *((uint8_t*) args);
    // Проверка, что передача аргумента работает
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

