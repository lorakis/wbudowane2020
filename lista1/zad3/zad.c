#include <avr/io.h>
#include <util/delay.h>

#define LED_DDR DDRD
#define LED_PORT PORTD
//xD
void left()
{
  _delay_ms(200);
  LED_PORT <<= 1;
}
void right()
{
  _delay_ms(200);
  LED_PORT >>= 1;
}

int main() {
  UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
  LED_DDR |= 0b11111111;
  LED_PORT |= 0b00000001;
  while(1)
  {
    for(int i = 0; i < 7; i++)
      left();
    for(int i = 0; i < 7; i++)
      right();
  }

}
