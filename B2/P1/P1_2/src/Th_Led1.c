#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_ThLed1;                        // thread id

void Thread_led1 (void *argument);                   // thread function

GPIO_InitTypeDef led_ld1 = {
        .Pin = GPIO_PIN_0,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

typedef struct  {
    GPIO_InitTypeDef pin;
    GPIO_TypeDef *port;
} mygpio_pin;
mygpio_pin pinB0;

int Init_ThLed1 (void) {
 
  // Initialize LEDS
  __HAL_RCC_GPIOB_CLK_ENABLE();
  pinB0.pin= led_ld1;
  pinB0.port=GPIOB;
  tid_ThLed1 = osThreadNew(Thread_led1, (void *)&pinB0, NULL);
  if (tid_ThLed1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_led1 (void *argument) {
 
  mygpio_pin *gpio = (mygpio_pin *)argument;
  HAL_GPIO_Init(gpio->port, &(gpio->pin));
  
  /**LED_1 empieza encendido**/
  HAL_GPIO_WritePin(gpio->port, gpio->pin.Pin, GPIO_PIN_SET);
  
  while (1) {
    /**Espero a que acabe el timer**/
    osThreadFlagsWait(0x0002U, osFlagsWaitAny, osWaitForever);
    /**Al acabar timer apago LED_1**/
    HAL_GPIO_WritePin(gpio->port, gpio->pin.Pin, GPIO_PIN_RESET);

  }
}