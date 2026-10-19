#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_ThLed3;                        // thread id 
void Thread_led3 (void *argument);                   // thread function

GPIO_InitTypeDef led_ld3 = { 
        .Pin = GPIO_PIN_14,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

typedef struct  {
    GPIO_InitTypeDef pin;
    GPIO_TypeDef *port;
} mygpio_pin;
mygpio_pin pinB14;

int Init_ThLed3 (void) {
 
  // Initialize LEDS
    __HAL_RCC_GPIOB_CLK_ENABLE();
  pinB14.pin= led_ld3;
  pinB14.port=GPIOB;
  tid_ThLed3 = osThreadNew(Thread_led3, (void *)&pinB14, NULL);
  if (tid_ThLed3 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_led3 (void *argument) {
  mygpio_pin *gpio = (mygpio_pin *)argument;
  HAL_GPIO_Init(gpio->port, &(gpio->pin));
  /**LED_3 empieza apagado**/
  HAL_GPIO_WritePin(gpio->port, gpio->pin.Pin, GPIO_PIN_RESET);
  while (1) {
    /**Espero a que acabe el timer**/
    osThreadFlagsWait(0x0002U, osFlagsWaitAny, osWaitForever);
    /**Al acabar timer enciendo LED_3**/
    HAL_GPIO_WritePin(gpio->port, gpio->pin.Pin, GPIO_PIN_SET);
  }
}