#include <avr/io.h>
#include <util/delay.h>

#define LED_DDR DDRD
#define LED_PORT PORTD

int main() {
  uint8_t tab[10];
  UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
  LED_DDR |= 0b11111111;

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

  while(1)
  {

    for(int i = 0; i < 10; i++)
    {
      LED_PORT = ~tab[i];
      _delay_ms(1000);
    }

  }

}
