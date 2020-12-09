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
#define B 1060 //done
#define H 995 //done
#define c 940 //done
#define cis 885 // done
#define d 835 // done
#define e 743 // done
#define f 700 // done
#define fis 690
#define g 622 // done
#define gis 587 // done
#define a 553 // done
#define b 520 // done
#define h 490 // done
#define c2 462 // done
#define d2 410 // done
#define e2 363 // done
#define f2 343 // done
#define fis2 300
#define g2 303 // done
#define gis2 285 //done
#define a2 269 // done
#define h2 230
#define c3 222 // done
#define d3 197
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
static const uint16_t test[] PROGMEM = {
  d3

};
static const uint16_t test_tempo[] PROGMEM = {
  n1
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

static const uint16_t undertale[] PROGMEM = {
  d, d, d2, a, x, gis, g, x, f, f, d, f, g,
  c, c, d2, a, x, gis, g, x, f, f, d, f, g,
  H, H, d2, a, x, gis, g, x, f, f, d, f, g,
  B, B, d2, a, x, gis, g, x, f, f, d, f, g, //13

  d, d, d2, a, x, gis, g, x, f, f, d, f, g,
  c, c, d2, a, x, gis, g, x, f, f, d, f, g,
  H, H, d2, a, x, gis, g, x, f, f, d, f, g,
  B, B, d2, a, x, gis, g, x, f, f, d, f, g,

  d2, d2, d3, a2, gis2, g2, f2, d2, f2, g2,
  c2, c2, d3, a2, gis2, g2, f2, d2, f2, g2,
  h, h, d3, a2, gis2, g2, f2, d2, f2, g2,
  b, b, d3, a2, gis2, g2, f2, d2, f2, g2,

  d2, d2, d3, a2, gis2, g2, f2, d2, f2, g2,
  c2, c2, d3, a2, gis2, g2, f2, d2, f2, g2,
  h, h, d3, a2, gis2, g2, f2, d2, f2, g2,
  b, b, d3, a2, gis2, g2, f2, d2, f2, g2,

  f2, f2, f2, f2, f2, d2, d2, d2, //8
  f2, f2, f2, g2, gis2, g2, f2, d2, f2, g2, x, //11
  f2, f2, f2, g2,  gis2, a2, c3, a2, //8
  d3, d3, d3, a2, d3, c3, c3, //7
  //218
  f2, f2, f2, f2, f2, d2, d2, d2, //8
  f2, f2, f2, f2, d2, f2, e2, d2, c2, // 9
  x, g2, e2, f2, f2, f2, f2, f, g, //9
  b, c2, d2, f2, c3, c3, x, x //6
  //250
};

static const uint16_t undertale_tempo[] PROGMEM = {
  n16, n16, n8, n8, n16, n16, n16, n16, n16, n16, n16, n16, n16, n16,
  n16, n16, n8, n8, n16, n16, n16, n16, n16, n16, n16, n16, n16, n16,
  n16, n16, n8, n8, n16, n16, n16, n16, n16, n16, n16, n16, n16, n16,
  n16, n16, n8, n8, n16, n16, n16, n16, n16, n16, n16, n16, n16, n16,

  n16, n16, n8, n8, n16, n16, n16, n16, n16, n16, n16, n16, n16, n16,
  n16, n16, n8, n8, n16, n16, n16, n16, n16, n16, n16, n16, n16, n16,
  n16, n16, n8, n8, n16, n16, n16, n16, n16, n16, n16, n16, n16, n16,
  n16, n16, n8, n8, n16, n16, n16, n16, n16, n16, n16, n16, n16, n16,
  // 104
  n16, n16, n8, n8, n8, n8, n8, n16, n16, n16,  //10
  n16, n16, n8, n8, n8, n8, n8, n16, n16, n16,  //10
  n16, n16, n8, n8, n8, n8, n8, n16, n16, n16,  //10
  n16, n16, n8, n8, n8, n8, n8, n16, n16, n16,  //10

  n16, n16, n8, n8, n8, n8, n8, n16, n16, n16,  //10
  n16, n16, n8, n8, n8, n8, n8, n16, n16, n16,  //10
  n16, n16, n8, n8, n8, n8, n8, n16, n16, n16,  //10
  n16, n16, n8, n8, n8, n8, n8, n16, n16, n16,  //10

  n8, n16, n8, n8, n8, n8, n8, n16,
  n8, n16, n8, n8, n8, n16, n16, n16, n16, n16, n8,
  n8, n16, n8, n8, n8, n8, n8, n8,
  n8, n8, n16, n16, n16, n16, n2,

  n8, n16, n8, n8, n8, n8, n8, n16,
  n8, n16, n8, n8, n8, n8, n8, n16, n8,
  n16, n8, n8, n8, n8, n8, n8, n8, n16,
  n8, n16, n8, n8, n16, n2, n1, n1
};

int main() {
  BUZZ_DDR |= _BV(BUZZ);

  while (1)
  {
    for(int i = 0; i < 252; i++)
    {
      int wartosc = pgm_read_word(&undertale[i]);
      int dlugosc = pgm_read_word(&undertale_tempo[i]);
      if(wartosc == 0)
        _delay_ms( (dlugosc)/2 );
      else
        TONE(wartosc,dlugosc);

      _delay_ms(10);
    }
  }
}
