#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
extern osThreadId_t tid_ThTimer;                // ID thread TIMER
 
void Thread_bottom (void *argument);                   // thread function

/**Configuracion PIN Pulsador de Usuario**/
GPIO_InitTypeDef bottom_pin = {
  .Pin = GPIO_PIN_13,
  .Mode = GPIO_MODE_IT_RISING,
  .Pull = GPIO_NOPULL,
  .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

void EXTI15_10_IRQHandler(){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == GPIO_PIN_13){
    osThreadFlagsSet(tid_ThTimer, 0x0001U);
  }
}

 
int Init_ThBottom (void) {
 
  /**Habilito interrupciones del boton**/
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
  tid_Thread = osThreadNew(Thread_bottom, (void*) &bottom_pin, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_bottom (void *argument) {
  GPIO_InitTypeDef *gpio = (GPIO_InitTypeDef*) argument;
  __HAL_RCC_GPIOC_CLK_ENABLE();     //Se activa reloj del Puerto C
  HAL_GPIO_Init(GPIOC, gpio);
  
  while (1) {
    
  }
}
