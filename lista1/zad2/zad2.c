#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

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

void kropka()
{
  LED_PORT |= _BV(LED); // on
  _delay_ms(500);
  LED_PORT &= ~_BV(LED); // off
  _delay_ms(200);

}

void kreska()
{
  LED_PORT |= _BV(LED); // on
  _delay_ms(1500);
  LED_PORT &= ~_BV(LED); // off
  _delay_ms(200);

}

char* tab[ 26 ] = {
"* - ",
"- * * *",
"- * - *",
"- * *" ,
"*",
"* * - *",
"- - *",
"* * * *",
"* *",
"* - - -" ,
"- * -" ,
"* - * *",
"- -",
"- *",
"- - -",
"* - - *",
"- - * -",
"* - *",
"* * *",
"-",
"* * -",
"* * * -",
"* - -",
"- * * -" ,
"- * - -",
"- - * *"
};


void literaNaMorsea(char a)
{
  for(int i = 0; i < strlen(tab[a-97]);i++)
  {
    if(tab[a-97][i] == '*')
      kropka();
    else if(tab[a-97][i] == '-')
      kreska();
  }
}


int main() {
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  while(1)
  {
    printf("Wpisz napis:\r\n" );
    char napis[30];
    scanf("%s", napis);
    printf("%s\r\n", napis );
    int dl = strlen(napis);
    for(int i = 0; i < dl; i++)
    {
      literaNaMorsea(napis[i]);
      _delay_ms(500);
    }
  }
}
