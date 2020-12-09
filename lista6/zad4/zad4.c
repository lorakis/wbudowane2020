#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

void spi_init(){
    // ustaw piny MOSI, SCK i ~SS jako wyjścia
    DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2);
    // włącz SPI w trybie master z zegarem 250 kHz
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}


// transfer jednego bajtu
void spi_transfer(uint8_t data){
    // rozpocznij transmisję
    SPDR = data;
    // czekaj na ukończenie transmisji
    while (!(SPSR & _BV(SPIF)));
    // wyczyść flagę przerwania
    SPSR |= _BV(SPIF);
}

int main() {
  spi_init();
  uint8_t tab[10];


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
      PORTB |= _BV(PB1);
      spi_transfer(tab[i]);
      PORTB &= ~_BV(PB1);
      _delay_ms(1000);
    }
  }

}
