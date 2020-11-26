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


const uint8_t eeprom_addr = 0xa0;

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
  uint8_t length = 0;
  uint16_t checksum = 0;
  #define i2cCheck(code, msg) \
    if ((TWSR & 0xf8) != (code)) { \
      printf(msg " failed, status: %.2x\r\n", TWSR & 0xf8); \
      i2cReset(); \
      continue; \
    }
  while(1)
  {
    char tab[20];
    scanf("%s", tab);
    if( strcmp(tab, "read") == 0)
    {
      printf("read\r\n");
      scanf("%"SCNx16, &addr);
      scanf("%"SCNx8, &length);
      i2cStart();
      i2cCheck(0x08, "I2C start");
      i2cSend(eeprom_addr | ((addr & 0x100) >> 7)); // 0-255 - do pierwszego, 256-511 - drugiego bloku pamięci
      i2cCheck(0x18, "I2C EEPROM write request");
      i2cSend(addr & 0xff);
      i2cCheck(0x28, "I2C EEPROM set address");
      i2cStart();
      i2cCheck(0x10, "I2C second start");
      i2cSend(eeprom_addr | 0x1 | ((addr & 0x100) >> 7)); // 0x1 - read
      i2cCheck(0x40, "I2C EEPROM read request");
      uint8_t records = length / 16;
      uint8_t last_record_length = length % 16;
      for(int i = 0; i <= records; i++)
      {
        uint8_t res[16] = {0};
        uint8_t record_length = 16;
        if(i == records)
          record_length = last_record_length;
        checksum = 0xFF;
        checksum -= record_length + addr;
        for(int j = 0; j < record_length; j++)
        {
          uint8_t data;
          if(j == record_length - 1 && i == records)
            data = i2cReadNoAck();
          else
            data = i2cReadAck();
          checksum -= data;
          res[j] = data;
        }
        checksum+=1;
        printf(":%02x %04x 00 ", record_length, addr);

        for(int j = 0; j < record_length; j++)
        {
          printf("%02x", res[j]);
        }
        printf("%02x \r\n", checksum);

        addr += 16;
      }
      i2cStop();
      i2cCheck(0xf8, "I2C stop")
      printf(":00000001ff\r\n");
    }
    else if( strcmp(tab, "write") == 0) {
      printf("write\r\n");
      uint16_t byte_count;
      uint16_t type;
      uint16_t checksum1;
      uint16_t data[16];
      char dw;
      scanf("%c", &dw);
      while(1)
      {
        scanf("%c", &dw);
        if(dw != ':')
        {
          printf("nie :\r\n");
          break;
        }
        scanf("%02x%04x%02x", &byte_count, &addr, &type);

        checksum1 = byte_count + addr + type;

        for(int i = 0; i < byte_count; i++)
        {
          scanf("%02x", &data[i]);
          checksum1 += data[i];
        }
        scanf("%02x", &checksum);

        if(checksum1 + checksum != 0x100)
        {
          printf("wrong checksum\r\n");
					break;
        }

        if(byte_count == 0x00 && addr == 0x00 && type == 0x01 && checksum == 0xff)
        {
          printf("odczytano :00000001ff, koniec\r\n");
          break;
        }
        i2cStart();
        i2cCheck(0x08, "I2C start")
        i2cSend(eeprom_addr | ((addr & 0x100) >> 7));
        i2cCheck(0x18, "I2C EEPROM write request");
        i2cSend(addr & 0xff);
        i2cCheck(0x28, "I2C EEPROM set address");

        for(int i = 0; i < byte_count; i++)
        {
          i2cSend(data[i]);
          i2cCheck(0x28, "I2C EEPROM data transmitted");
        }

        i2cStop();
        i2cCheck(0xf8, "I2C stop");
        printf("write continue\r\n");
      }
      /*
      :10 0000 00 00010202040202010102010101010101d9
      :10 0010 00 12010101010101010101010101010101bf
      :10 0020 00 00000000000000000000000000000000d0
      :05 0030 00 0012000000b9
      :00000001ff

      read :05 0030 00 0012010102b5
      */

    }
  }
}
