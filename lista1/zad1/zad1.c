#include<avr/io.h>
#include<stdio.h>
#include<inttypes.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
  // wyczyść rejestr UCSR0A
  UCSR0A = 0;
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

FILE uart_file;

int main()
{
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;

  int8_t a1, a2;
  int16_t b1, b2;
  int32_t c1, c2;
  //int64_t d1, d2;
  //float e1, e2;
  scanf("%"SCNd8, &a1);
  scanf("%"SCNd8, &a2);
  scanf("%"SCNd16, &b1);
  scanf("%"SCNd16, &b2);
  scanf("%"SCNd32, &c1);
  scanf("%"SCNd32, &c2);
  //scanf("%"SCNu64, &d1);
  //scanf("%"SCNu64, &d2);
  //scanf("%f", &e1);
  //scanf("%f", &e2);


  printf("Dodawanie int8_t: %"PRId8"\r\n", a1+a2);
  printf("Odejmowanie int8_t: %"PRId8"\r\n", a1-a2);
  printf("Mnozenie int8_t: %"PRId8"\r\n", a1*a2);
  printf("Dzielenie int8_t: %"PRId8"\r\n", a1/a2);

  printf("Dodawanie int16_t: %"PRId16"\r\n", b1+b2);
  printf("Odejmowanie int16_t: %"PRId16"\r\n", b1-b2);
  printf("Mnozenie int16_t: %"PRId16"\r\n", b1*b2);
  printf("Dzielenie int16_t: %"PRId16"\r\n", b1/b2);

  printf("Dodawanie int32_t: %"PRId32"\r\n", c1+c2);
  printf("Odejmowanie int32_t: %"PRId32"\r\n", c1-c2);
  printf("Mnozenie int32_t: %"PRId32"\r\n", c1*c2);
  printf("Dzielenie int32_t: %"PRId32"\r\n", c1/c2);

  //printf("Dodawanie int64_t: %"PRId64"\n", d1+d2);
  //printf("Odejmowanie int64_t: %"PRId64"\n", d1-d2);
  //printf("Mnozenie int64_t: %"PRId64"\n", d1*d2);
  //printf("Dzielenie int64_t: %"PRId64"\n", d1/d2);

  //printf("Dodawanie float: %f \n", e1+e2);
  //printf("Odejmowanie float: %f \n", e1-e2);
  //printf("Mnozenie float: %f \n", e1*e2);
  //printf("Dzielenie float: %f \n", e1/e2);


}
