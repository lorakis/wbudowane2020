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
  // częstotliwość 16e6/(1024*(1+31)) = 500 Hz
  // 16 -> 1kHz
  // wzór: datasheet 20.12.3 str. 164
  ICR1 = 10230;
  TCCR1A = _BV(COM1A1);
  TCCR1B = _BV(WGM13) | _BV(CS10);
  TIMSK1 = _BV(ICIE1) | _BV(TOIE1);
  // ustaw pin OC1A (PB1) jako wyjście
  DDRB |= _BV(PB1);
}

// inicjalizacja ADC
void adc_init()
{
  ADMUX   = _BV(REFS0); // referencja AVcc, wejście ADC0
  DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADIE); // ADC interrupt enable
  ADCSRA |= _BV(ADEN); // włącz ADC
}
int8_t interrupt_type = 0;
uint16_t ovf_val, capt_val;

ISR(TIMER1_OVF_vect)
{
	ADMUX |= _BV(MUX0);
	ADCSRA |= _BV(ADSC);
	interrupt_type = 1;
}

ISR(TIMER1_CAPT_vect)
{
	interrupt_type = 2;
	ADMUX |= _BV(MUX0);
	ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect)
{
	if (interrupt_type == 1) {
		ovf_val = ADC;
		interrupt_type = 3;
		ADMUX &= ~_BV(MUX0);
		ADCSRA |= _BV(ADSC);
	} else if (interrupt_type == 2) {
		capt_val = ADC;
		interrupt_type = 3;
		ADMUX &= ~_BV(MUX0);
		ADCSRA |= _BV(ADSC);
	} else if (interrupt_type == 3) {
		OCR1A = ADC * 10;
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
		printf("Zmierzono ovf: %"PRIu16"mv | capt: %"PRIu16"mv\r\n", ovf_val * (5000 / 1024), capt_val * (5000 / 1024));
	}
}
