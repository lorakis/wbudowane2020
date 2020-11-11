#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem


#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define pom 20

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

// inicjalizacja ADC
void adc_init()
{
  ADMUX   = 0b01001110; // referencja AVcc, wejście ADC0
  // DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADIE); // ADC interrupt enable
  ADCSRA |= _BV(ADEN); // włącz ADC
}

ISR(ADC_vect)
{
	ADCSRA |= _BV(ADIF);
}

FILE uart_file;

uint16_t tab1[50];
uint16_t tab2[50];


uint32_t wariancja(uint16_t *tab)
{
  uint32_t suma = 0;
  uint32_t suma2 = 0;
  for(int i = 0; i < pom; i++)
    suma += tab[i];
  suma /= pom;
  for(int i = 0; i < pom; i++)
  {
    uint32_t s = tab[i] - suma;
    s *= s;
    suma2 += s;
  }
  suma2 /= pom;
  return suma2;
}

int main()
{
  DDRD |= _BV(PB5);
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  // zainicjalizuj ADC

  adc_init();
  sei();
  for(int i = 0; i < pom; i++)
  {
    _delay_ms(1);
    ADCSRA |= _BV(ADSC); // wykonaj konwersję
    while (!(ADCSRA & _BV(ADIF))); // czekaj na wynik
    ADCSRA |= _BV(ADIF); // wyczyść bit ADIF (pisząc 1!)
    uint32_t x = 1125300 / ADC;
    tab1[i] = (uint16_t)x;
  }

  for(int i = 0; i < pom; i++)
  {
    _delay_ms(1);
    sleep_mode();
    uint32_t x = 1125300 / ADC;
    tab2[i] = (uint16_t)x;

  }

  for(int i = 0; i < pom; i++)
  {
    printf("Bez noise reduction %d: %"PRId16"\r\n", i, tab1[i]);
  }
  for(int i = 0; i < pom; i++)
  {
    printf("Z noise reduction %d: %"PRId16"\r\n", i, tab2[i]);
  }
  printf("Wariancja bez noise reduction: %"PRId32"\r\n", wariancja(tab1));
  printf("Wariancja z noise reduction: %"PRId32"\r\n", wariancja(tab2));
  while(1);
}
