#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"                      // Incluimos capa HAL para GPIO

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThJoy;                        // thread id

/**Declaracion de LEDs**/
GPIO_InitTypeDef Leds;                        // LED_1 --> Menos significativo
/**Declaracion GPIO Pulsadores**/
GPIO_InitTypeDef JoyStick;                    // Variable para configurar pines
/**Declaracion Timer gestion de rebotes**/
osTimerId_t timRebotes_id;

typedef struct {
  
  uint32_t arriba;
  uint32_t abajo;
  uint32_t izquierda;
  uint32_t derecha;
  uint32_t pulsar;
  
}pulsaciones;

pulsaciones contador;


/**---- ISR Pulsaciones ----**/
void EXTI15_10_IRQHandler(){
  //Le doy un pin cualquiera del JoyStick, luego leere cual se ha activado
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if( GPIO_Pin == GPIO_PIN_10 || GPIO_Pin == GPIO_PIN_11 || GPIO_Pin == GPIO_PIN_12
      || GPIO_Pin == GPIO_PIN_14 || GPIO_Pin == GPIO_PIN_15){
        
        osThreadFlagsSet(tid_ThJoy, 0x0001);
      }
}


/**Funcion CallBack del timer**/
void TimRebotes_CallBack(void* arg){
  
  /**Una vez acabado el timer gestionar: que se ha pulsado, actualizar variable y mostrar en LEDs**/
  
  /**Estructura if else para ver que accion se ha realizado**/
  //---- Lectura ARRIBA
  if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET){
    //---- Escribimos 001 en los LEDs
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    
    contador.arriba++;
    
    //---- Lectura DERECHA
  } else if ( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_SET ){
      //---- Escribimos 010 en los LEDs
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
      
      contador.derecha++;
    //---- Lectura ABAJO
    } else if ( HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_SET ){
        //---- Escribimos 011 en los LEDs
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
      
        contador.abajo++;
      
      //---- Lectura IZQUIERDA
      } else if ( HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14) == GPIO_PIN_SET ){
          //---- Escribimos 100 en los LEDs
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
        
        contador.izquierda++;
        
        //---- Lectura PULSAR
        }else if ( HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15) == GPIO_PIN_SET ){
            //---- Escribimos 101 en los LEDs
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
          
            contador.pulsar++;
        }
}


 
void Th_Joy (void *argument);                // thread function
void Init_Puertos(void);                     // Inicializa LEDs y puertos interrupciones pulsaciones
 
int Init_ThJoy (void) {
  
  contador.arriba = 0;
  contador.abajo = 0;
  contador.izquierda = 0;
  contador.derecha = 0;
  contador.pulsar = 0;
  
  Init_Puertos();
  timRebotes_id = osTimerNew(TimRebotes_CallBack, osTimerOnce, NULL, NULL);
  
  const osThreadAttr_t attr = { .name = "ThJoy" };
  tid_ThJoy = osThreadNew(Th_Joy, NULL, &attr);
  if (tid_ThJoy == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Th_Joy (void *argument) {
  while (1) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
    osTimerStart(timRebotes_id, 50);
  }
}

void Init_Puertos(void){
/**---- Inicializacion RELOJES puertos a usar (B y E) ----**/
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
  
/**---- Inicializacion GPIO LEDs ----**/
  Leds.Speed = GPIO_SPEED_FREQ_HIGH;
  Leds.Mode = GPIO_MODE_OUTPUT_PP;
  Leds.Pull = GPIO_NOPULL;
  
  //---- LED_1
  Leds.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB, &Leds);

  //---- LED_2
  Leds.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &Leds);

  //---- LED_3
  Leds.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &Leds);
  
/**---- Inicializacion GPIO JoyStick ----**/
  JoyStick.Mode = GPIO_MODE_IT_RISING;
  JoyStick.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  JoyStick.Pull = GPIO_PULLDOWN;
  
  //---- Habilito INTERRUPCIONES
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
  //---- Arriba
  JoyStick.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOB, &JoyStick);
  
  //---- Derecha
  JoyStick.Pin = GPIO_PIN_11;
  HAL_GPIO_Init(GPIOB, &JoyStick);
  
  //---- Izquierda
  JoyStick.Pin = GPIO_PIN_12;
  HAL_GPIO_Init(GPIOE, &JoyStick);
  
  //---- Abajo
  JoyStick.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOE, &JoyStick);
  
  //---- Pulsar
  JoyStick.Pin = GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &JoyStick);
  

}
