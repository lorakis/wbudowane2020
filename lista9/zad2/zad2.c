#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include "hd44780.h"

int hd44780_transmit(char data, FILE *stream)
{
  LCD_WriteData(data);
  return 0;
}



void dodaj_znaki(uint8_t data, uint8_t adres)
{
  for(uint8_t i = 0; i < 8; i++)
  {
    LCD_WriteCommand( HD44780_CGRAM_SET | (adres + i));
    LCD_WriteData(data);
  }
}



FILE hd44780_file;

int main()
{
  // skonfiguruj wyświetlacz
  LCD_Initialize();
  LCD_Clear();
  // skonfiguruj strumienie wyjściowe
  fdev_setup_stream(&hd44780_file, hd44780_transmit, NULL, _FDEV_SETUP_WRITE);
  stdout = stderr = &hd44780_file;
  // program testowy
  // wgrywamy nowe znaki
  uint8_t adres = 0x00;
  uint8_t data  = 0x20;
  for(int i = 0; i < 6; i++)
  {
    dodaj_znaki(data, adres);
    adres += 8;
    data |= (data>>1);
  }


  while(1)
  {
    LCD_GoTo(0, 0);
    for(int i = 0; i < 81; i++)
    {
      LCD_GoTo(0, 0);
      LCD_Clear();
      int k = i/5;
      for(int j = 0; j < k; j++)
        LCD_WriteData(5);
      LCD_WriteData(i%5);
      LCD_GoTo(0, 1);
      printf("%d/81 done", i);
      _delay_ms(250);
    }
  }
}
