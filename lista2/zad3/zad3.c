#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

#define LED_DDR DDRD
#define LED_PORT PORTD

#define BTN1 PB4
#define BTN2 PB3
#define BTN3 PB2
#define BTN_PIN PINB
#define BTN_PORT PORTB

#define DEBOUNCE_TIME 1000

uint8_t debounce1(void)
{
  if (bit_is_clear(BTN_PIN, BTN1))
  { /* button is pressed now */
    _delay_us(DEBOUNCE_TIME);
  if (bit_is_clear(BTN_PIN, BTN1))
  { /* still pressed */
    return (1);
  }
}
 return (0);
}
uint8_t debounce2(void)
{
  if (bit_is_clear(BTN_PIN, BTN2))
  { /* button is pressed now */
    _delay_us(DEBOUNCE_TIME);
  if (bit_is_clear(BTN_PIN, BTN2))
  { /* still pressed */
    return (1);
  }
}
 return (0);
}
uint8_t debounce3(void)
{
  if (bit_is_clear(BTN_PIN, BTN3))
  { /* button is pressed now */
    _delay_us(DEBOUNCE_TIME);
    if (bit_is_clear(BTN_PIN, BTN3))
    { /* still pressed */
      return (1);
    }
  }
 return (0);
}

int main()
{
  UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
  BTN_PORT |= 0b11111111;
  LED_DDR |= 0b11111111;
  LED_PORT |= 0b00000000;
  uint8_t i = 0b00000000;
  uint8_t kod = 0b00000000;
  uint8_t button1WasPressed = 0;
  uint8_t button2WasPressed = 0;
  uint8_t button3WasPressed = 0;
  while(1)
  {
    if(debounce1())
    { /* debounced button press */
      if(button1WasPressed == 0)
      { /* but wasn't last time through */
        kod = 0b00000000; /* do whatever */
        i = 0b00000000;
        LED_PORT = kod;
       button1WasPressed = 1; /* update the state */
      }
    }
    else
    { /* button is not pressed now */
      button1WasPressed = 0; /* update the state */
    }
    if(debounce2())
    { /* debounced button press */
      if(button2WasPressed == 0)
      { /* but wasn't last time through */
        i++;
        i %= 256;
        kod = i ^ (i >> 1);/* do whatever */
        LED_PORT = kod;
        button2WasPressed = 1; /* update the state */
      }
    }
    else
    { /* button is not pressed now */
      button2WasPressed = 0; /* update the state */
    }
    if(debounce3())
    { /* debounced button press */
      if(button3WasPressed == 0)
      { /* but wasn't last time through */
        i--;
        i %= 256;
        kod = i ^ (i >> 1);/* do whatever */
        LED_PORT = kod;
        button3WasPressed = 1; /* update the state */
      }
    }
    else
    { /* button is not pressed now */
      button3WasPressed = 0; /* update the state */
    }
  }

}
