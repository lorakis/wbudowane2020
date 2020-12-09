#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#define LED_DDR DDRD
#define LED_PORT PORTD
#define MULT_DDC DDRC
#define MULT_PORT PORTC

#define SHOW(step, delay, number1, number2) \
  for(uint16_t i = 0; i < (uint32_t)1000 * (delay) / (step) / 2; i++) { \
    LED_PORT = 0b11111111; \
    MULT_PORT = (1<<PC0); \
    LED_PORT = ~tab[number1]; \
    _delay_us(step); \
    MULT_PORT = (1<<PC1); \
    LED_PORT = ~tab[number2]; \
    _delay_us(step); \
  }

uint8_t tab[11];


int main()
{

  UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
  LED_DDR |= 0b11111111;
  MULT_DDC |= 0b11111111;

  tab[0] = 0b00111111;
  tab[1] = 0b00000110;
  tab[2] = 0b01011011;
  tab[3] = 0b01001111;
  tab[4] = 0b01100110;
  tab[5] = 0b01101101;
  tab[6] = 0b01111101;
  tab[7] = 0b00000111;
  tab[8] = 0b01111111;
  tab[9] = 0b01101111;
  tab[10]= 0b00000000;
  uint8_t i = 0;
  uint8_t n1 = 0;
  uint8_t n2 = 0;
  while(1)
  {
    n1 = i%10;
    n2 = i/10;
    if( n2 == 0)
      n2 = 10;
    SHOW(200, 1000, n1, n2);
    //SHOW(50, 1000, n1+1, n2+2);
    i++;
    if(i == 60)
      i = 0;
  }
}
