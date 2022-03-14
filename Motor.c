#include "Motor.h"



// Положим указатель на драйвер в переменную
static PWMDriver *pwm1Driver = &PWMD1;

//uint32_t speed =0;

PWMConfig pwm1conf = {
    // Укажем частоту 500кГц (предделитель равен 436, так что значение допустимое)
    .frequency = 200000,
    // Период равен 10000 тактам таймера, то есть 0,02 секунды
    .period    = 10000,
    // Не будем использовать прерывание по окончанию периода
    .callback  = NULL,
    // Настроим только первый кнал в состояние ACTIVE_HIGH и так же не будем использовать прерывание
    .channels  = {
                  {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
                  {.mode = PWM_OUTPUT_ACTIVE_HIGH,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL}
                  },
    // Регистры оставим в покое. Обратите внимание, advanced функции таймера выключены, поэтому регистра bdtr нет
    .cr2        = 0,
    .dier       = 0
};

void Motor_GPIO_Init(void)
{
  palSetPadMode(IN2_PORT,IN2_PIN,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IN1_PORT,IN1_PIN,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(ENA_PORT,ENA_PIN,  PAL_MODE_ALTERNATE(1) );
}
void Motor_Forward(void)
{
  IN1_HI;
  IN2_LO;
  Motor_PWMD();
  flag_start=1;

}
void Motor_Back(void)
{
  IN1_LO;
  IN2_HI;
}
void Motor_Stop(void)
{
  IN1_LO;
  IN2_LO;
  flag_start=0;
}

void Motor_Speed(void)
{
  pwmEnableChannel( &PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1,speed) );

}

void Motor_PWMD (void)
{
  // Запустим модуль в работу
  pwmStart( &PWMD1, &pwm1conf );
  // Запустим канал с коэффициентом заполнения 50% (здесь третий аргумент это количество тактов, 5000 / 10000 = 0,5)
  pwmEnableChannel( &PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1,speed) );
}







