#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>


#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB


#define c 955   // done
#define d 852 // done
#define e 758   // done
#define f 716 // done
#define g 638 // done
#define a 568 // done
#define h 506 // done
#define c2 478  // done
#define d2 426  // done
#define e2 380
#define x 40000

#define n1 1600
#define n2 800
#define n4 400
#define n8 200
#define TONE(step, delay) \
    for (uint16_t i = 0; i < (uint32_t)1000 * (delay) / (step) / 2; i++) { \
      BUZZ_PORT |= _BV(BUZZ); \
      _delay_us(step); \
      BUZZ_PORT &= ~_BV(BUZZ); \
      _delay_us(step); \
    }

void delay(int time)
{
  for(int i = 0; i < time; i++)
    _delay_us(1);
}

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

static const uint16_t utwor[] PROGMEM = {
  x, g,
  e, g, e2, e2,
  c2, c2, g,
  e, g, c2, c2,
  h, x, h,
  a, a, h, h,
  a, a, a,
  g, g, a, a,
  g, x, g,
  a, h,
  c2, d2,
  e2,
  c2, x, e2,
  d2, d2,
  a, h,
  c2, c2
};

static const int tempo[] PROGMEM = {
  n2, n4,
  n4, n4, n4, n4,
  n4, n2, n4,
  n4, n4, n4, n4,
  n2, n4, n4,
  n4, n4, n4, n4,
  n4, n2, n4,
  n4, n4, n4, n4,
  n2, n4, n4,
  n2, n2,
  n2, n2,
  n1,
  n2, n4, n4,
  n2, n2,
  n2, n2,
  n1, n4
};


int main() {
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  BUZZ_DDR |= _BV(BUZZ);

  while (1) {
    //TONE(e2,1000);

    //TONE(pgm_read_word(&utwor[2]),pgm_read_word(&tempo[2]));

    for(int i = 0; i < 44; i++)
    {
      //TONE(pgm_read_byte(&utwor[i]),pgm_read_byte(&tempo[i]));

      int wartosc = pgm_read_word(&utwor[i]);
      int dlugosc = pgm_read_word(&tempo[i]);
      TONE(wartosc,dlugosc);

      _delay_ms(10);
    }


  }
}
