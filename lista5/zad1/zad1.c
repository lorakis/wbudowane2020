#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/sleep.h>

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PD2
#define BTN_PIN PIND
#define BTN_PORT PORTD

void timer_init()
{
	// prescaler 1024 - 16000000Hz/1024 = 15625Hz
	TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);
  OCR2B = 155;
	TIMSK2 |= _BV(TOIE2);
}


bool tab[102];
int i = 0;
ISR(TIMER2_OVF_vect)
{

  if( tab[(i+1) % 101] )
  {
      LED_PORT &= ~_BV(LED);

  }
  else
  {
    LED_PORT |= _BV(LED);
    tab[i]=false;
  }

	if (BTN_PIN & _BV(BTN))
		tab[i] = true;
	else
		tab[i] = false;
  i++;
	if( i >= 101 )
  {
    i = 0;
  }
}

int main() {
  BTN_PORT |= _BV(BTN);
  LED_DDR |= _BV(LED);
	timer_init();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	sei();
	while (1)
  {
		sleep_mode();
	}
}
