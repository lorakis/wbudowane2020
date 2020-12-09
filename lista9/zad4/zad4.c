#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#define BUZZ PB1
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
#define x 0

#define n1 1600
#define n2 800
#define n4 400
#define n6 300
#define n8 200
#define n16 100

#define TONE(step, delay) \
    for (uint16_t i = 0; i < (uint32_t)1000 * (delay) / (step) / 2; i++) { \
      BUZZ_PORT |= _BV(BUZZ); \
      _delay_us(step); \
      BUZZ_PORT &= ~_BV(BUZZ); \
      _delay_us(step); \
    }

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
  //128
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




int main() {
  BUZZ_DDR |= _BV(BUZZ);

  while (1)
  {
    for(int i = 0; i < 128; i++)
    {
      //TONE(pgm_read_byte(&utwor[i]),pgm_read_byte(&tempo[i]));

      int wartosc = pgm_read_word(&astro[i]);
      int dlugosc = pgm_read_word(&astro_tempo[i]);
      if(wartosc == 0)
        _delay_ms( (dlugosc) );
      else
        TONE(wartosc,dlugosc);

      _delay_ms(10);
    }
  }
}
