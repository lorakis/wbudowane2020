#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "i2c.h"

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
  // wyczyść rejestr UCSR0A
  UCSR0A = 0;
  // włącz odbiornik i nadajnik
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  // ustaw format 8n1
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
  // czekaj aż transmiter gotowy
  while(!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

// odczyt jednego znaku
int uart_receive(FILE *stream)
{
  // czekaj aż znak dostępny
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

FILE uart_file;


const uint8_t eeprom_addr = 0xd0;

int main()
{
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  // zainicjalizuj I2C
  i2cInit();
  // program testowy
  uint16_t addr = 0;

  #define i2cCheck(code, msg) \
    if ((TWSR & 0xf8) != (code)) { \
      printf(msg " failed, status: %.2x\r\n", TWSR & 0xf8); \
      i2cReset(); \
      continue; \
    }
  char tab[20];
  while(1)
  {
    scanf("%s", tab);
    if( strcmp(tab, "date") == 0)
    {
      addr = 0x04;
      i2cStart();
      i2cCheck(0x08, "I2C start");
      i2cSend(eeprom_addr);
      i2cCheck(0x18, "I2C EEPROM write request");
      i2cSend(addr & 0xff);
      i2cCheck(0x28, "I2C EEPROM set address");
      i2cStart();
      i2cCheck(0x10, "I2C second start");
      i2cSend(eeprom_addr | 0x1 );
      i2cCheck(0x40, "I2C EEPROM read request");
      uint8_t day = i2cReadAck();
      day = (day >> 4) * 10 + (day & 0x0f);
      uint8_t month = i2cReadAck();
      month = ((month & 0xf0) >> 4) * 10 + (month & 0x0f);
      uint16_t year = i2cReadNoAck();
      year = 2000 + (year >> 4) * 10 + (year & 0x0f);
      i2cCheck(0x58, "I2C EEPROM read");
      i2cStop();
      i2cCheck(0xf8, "I2C stop");
      printf("Data: %.2d-%.2d-%.4d\r\n", day, month, year);
    } else if( strcmp(tab, "time") == 0)
    {
      addr = 0x00;
      i2cStart();
      i2cCheck(0x08, "I2C start");
      i2cSend(eeprom_addr);
      i2cCheck(0x18, "I2C EEPROM write request");
      i2cSend(addr & 0xff);
      i2cCheck(0x28, "I2C EEPROM set address");
      i2cStart();
      i2cCheck(0x10, "I2C second start");
      i2cSend(eeprom_addr | 0x1 );
      i2cCheck(0x40, "I2C EEPROM read request");
      uint8_t sec = i2cReadAck();
      sec = (sec >> 4) * 10 + (sec & 0x0f);
      uint8_t min = i2cReadAck();
      min = (min >> 4) * 10 + (min & 0x0f);
      uint16_t hours = i2cReadNoAck();
      hours = ((hours & 0x20) >> 4) * 10 + ((hours & 0x10) >> 4) * 10 + (hours & 0x0f);
      i2cCheck(0x58, "I2C EEPROM read");
      i2cStop();
      i2cCheck(0xf8, "I2C stop");
      printf("Godzina: %.2d:%.2d:%.2d\r\n", hours, min, sec);
    } else if( strcmp(tab, "set") == 0)
    {
      scanf("%s", tab);
      if( strcmp(tab, "date") == 0)
      {
        scanf("%s", tab);
        uint8_t day = atoi(&tab[0]);
        day = ((day / 10) << 4) + (day % 10);
        uint8_t month = atoi(&tab[3]);
        month = ((month / 10) << 4) + (month % 10);
        uint16_t year = atoi(&tab[6]);
        year = (((year % 100) / 10) << 4) + (year % 10);
        addr = 0x04;
        i2cStart();
        i2cCheck(0x08, "I2C start");
        i2cSend(eeprom_addr);
        i2cCheck(0x18, "I2C EEPROM write request");
        i2cSend(addr & 0xff);
        i2cCheck(0x28, "I2C EEPROM set address");
        i2cSend(day);
        i2cSend(month);
        i2cSend(year);
        i2cStop();
        i2cCheck(0xf8, "I2C stop");
        printf("Zapisano podana date!\r\n");
      } else if( strcmp(tab, "time") == 0)
      {
        scanf("%s", tab);
        uint8_t hours = atoi(&tab[0]);
        hours = ((hours / 10) % 10 << 4) + (hours % 10);
        uint8_t min = atoi(&tab[3]);
        min = ((min / 10) << 4) + (min % 10);
        uint16_t sec = atoi(&tab[6]);
        sec = ((sec / 10) << 4) + (sec % 10);
        addr = 0x00;
        i2cStart();
        i2cCheck(0x08, "I2C start");
        i2cSend(eeprom_addr);
        i2cCheck(0x18, "I2C EEPROM write request");
        i2cSend(addr & 0xff);
        i2cCheck(0x28, "I2C EEPROM set address");
        i2cSend(sec);
        i2cSend(min);
        i2cSend(hours);
        i2cStop();
        i2cCheck(0xf8, "I2C stop");
        printf("Zapisano podana godzine!\r\n");
      }
    }
  }
}
