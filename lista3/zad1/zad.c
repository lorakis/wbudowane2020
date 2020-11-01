#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>


#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

#define BUZZ PB5
#define BUZZ_DDR DDRB
#define BUZZ_PORT PORTB

#define F 1400
#define G 1275
#define A 1160
#define B 1100
#define H 1050
#define c 955   // done
#define cis 920
#define d 852 // done
#define e 758   // done
#define f 716 // done
#define fis 690
#define g 638 // done
#define a 568 // done
#define b 530
#define h 506 // done
#define c2 478  // done
#define d2 426  // done
#define e2 380
#define f2 327
#define fis2 300
#define g2 280
#define x 30000

#define n1 1600
#define n2 800
#define n4 400
#define n6 300
#define n8 200
#define n16 100
#define TONE(step, delay) \
    if(step == 30000) { _delay_ms(delay); } \
    else { \
      for (uint16_t i = 0; i < (uint32_t)1000 * (delay) / (step) / 2; i++) { \
        BUZZ_PORT |= _BV(BUZZ); \
        _delay_us(step); \
        BUZZ_PORT &= ~_BV(BUZZ); \
        _delay_us(step); \
      } \
    } \

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

static const uint16_t darude[] PROGMEM = {
  h, h, h, h, h, x, //6
  h, h, h, h, h, h, h, //7
  e2, e2, e2, e2, e2, e2, e2, //7
  d2, d2, d2, d2, d2, d2, d2, //7
  a, //1
  h, h, h, h, h, h, x, //7
  h, h, h, h, h, h, x, //7
  h, e2,
  h, h, h, h, h, x, //6
  h, h, h, h, h, h, h, d2 //8
 //58
};



static const int darude_tempo[] PROGMEM = {
  n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8,
  n4,
  n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8,
  n8, n4,
  n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n4
};

static const uint16_t astro[] PROGMEM = {
  G, x, G, d, c, x, B, x,
  A, x, A, A, c, x, B, A,
  G, x, G, b, a, b, a, b,
  G, x, G, b, a, b, a, b,
  G, x, G, d, c, x, B, x,
  A, x, A, A, c, x, B, A,
  G, x, G, b, a, b, a, b,
  G, x, G, b, a, b, a, b,

  G, x, G, d, c, x, B, x,
  A, x, A, A, c, x, B, A,
  G, x, G, b, a, b, a, b,
  G, x, G, b, a, b, a, b,
  B, B, B, B, d, d, d, d,
  c, c, c, c, f, f, f, f,
  g, g, g, g, g, g, g, g,
  g, g, g, g, c, B, A, F

};



static const int astro_tempo[] PROGMEM = {
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,

  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n8, n8
};

static const uint16_t spooky[] PROGMEM = {
  g, g, fis, fis, H, d, H, H, //8
  g, g, fis, fis, H, x, x, //7
  g, g, fis, fis, H, d, H, //7
  d, e, cis, d, H, x, x, //7
  g2, g2, fis2, fis2, h, d2, h, x, //8
  g, g, fis, fis, H, x, x, fis, //8
  g2, g2, fis2, fis2, h, d2, h, H, cis, //9
  d, e, cis, d, H, x, x //7

};

static const uint16_t spooky_tempo[] PROGMEM = {
  n8, n8, n8, n8, n8, n16, n6, n8,
  n16, n6, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n8, n4,
  n8, n8, n8, n8, n8, n8,
  n8, n8, n8, n8, n8, n16, n6, n8,
  n8, n8, n8, n8, n8, n8, n8, n8,
  n8, n16, n6, n8, n16, n6, n16, n16,
  n8, n8, n8, n8, n8, n8, n8
};
int main() {
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  BUZZ_DDR |= _BV(BUZZ);

  while (1) {
    //TONE(g,1000);

    //TONE(pgm_read_word(&utwor[2]),pgm_read_word(&tempo[2]));

    for(int i = 0; i < 128; i++)
    {
      //TONE(pgm_read_byte(&utwor[i]),pgm_read_byte(&tempo[i]));

      int wartosc = pgm_read_word(&astro[i]);
      int dlugosc = pgm_read_word(&astro_tempo[i]);
      TONE(wartosc,dlugosc);

      _delay_ms(10);
    }


  }
}
