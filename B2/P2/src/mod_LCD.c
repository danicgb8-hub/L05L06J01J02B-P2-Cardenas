#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
#include <Driver_SPI.h>>

osThreadId_t tid_th_LCD;                        // thread LCD
osThreadId_t tid_th_LCD_test;                   // thread LCD_test

//Funciones de LCD
void LCD_reset(void);

// Funciones inicializacion de los thread
int Init_th_LCD(void);
int Init_th_LCD_test(void);

// Funcion de cada thread
void th_LCD(void* arg);
void th_LCD_test(void* arg);

//Funcion Callback SPI
void cb_spi(uint32_t event){
  
  if( event & ARM_SPI_EVENT_TRANSFER_COMPLETE){
      osThreadFlagsSet(tid_th_LCD, 0x01U);
  } else if((event & ARM_SPI_EVENT_DATA_LOST) || (event & ARM_SPI_EVENT_MODE_FAULT)){
      osThreadFlagsSet(tid_th_LCD, 0x02U);
  }
  
}

//Variables SPI
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

//variables GPIO
GPIO_InitTypeDef gpio;

int Init_th_LCD(void){
  
  const osThreadAttr_t nombre_LCD = { .name = "th_LCD" };
  tid_th_LCD = osThreadNew(th_LCD,NULL, &nombre_LCD);
  if( tid_th_LCD == NULL){
      return(-1);
    }
  return(0);
}

int Init_th_LCD_test(void){
  
  const osThreadAttr_t nombre_test = {.name = "th_LCD_test"};
  tid_th_LCD_test = osThreadNew(th_LCD_test, NULL, &nombre_test);
  if(tid_th_LCD_test == NULL){
      return(-1);
    }
  return(0);
  
}

void th_LCD(void* arg){
  
  LCD_reset();
  
  while(1){
  
  }

}

void th_LCD_test(void* arg){
  
  while(1){
  
  
  }
  
}


void LCD_reset(void){
  
  //Inicializacion del SPI con funcion callback
  SPIdrv->Initialize(cb_spi);
  //Damos power al SPI
  SPIdrv->PowerControl(ARM_POWER_FULL);
  //Decimos como queremos que se comporte el SPI
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
  
  /********INICIALIZACION GPIOs*******/
  gpio.Pull = GPIO_PULLUP;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
  
  //Señal de RESET
  __HAL_RCC_GPIOA_CLK_ENABLE();
  gpio.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOA, &gpio);
  
  //Señal CS
  __HAL_RCC_GPIOD_CLK_ENABLE();
  gpio.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOD, &gpio);
  
  //Señal A0
  __HAL_RCC_GPIOF_CLK_ENABLE();
  gpio.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOF, &gpio);
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  osDelay(1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  osDelay(1);
  
}