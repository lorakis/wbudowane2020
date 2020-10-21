#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PB4
#define BTN_PIN PINB
#define BTN_PORT PORTB


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

char dek[32];

void wypelnij_dek()
{
    dek[2] = 't';
    dek[3] = 'e';
    dek[7] = 'i';
    dek[6] = 'a';
    dek[4] = 'm';
    dek[5] = 'n';
    dek[8] = 'o';
    dek[9] = 'g';
    dek[10] = 'k';
    dek[11] = 'd';
    dek[12] = 'w';
    dek[13] = 'r';
    dek[14] = 'u';
    dek[15] = 's';
    dek[18] = 'q';
    dek[19] = 'z';
    dek[20] = 'y';
    dek[21] = 'c';
    dek[22] = 'x';
    dek[23] = 'b';
    dek[24] = 'j';
    dek[25] = 'p';
    dek[26] = '!';
    dek[27] = 'l';
    dek[28] = '!';
    dek[29] = 'f';
    dek[30] = 'v';
    dek[31] = 'h';
}

int main()
{
  // zainicjalizuj UART
 uart_init();
 // skonfiguruj strumienie wejścia/wyjścia
 fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
 stdin = stdout = stderr = &uart_file;

  BTN_PORT |= _BV(BTN);
  LED_DDR |= _BV(LED);
  wypelnij_dek();

  int time_on = 0;
  int time_off = 61;
  int lit = 1;
  while(1)
  {

    if (BTN_PIN & _BV(BTN))
    {
      time_on = 0;
      time_off++;
      LED_PORT &= ~_BV(LED); //off
    }
    else
    {
      //LED_PORT &= ~_BV(LED);//off
      time_on++;
      time_off = 0;
    }


    if( time_on > 0)
    {
      if( time_on == 1)
      {
        lit = lit << 1;
        lit += 1;
      }
      if( time_on == 60) // 1,2 na kreskę
      {
        LED_PORT |= _BV(LED); //on
        lit = lit - 1;

      }

    }


    if( time_off == 60)
    {
      LED_PORT |= _BV(LED); //on

      //printf("%d", lit);
      if(lit > 31)
        printf("!");
      printf("%c", dek[lit]);
      lit = 1;
    }

    _delay_ms(20);

  }



}
