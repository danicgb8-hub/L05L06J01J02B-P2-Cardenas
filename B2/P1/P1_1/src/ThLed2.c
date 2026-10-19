#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
#include "ThLed2.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_ThLed2;                 // thread id
int Init_ThLed2 (void);                  // Init_Thread function
void ThLed2 (void *argument);            // thread function
extern osThreadId_t tid_ThLed3;          // ID de ThLed3

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
 
  // Initialize LED_2
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
  int8_t ciclos_LED2 = 0;
  while (1) {
    
    osThreadFlagsWait(0x0001U, osFlagsWaitAny, osWaitForever);
    /***Cuando llega el flag realizamos 20 ciclos del LED_2***/
    for(int i = 0; i < 40; i++){
      HAL_GPIO_TogglePin(gpio->port, gpio->pin.Pin);
      if( ciclos_LED2 < 30){
          ciclos_LED2++;
        }else if( ciclos_LED2 == 30 ){
          osThreadFlagsSet(tid_ThLed3, 0x0002U);
          ciclos_LED2++;
        }
      osDelay(gpio->delay_137);
    }
    //Volvemos a poner los ciclos de LED_2 a cero (controlan cuando se enciende LED_3)
    ciclos_LED2 = 0;  
    }
}