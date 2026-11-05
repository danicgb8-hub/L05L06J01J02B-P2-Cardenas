#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
#include <Driver_SPI.h>
#include "Arial12x12.h"

osThreadId_t tid_th_LCD;                        // thread LCD
osThreadId_t tid_th_LCD_test;                   // thread LCD_test

//Funciones de LCD
void LCD_reset(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char data);
void LCD_init(void);
void LCD_update(void);
void LCD_symbolTolocalBuffer_L1(uint8_t symbol);

// Funciones inicializacion de los thread
int Init_th_LCD(void);
int Init_th_LCD_test(void);

// Funcion de cada thread
void th_LCD(void* arg);
void th_LCD_test(void* arg);

//Funcion Callback SPI
void cb_spi(uint32_t event){
  
  if( event & ARM_SPI_EVENT_TRANSFER_COMPLETE ){
      /*TODO HA IDO BIEN*/
      osThreadFlagsSet(tid_th_LCD, 0x01U);
  } else if((event & ARM_SPI_EVENT_DATA_LOST) || (event & ARM_SPI_EVENT_MODE_FAULT)){
      /*HA HABIDO ALGUN ERROR*/
      osThreadFlagsSet(tid_th_LCD, 0x02U);
  }
  
}

//Variables SPI
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

//variables GPIO
GPIO_InitTypeDef gpio;

unsigned char buffer[512];

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
  LCD_init();
  
  while(1){
    osThreadFlagsWait(0x03U, osFlagsWaitAny, osWaitForever);
    LCD_symbolTolocalBuffer_L1('H');
    LCD_update();
  }
}

void th_LCD_test(void* arg){
  
  osDelay(1000);
  osThreadFlagsSet(th_LCD_test, 0x03U);
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


//LCD_wr_data : Función que escribe un dato en el LCD.
void LCD_wr_data(unsigned char data){
  
  /*Ponemos CS = 0; Activo a nivel bajo*/
  /*CS sirve para seleccionar y activar al esclavo*/
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
 /*Ponemos A0 = 1; Para que lo que mandemos sea escrito y no un dato*/
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
  
  /*Escribimos un dato en LCD*/
  SPIdrv->Send(&data, sizeof(data));
  
  uint32_t flag = osThreadFlagsWait(0x01U | 0x02U, osFlagsWaitAny, osWaitForever);
  if(flag & 0x02U){
    /*ERROR*/
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
  } else if (flag & 0x01U){
      /*CS = 1*/
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
  }
}

//LCD_wr_data : Función que envia comando al LCD.
void LCD_wr_cmd(unsigned char data){
  
  /*Ponemos CS = 0; Activo a nivel bajo*/
  /*CS sirve para seleccionar y activar al esclavo*/
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
 /*Ponemos A0 = 0; Para que lo que mandemos sea escrito y no un dato*/
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
  
  /*Escribimos un dato en LCD*/
  SPIdrv->Send(&data, sizeof(data));
  
  uint32_t flag = osThreadFlagsWait(0x01U | 0x02U, osFlagsWaitAny, osWaitForever);
  if(flag & 0x02U){
    /*ERROR*/
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
  } else if (flag & 0x01U){
      /*CS = 1*/
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    }
}

/*Inicializa al LCD*/
void LCD_init(void){
  
  LCD_wr_cmd(0xAE); //Display off
  LCD_wr_cmd(0xA2); //Fija la relacion tension de polarizacion a 1/9
  LCD_wr_cmd(0xA0); //Direccionamiento de RAM es el normal
  LCD_wr_cmd(0xC8); //scan en salidas COM es el normal
  LCD_wr_cmd(0x22); //Fija relacion de resistencia internas a 2
  LCD_wr_cmd(0x2F); //Power on
  LCD_wr_cmd(0x40); //Display empieza en la linea 0 
  LCD_wr_cmd(0xAF); //DISPLAY ON
  LCD_wr_cmd(0x81); //Contraste
  
  LCD_wr_cmd(0x08); //Valor Contraste
  
  LCD_wr_cmd(0xA4); //Display all points normal
  LCD_wr_cmd(0xA6); //LCD display normal
}

//Actualiza lo que se muestra en pantalla
void LCD_update(void){
  
  int i ;
  LCD_wr_cmd (0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd (0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd (0xB0); // Página 0
  for (i =0; i <128; i++){
      LCD_wr_data ( buffer[i]) ;
   }
   LCD_wr_cmd (0x00); // 4 bits de la parte baja de la dirección a 0
   LCD_wr_cmd (0x10); // 4 bits de la parte alta de la dirección a 0
   LCD_wr_cmd (0xB1); // Pá gina 1
   for (i =128; i <256; i++) {
      LCD_wr_data ( buffer[i]) ;
   }
   LCD_wr_cmd (0x00);
   LCD_wr_cmd (0x10);
   LCD_wr_cmd (0xB2); //Pá gina 2
   for (i =256; i <384; i++) {
    LCD_wr_data ( buffer[i]) ;
   }
   LCD_wr_cmd (0x00);
   LCD_wr_cmd (0x10);
   LCD_wr_cmd (0xB3); // Pagina 3
   for (i =384; i <512; i++) {
   LCD_wr_data ( buffer[i]) ;
   }
}


void LCD_symbolTolocalBuffer_L1(uint8_t symbol){
  /*Value1 --> Valor que se escribira en la pagina 0 del LCD*/
  /*Value2--> Valor que se escribe en las 4 primeras filas de la pagina 1*/
  /*Como value 2 es de la pagina 1, al incluir su valor en el buffer se le suma 128 (0 pagina)*/
  
  
  static uint16_t positionL1 = 0;
  uint8_t i, value1, value2;
  uint16_t offset = 0;
  
  /*Indica donde esta el primer dato del simbolo que quiero escribir en tabla Arial12x12*/
  offset = 25*(symbol - ' ');
  
  /*for de 12 iteraciones, el ancho maximo que puede tener una letra*/
  for(i = 0; i<12; i++){
    value1 = Arial12x12[offset+i*2+1];
    value2 = Arial12x12[offset+i*2+2];
    
    /*Guardo los valores de pag0 y pag1 (4 primeras filas) en el buffer*/
    buffer[i + positionL1] = value1;
    buffer[i +128 + positionL1] = value2;
  }
  /*Avanzo el cursor de la pagina 0*/
  positionL1 = positionL1 + Arial12x12[offset];
}