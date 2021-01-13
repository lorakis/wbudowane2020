#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "pid.h"

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

//! \xrefitem todo "Todo" "Todo list"
#define K_P     2
//! \xrefitem todo "Todo" "Todo list"
#define K_I     1.5*0.5 //66*0.5
//! \xrefitem todo "Todo" "Todo list"
#define K_D     1.5*0.12 //66 *0.12
struct PID_DATA pidData;

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
  ICR1 = 255;
  TCCR1A = _BV(COM1A1);
  TCCR1B = _BV(WGM13) | _BV(CS10) | _BV(CS12);
  TIMSK1 = _BV(ICIE1) | _BV(TOIE1); // odpala przerwania
  // ustaw pin OC1A (PB1) jako wyjście

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
  DIDR0 = _BV(ADC0D) | _BV(ADC1D);
}
uint16_t capt, ovf, typ, potenc, xd = 1;
int16_t pid_val = 0;

ISR(TIMER1_CAPT_vect) // tranzystro zamknięty
{
  typ = 2;
  ADMUX |= _BV(MUX0); // przełączamy na ADC1
  ADCSRA |= _BV(ADSC);
}

ISR(TIMER1_OVF_vect) // tranzystor otwarty
{
  typ = 1;
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
    potenc = ADC;
    xd = 1;
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
  pid_Init(K_P * SCALING_FACTOR, K_I * SCALING_FACTOR , K_D * SCALING_FACTOR , &pidData);

  DDRB |= _BV(PB1);
  while(1)
  {
    printf("pid_val: %"PRIu16"  potenc: %"PRIu16"   ovf: %"PRIu16"mV capt: %"PRIu16"mV\r\n",pid_val, potenc, ovf * (5000 / 1024), capt * (5000 / 1024));
    if(xd == 1)
    {
      pid_val = pid_Controller(capt, potenc, &pidData);
      pid_val /= 128;
      if(pid_val < 0)
        pid_val = 0;
      OCR1A = pid_val;
      xd = 0;
    }
  }
}
