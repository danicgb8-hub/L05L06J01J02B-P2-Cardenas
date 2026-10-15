#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
#include "ThLed2.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_ThLed2;                        // thread id
int Init_ThLed2 (void);                         // Init_Thread function
void ThLed2 (void *argument);                   // thread function

GPIO_InitTypeDef led_ld2 = { 
        .Pin = GPIO_PIN_7,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };
typedef struct  {
    GPIO_InitTypeDef pin;
    GPIO_TypeDef *port;
    int delay_137;
} mygpio_pin;

mygpio_pin pinB7;

int Init_ThLed2 (void) {
 
  // Initialize LEDS
    __HAL_RCC_GPIOB_CLK_ENABLE();
  pinB7.pin = led_ld2;
  pinB7.port = GPIOB;
  pinB7.delay_137 = 137;
  const osThreadAttr_t attr = { .name = "Thled2"};
  tid_ThLed2 = osThreadNew(ThLed2, (void *)&pinB7, &attr);
  if (tid_ThLed2 == NULL) {
    return(-1);
  }

  return(0);
}
 
void ThLed2 (void *argument) {
  mygpio_pin *gpio = (mygpio_pin *)argument;
  HAL_GPIO_Init(gpio->port, &(gpio->pin));
  while (1) {
    HAL_GPIO_TogglePin(gpio->port, gpio->pin.Pin);
    osDelay(gpio->delay_137);
  }
}
