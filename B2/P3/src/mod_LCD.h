#ifndef __MOD_LCD_H
#define __MOD_LCD_H
  #include "cmsis_os2.h" 
  int Init_th_LCD(void);
  int Init_th_LCD_test(void);
  void Escribe_L1(uint8_t *cadena);
  void Escribe_L2(uint8_t *cadena);
  
#endif
