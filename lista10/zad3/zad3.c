#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void adc_init()
{
  ADMUX   = _BV(REFS0); // referencja AVcc, wejście ADC0
  DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADEN); // włącz ADC
}

void timer_init()
{
  // ustaw tryb licznika
  // COM1A = 10   -- non-inverting mode
  // WGM1  = 1110 -- fast PWM top=ICR1
  // CS1   = 101  -- prescaler 1024
  // ICR1  = 15624
  // częstotliwość 16e6/(1024*(1+15624)) = 1 Hz
  // wzór: datasheet 20.12.3 str. 164
  ICR1 = 31;
  TCCR1A = _BV(COM1A1) | _BV(WGM11) | _BV(COM1B1);
  TCCR1B = _BV(WGM12) | _BV(WGM13) | _BV(CS10) | _BV(CS12);
  // ustaw pin OC1A (PB1) jako wyjście
  DDRB |= _BV(PB1) | _BV(PB2);
}


int main()
{
  adc_init();
  timer_init();
  while(1)
  {
    uint16_t v_ADC = 0;
    ADCSRA |= _BV(ADSC); // wykonaj konwersję
    while (!(ADCSRA & _BV(ADIF))); // czekaj na wynik
    ADCSRA |= _BV(ADIF); // wyczyść bit ADIF (pisząc 1!)
    v_ADC = ADC;
    if(v_ADC >= 512)
    {
      OCR1A = (v_ADC - 512) /32;
      OCR1B = 0;
    }
    else if(v_ADC < 512)
    {
      OCR1A = 0;
      OCR1B = (512 - v_ADC) /32;
    }

  }
}
