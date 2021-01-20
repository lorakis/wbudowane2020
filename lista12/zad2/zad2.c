#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <util/delay.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

#define pom 50

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
  ADMUX   = _BV(REFS0); // referencja AVcc, wejście ADC0
  ADMUX |= _BV(MUX0);
  DIDR0   = _BV(ADC1D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADEN); // włącz ADC
}


float tab[pom+2];

float zmierz()
{
  ADCSRA |= _BV(ADSC); // wykonaj konwersję
  while (!(ADCSRA & _BV(ADIF))); // czekaj na wynik
  ADCSRA |= _BV(ADIF); // wyczyść bit ADIF (pisząc 1!)
  return 5.0 * ADC / 1024.0;
}
float kwadratowa()
{
  float sum = 0;
  for(int i = 0; i < pom; i++)
  {
    sum += tab[i] * tab[i] / pom;
  }
  return sqrt(sum);
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

  while(1)
  {

    for(int i = 0; i < pom; i++)
    {
      tab[i] = zmierz() - 2.5;
    }
    float x = kwadratowa();
    float xref = 2.5;
    float L = 20 * log10(x/xref);

    for(int i = 0; i < 40 + (int)L ; i++)
      printf("#");
    for(int i = 0; i < -(int)L ; i++)
        printf(" ");
    printf("  %.2f\r\n", L);
    //printf("L = %f\r\n", L);
  }

}
