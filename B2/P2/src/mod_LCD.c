#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 

osThreadId_t tid_th_LCD;                        // thread LCD
osThreadId_t tid_th_LCD_test;                   // thread LCD_test

int Init_th_LCD(void);
int Init_th_LCD_test(void);

void th_LCD(void);
void th_LCD_test(void);

int Init_th_LCD(void){

}

int Init_th_LCD_test(void){


}

void th_LCD(void){
  
  while(1){
  
  }

}

void th_LCD_test(void){
  
  while(1){
  
  
  }
  
}