#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PB4
#define BTN_PIN PINB
#define BTN_PORT PORTB
bool tab[102];

int main()
{
  BTN_PORT |= _BV(BTN);
  LED_DDR |= _BV(LED);
  int i = 0;
  while(1)
  {

    if( tab[(i+1) % 101] == true)
      LED_PORT &= ~_BV(LED);
    else
      LED_PORT |= _BV(LED);
    if (BTN_PIN & _BV(BTN))
      tab[i] = true;
    else
      tab[i] = false;
    i++;
    if( i >= 101 )
    {
      i = 0;
    }
    _delay_ms(10);
  }

}
