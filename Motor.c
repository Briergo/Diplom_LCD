#include "Motor.h"



// ������� ��������� �� ������� � ����������
static PWMDriver *pwm1Driver = &PWMD1;

//uint32_t speed =0;

PWMConfig pwm1conf = {
    // ������ ������� 500��� (������������ ����� 436, ��� ��� �������� ����������)
    .frequency = 200000,
    // ������ ����� 10000 ������ �������, �� ���� 0,02 �������
    .period    = 10000,
    // �� ����� ������������ ���������� �� ��������� �������
    .callback  = NULL,
    // �������� ������ ������ ���� � ��������� ACTIVE_HIGH � ��� �� �� ����� ������������ ����������
    .channels  = {
                  {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
                  {.mode = PWM_OUTPUT_ACTIVE_HIGH,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL}
                  },
    // �������� ������� � �����. �������� ��������, advanced ������� ������� ���������, ������� �������� bdtr ���
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
  // �������� ������ � ������
  pwmStart( &PWMD1, &pwm1conf );
  // �������� ����� � ������������� ���������� 50% (����� ������ �������� ��� ���������� ������, 5000 / 10000 = 0,5)
  pwmEnableChannel( &PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1,speed) );
}







