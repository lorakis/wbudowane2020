#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem
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
  // ustaw tryb licznika
  // COM1A = 10   -- non-inverting mode
  // WGM1  = 1110 -- fast PWM top=ICR1
  // CS1   = 101  -- prescaler 1024
  // ICR1  = 15624
  // częstotliwość 16e6/(1024*(9+1)) = 1562 Hz
  // 16 -> 1kHz
  // wzór: datasheet 20.12.3 str. 164
  ICR1 = 16;
  TCCR1A = _BV(COM1A1);
  TCCR1B = _BV(WGM13) | _BV(CS10) | _BV(CS12);;
  TIMSK1 = _BV(ICIE1) | _BV(TOIE1); // odpala przerwania
  // ustaw pin OC1A (PB1) jako wyjście
  DDRB |= _BV(PB1);
}

// inicjalizacja ADC
void adc_init()
{
  ADMUX   = _BV(REFS0); // referencja AVcc, wejście ADC0
  // DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADIE); // ADC interrupt enable
  ADCSRA |= _BV(ADEN); // włącz ADC
}
uint16_t capt, ovf, typ;

ISR(TIMER1_CAPT_vect) // tranzystro zamknięty
{
  typ = 1;
  ADMUX |= _BV(MUX0); // przełączamy na ADC1
  ADCSRA |= _BV(ADSC);
}

ISR(TIMER1_OVF_vect) // tranzystor otwarty
{
  typ = 2;
  ADMUX |= _BV(MUX0); // przełączamy na ADC1
  ADCSRA |= _BV(ADSC); // odpalamy konwersję, po konwersji odpala sie przerwanie adc
}

ISR(ADC_vect)
{
  if(typ == 1)
  {
    ovf = ADC;
    typ = 3;
    ADMUX &= ~_BV(MUX0); // przełączamy na ADC0
    ADCSRA |= _BV(ADSC);
  } else if(typ == 2)
  {
    capt = ADC;
    typ = 3;
    ADMUX &= ~_BV(MUX0);
    ADCSRA |= _BV(ADSC);
  } else if(typ == 3)
  {
    OCR1A = ADC / 128;
  }
}


FILE uart_file;

int main()
{
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  adc_init();
	timer_init();
	sei();

  while(1)
  {
    printf("ovf: %"PRIu16"mV     capt: %"PRIu16"mV\r\n", ovf * (5000 / 1024), capt * (5000 / 1024));
  }
}
