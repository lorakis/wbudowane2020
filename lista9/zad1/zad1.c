#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "hd44780.h"

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

int hd44780_transmit(char data, FILE *stream)
{
  LCD_WriteData(data);
  return 0;
}
// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
  // wyczyść rejestr UCSR0A
  UCSR0A = 0;
  // włącz odbiornik i nadajnik
  UCSR0B = _BV(RXEN0) | _BV(RXCIE0);
  // ustaw format 8n1
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}



char row[16];
int8_t pos = 0;
int8_t y = 0;

ISR(USART_RX_vect)
{
  row[pos] = UDR0;

  if(row[pos] == '\r')
  {
    row[pos] = '\0';
    LCD_Clear();
    LCD_GoTo(15-pos, 0);
    printf("%s", row);
    y = 1;
    LCD_GoTo(15, 1);
    pos = 0;
    for(int i = 0; i < 16; i++)
      row[i] = '\0';
    return;
  }
  else
  {
    LCD_GoTo(15-pos-1, y);
    printf("%s", row);
  }
  pos++;
  if(pos == 16)
  {
    LCD_Clear();
    LCD_GoTo(0, 0);
    printf("%s", row);
    y = 1;
    LCD_GoTo(15, y);
    pos = 0;
    for(int i = 0; i < 16; i++)
      row[i] = '\0';
    return;
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

  //uint16_t v = 0;
  sei();
  uart_init();
  set_sleep_mode(SLEEP_MODE_IDLE);
  LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON | HD44780_CURSOR_BLINK);
  LCD_GoTo(15, 0);
  while(1) {
    sleep_mode();
  }
}
