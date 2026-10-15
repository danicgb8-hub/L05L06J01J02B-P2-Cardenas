#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_Thread;                        // thread id
int Init_Thread (void);                         // Init_Thread function
void Thread (void *argument);                   // thread function

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
 
  // Initialize LEDS
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
 
void Thread (void *argument) {
  mygpio_pin *gpio = (mygpio_pin *)argument;
  HAL_GPIO_Init(gpio->port, &(gpio->pin));	
  while (1) {
    HAL_GPIO_TogglePin(gpio->port, gpio->pin.Pin);
    osDelay(gpio->delay_200);
    HAL_GPIO_TogglePin(gpio->port, gpio->pin.Pin);
    osDelay(gpio->delay_800);
  }
}
