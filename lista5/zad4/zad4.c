#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem


#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define pom 20

// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
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

void timer_init()
{
	TCCR1B = _BV(CS12);
	TIMSK1 = _BV(ICIE1);
}

bool f = false;

ISR(TIMER1_CAPT_vect)
{
	if (!f)
  {
		TCNT1 = 0;
		f = true;
	}
  else
  {
		f = false;
		printf("%d\r\n", 62500 / ICR1);
	}
}

FILE uart_file;

int main()
{
	PORTB |= _BV(PB0);
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  timer_init();
	set_sleep_mode(SLEEP_MODE_IDLE);
  sei();
	while (1)
  {
		sleep_mode();
	}
}
