#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_Thread;                        // thread id
int Init_Thread (void);                         // Init_Thread function
void Thread (void *argument);                   // thread function
extern osThreadId_t tid_ThLed2;                 // ID del thread 2

GPIO_InitTypeDef led_ld1 = { 
        .Pin = GPIO_PIN_0,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };
typedef struct  {
    GPIO_InitTypeDef pin;
    GPIO_TypeDef *port;
    int delay_200;
    int delay_800;
} mygpio_pin;

mygpio_pin pinB0;

int Init_Thread (void) {
 
  // Initialize LED_1
    __HAL_RCC_GPIOB_CLK_ENABLE();
  pinB0.pin = led_ld1;
  pinB0.port = GPIOB;
  pinB0.delay_200 = 200;
  pinB0.delay_800 = 800;
  const osThreadAttr_t attr = { .name = "Thled1"};
  tid_Thread = osThreadNew(Thread, (void *)&pinB0, &attr);
  if (tid_Thread == NULL) {
    return(-1);
  }

  return(0);
}

//Se entiende como 1 ciclo del LED1 como: 1ciclo = encenderse y apagarse 1 vez
void Thread (void *argument) {
  int8_t ciclos_LED1 = 0;
  mygpio_pin *gpio = (mygpio_pin *)argument;
  HAL_GPIO_Init(gpio->port, &(gpio->pin));
  while (1) {
    HAL_GPIO_TogglePin(gpio->port, gpio->pin.Pin);
    osDelay(gpio->delay_200);
    HAL_GPIO_TogglePin(gpio->port, gpio->pin.Pin);
    osDelay(gpio->delay_800);
    
    /****Calculo de los ciclos necesarios para activar LED 2****/
    if(ciclos_LED1 < 5){
      ciclos_LED1++;
    }else if (ciclos_LED1 == 5){
      osThreadFlagsSet(tid_ThLed2, 0x0001U);
      ciclos_LED1++;
    }
  }
}
