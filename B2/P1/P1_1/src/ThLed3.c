#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
#include "ThLed2.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_ThLed3;                        // thread id
int Init_ThLed3 (void);                         // Init_Thread function
void ThLed3 (void *argument);                   // thread function
extern osThreadId_t tid_ThLed2;                 // ID ThLed2

GPIO_InitTypeDef led_ld3 = { 
        .Pin = GPIO_PIN_14,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };
typedef struct  {
    GPIO_InitTypeDef pin;
    GPIO_TypeDef *port;
    int delay_287;
} mygpio_pin;

mygpio_pin pinB14;

int Init_ThLed3 (void) {
 
  // Initialize LEDS
    __HAL_RCC_GPIOB_CLK_ENABLE();
  pinB14.pin = led_ld3;
  pinB14.port = GPIOB;
  pinB14.delay_287 = 287;
  const osThreadAttr_t attr = { .name = "Thled3"};
  tid_ThLed3 = osThreadNew(ThLed3, (void *)&pinB14, &attr);
  if (tid_ThLed3 == NULL) {
    return(-1);
  }

  return(0);
}
 
void ThLed3 (void *argument) {
  mygpio_pin *gpio = (mygpio_pin *)argument;
  HAL_GPIO_Init(gpio->port, &(gpio->pin));
  int8_t ciclos_LED3 = 0;
  while (1) {
    
    osThreadFlagsWait(0x0002U, osFlagsWaitAny, osWaitForever);
    //Se realizan 30 ciclos del LED_3
    for(int i = 0; i < 60; i++){
      HAL_GPIO_TogglePin(gpio->port, gpio->pin.Pin);
      if( ciclos_LED3 < 50 ){
          ciclos_LED3++;
        }else if (ciclos_LED3 == 50){
          osThreadFlagsSet(tid_ThLed2,0x0001U);
          ciclos_LED3++;
        }
      osDelay(gpio->delay_287);
    }
    //Se reinicia cuenta de ciclos del LED_3
    ciclos_LED3 = 0;  
    }
}