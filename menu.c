#include "menu.h"
#include <stdlib.h>
#include "TFT_8080.h"



/*Vspomogat peremenn*/
node_t* current=NULL;
int count=0;
char dataled=0xFF;

/*Create glavnoe menu*/ 
node_t* menu1=NULL;
node_t* menu2=NULL;
node_t* menu3=NULL;
/*Create podmenu menu*/
node_t* menu1_1=NULL;
node_t* menu2_1=NULL;
node_t* menu3_1=NULL;
void Menu_GPIO_Init(void)
{
  uint8_t arg = 5;
  // А ногу с кнопкой на вход
  palSetPadMode(UP_GPIO_Port, UP_Pin, PAL_MODE_INPUT_PULLDOWN);
  // Разрешаем прерывание, указываем callbcak функцию и передаем указатель на аргумент
  palEnablePadEvent(UP_GPIO_Port, UP_Pin, PAL_EVENT_MODE_RISING_EDGE);
  palSetPadCallback(UP_GPIO_Port, UP_Pin, up_button, &arg);

  palSetPadMode(DOWN_GPIO_Port, DOWN_Pin, PAL_MODE_INPUT_PULLDOWN);
  palEnablePadEvent(DOWN_GPIO_Port, DOWN_Pin, PAL_EVENT_MODE_RISING_EDGE);
  palSetPadCallback(DOWN_GPIO_Port, DOWN_Pin, down_button, &arg);

  palSetPadMode(LEFT_GPIO_Port, LEFT_Pin, PAL_MODE_INPUT_PULLDOWN);
  palEnablePadEvent(LEFT_GPIO_Port, LEFT_Pin, PAL_EVENT_MODE_RISING_EDGE);
  palSetPadCallback(LEFT_GPIO_Port, LEFT_Pin, left_button, &arg);

  palSetPadMode(RIGHT_GPIO_Port, RIGHT_Pin, PAL_MODE_INPUT_PULLDOWN);
  palEnablePadEvent(RIGHT_GPIO_Port, RIGHT_Pin, PAL_EVENT_MODE_RISING_EDGE);
  palSetPadCallback(RIGHT_GPIO_Port, RIGHT_Pin, right_button, &arg);
}
void Menu_Create(void)
{
   /* MENU 1*/
  menu1=malloc(sizeof(node_t));
  menu1->prev=NULL;
  menu1->parent=NULL;
  menu1->text="Motor Forward";
  menu1->next=malloc(sizeof(node_t));
  menu1->child=malloc(sizeof(node_t));
  menu1->cmd=1;
  menu1->x_pos=20;
  menu1->y_pos=40;
  /* MENU 1_1*/
  menu1_1=menu1->child;
  menu1_1->parent=menu1;
  menu1_1->text="Menu 1_1";
  menu1_1->child=NULL;
  menu1_1->prev=NULL;
  menu1_1->next=NULL;
  /* MENU 1_2*/
 /*menu1_2=menu1_1->next;
  menu1_2->parent=menu1;
  menu1_2->text="Menu 1_2";
  menu1_2->child=NULL;
  menu1_2->prev=menu1_1;
  menu1_2->next=NULL;
  menu1_2->x_pos=80;
  menu1_2->y_pos=100;*/
  /* MENU 2*/
  menu2 = menu1->next; 
  menu2->child=NULL;
  menu2->parent=NULL;
  menu2->text="Motor Speed";
  menu2->prev=menu1;
  menu2->next=malloc(sizeof(node_t));
  menu2->x_pos=20;
  menu2->cmd=2;
  menu2->y_pos=100;
  /* MENU 2_1*/
  /*menu2->child=malloc(sizeof(node_t));
  menu2_1=menu2->child;
  menu2_1->child=NULL;
  menu2_1->text="Menu 2_1";
  menu2_1->prev=NULL;
  menu2_1->parent=menu1;
  menu2_1->next=malloc(sizeof(node_t));
  menu2_1->x_pos=80;
  menu2_1->y_pos=40;*/
  /* MENU 2_1*/
 /* menu2_2=menu2_1->next;
  menu2_2->text="Menu 2_2";
  menu2_2->child=NULL;
  menu2_2->parent=menu1;
  menu2_2->prev=menu2_1;
  menu2_2->next=NULL;
  menu2_2->x_pos=80;
  menu2_2->y_pos=100;*/
  /* MENU 3*/
  menu3=menu2->next;
  menu3->child=NULL;
  menu3->parent=NULL;
  menu3->text="Motor Stop";
  menu3->prev=menu2;
  menu3->next=NULL;
  menu3->cmd=3;
  menu3->x_pos=20;
  menu3->y_pos=160;
  /* MENU 3_1*/
  /*
  menu3_1=menu3->child;
  menu3_1->next=malloc(sizeof(node_t));
  menu3_1->parent=menu1;
  menu3_1->child=NULL;
  menu3_1->prev=NULL;
  menu3_1->text="Menu 3_1";
  menu3_1->x_pos=80;
  menu3_1->y_pos=40;
  /* MENU 3_2*/
  /*menu3_2=menu3_1->next;
  menu3_2->next=NULL;
  menu3_2->parent=menu1;
  menu3_2->prev=menu3_1;
  menu3_2->child=malloc(sizeof(node_t));
  menu3_2->text="Menu 3_2";
  menu3_2->x_pos=80;
  menu3_2->y_pos=100;*/
  /*Menu init*/
  current=menu1;
  Menu_Disp();
  Cursor();
 
}

void Menu_Disp(void)
{
  TFT_Fill_Screen(0,320,0,240,WHITE);
  node_t* i;
  i=current;
  while(i->prev!=NULL)
        i=i->prev;
  while(i!=NULL)
  {
      TFT_Draw_String(i->x_pos,i->y_pos, BLACK, WHITE, i->text, 2);
      i=i->next;
  }  
}

void Cursor(void)
{
    TFT_Draw_String(current->x_pos,current->y_pos, RED, WHITE, current->text, 2);
}

