#include <avr/io.h>


void timer1_init()
{
 TCCR1A=(_BV(COM1A1) | _BV(COM1B1));
 OCR1A=64;
 OCR1B=1024;
 TCCR1A|=_BV(WGM10);
 TCCR1B=_BV(WGM12) | _BV(CS10); //Preskaler 1, FastPWM 8-bitowy
 DDRB |=(_BV(PB1) | _BV(PB2));
}

int main()
{
  // uruchom licznik
  timer1_init();
  // ustaw wypełnienie 50%

  while(1);

}
