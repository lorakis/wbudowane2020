/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "mpu_wrappers.h"
#include <avr/io.h>
#include <avr/interrupt.h>


#include <stdio.h>
#include "uart.h"

/******************************************************************************
 * Private macro definitions.
 ******************************************************************************/


#define vADC_TASK_PRIORITY 1

/******************************************************************************
 * Private function prototypes.
 ******************************************************************************/

static void vADC(void* pvParameters);


/******************************************************************************
 * Public function definitions.
 ******************************************************************************/

SemaphoreHandle_t mutex_r, mutex_a;
uint8_t i = 0;

void adc_init()
{
  ADMUX = _BV(REFS0); // ref wyjscia na ADC0
  DIDR0 = _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D); // włączamy A0 A1 A2
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADIE); // ADC interrupt enable
  ADCSRA |= _BV(ADEN); // włącz ADC
}

uint16_t readADC(uint8_t mux)
{
  ADMUX = _BV(REFS0) | mux;
  ADCSRA |= _BV(ADSC);
  xSemaphoreTake(mutex_a, portMAX_DELAY); // blokada na mutex a
  uint16_t w = ADC;
  return w;
}

ISR(ADC_vect)
{
  xSemaphoreGiveFromISR(mutex_a, NULL); // odblokuj mutex a
}

/**************************************************************************//**
 * \fn int main(void)
 *
 * \brief Main function.
 *
 * \return
 ******************************************************************************/
int main(void)
{
  adc_init();
  sei();
  // Create task.
  xTaskHandle adc0_handle;
  xTaskHandle adc1_handle;
  xTaskHandle adc2_handle;
  mutex_a = xSemaphoreCreateMutex();
  mutex_r = xSemaphoreCreateMutex();
  xSemaphoreTake(mutex_a, portMAX_DELAY);

    xTaskCreate
        (
         vADC,
         "adc0",
         configMINIMAL_STACK_SIZE,
         0,
         vADC_TASK_PRIORITY,
         &adc0_handle
        );

    xTaskCreate
        (
         vADC,
         "adc1",
         configMINIMAL_STACK_SIZE,
         1,
         vADC_TASK_PRIORITY,
         &adc1_handle
        );

    xTaskCreate
        (
         vADC,
         "adc2",
         configMINIMAL_STACK_SIZE,
         2,
         vADC_TASK_PRIORITY,
         &adc2_handle
        );

    // Start scheduler.
    vTaskStartScheduler();

    return 0;
}

/**************************************************************************//**
 * \fn static vApplicationIdleHook(void)
 *
 * \brief
 ******************************************************************************/
void vApplicationIdleHook(void)
{

}

/******************************************************************************
 * Private function definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn static void vSerial(void* pvParameters)
 *
 * \brief
 *
 * \param[in]   pvParameters
 ******************************************************************************/
static void vADC(void* pvParameters)
{
  uart_init();
  stdin = stdout = stderr = &uart_file;
  uint8_t port = (uint8_t)pvParameters;
  uint16_t value;
  char result[5];
  while (1)
  {
    xSemaphoreTake(mutex_r, portMAX_DELAY); // blokada mutexu r
    value = readADC(port);
    putchar('A');
    putchar('0'+port);
    utoa(value, result, 10);
    putchar(':');
    puts(result);
    puts("\r");
    xSemaphoreGive(mutex_r); // zwolnienie r
    vTaskDelay(((port*100)+3000) / portTICK_PERIOD_MS);
  }

}
