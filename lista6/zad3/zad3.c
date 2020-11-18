#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "nuty.h"

#define SOUND_LENGTH 8736
// inicjalizacja SPI
void spi_init()
{
    // ustaw piny MOSI, SCK i ~SS jako wyjścia
    DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2);
    // włącz SPI w trybie master z zegarem 250 kHz
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}

// transfer jednego bajtu
uint8_t spi_transfer(uint8_t data)
{
    // rozpocznij transmisję
    SPDR = data;
    // czekaj na ukończenie transmisji
    while (!(SPSR & _BV(SPIF)));
    // wyczyść flagę przerwania
    SPSR |= _BV(SPIF);
    // zwróć otrzymane dane
    return SPDR;
}

void timer_init()
{
	// prescaler 64
	TCCR2B = _BV(CS22);
}



int main()
{
  timer_init();
  spi_init();

  while(1)
  {
    for(int i = 0; i < SOUND_LENGTH; i++)
    {
      while(TCNT2 < 32); //32*64 = 2048, 16mhz/2048 = 8khz
      int dzwiek = pgm_read_byte(&nuty[i]);

      PORTB &= ~_BV(PB2);
      spi_transfer(0x30 | (dzwiek >> 4)); // BUF = 0, GA = 1, SHDN = 1 i D7-4 = dzwiek >> 4
      spi_transfer(dzwiek << 4); // D3-0 dzwiek << 4
      PORTB |= _BV(PB2);
      TCNT2 = 0;
    }
  }
}
