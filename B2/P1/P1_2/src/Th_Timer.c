#include "cmsis_os2.h"                          // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThTimer;                        // thread id
extern osThreadId_t tid_ThLed1;                  // ID thread LED_1
extern osThreadId_t tid_ThLed3;                  // ID thread LED_3
 
void Thread (void *argument);                   // thread function

void TimerLed_CallBack(void *arg){
  osThreadFlagsSet(tid_ThLed1, 0x0002U);
  osThreadFlagsSet(tid_ThLed3, 0x0002U);
}
 
int Init_ThTimer (void) {
  
  osTimerId_t timer_id;
  timer_id = osTimerNew(TimerLed_CallBack, osTimerOnce,NULL, NULL);
  
  tid_ThTimer = osThreadNew(Thread, (void*)timer_id, NULL);
  if (tid_ThTimer == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
  osThreadId_t tim_id = (osThreadId_t*) argument;
  //Inicio timer nada mas se inicia el programa
  osTimerStart(tim_id, 3000);
  while (1) {
    /**Espero a ver si se pulsa el boton**/
    osThreadFlagsWait(0x0001U, osFlagsWaitAny,osWaitForever);
    /**Si se pulsa el boton reinicio el timer**/
    osTimerStart(tim_id, 3000);
  }
}
