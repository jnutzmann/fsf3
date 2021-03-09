
#include "hw/stm32f3xx/uart_shell.h"

#include "freertos/ticks.hpp"
#include "freertos/src/CMSIS_RTOS_V2/cmsis_os.h"

namespace hw {
namespace stm32f3xx {

UartShell::UartShell(const Config & config)
            : freertos::Thread("dbg", 128*3, osPriorityBelowNormal),
              _config(config),
			  _txQueue(config.txQueueLength, sizeof(char)),
			  _rxQueue(config.rxQueueLength, sizeof(char)) {
  Start();
}

void UartShell::InitHardware() {

  uint32_t af;
  USART_TypeDef* instance;

  switch (_config.uartIndex) {
  case 1:
    __HAL_RCC_USART1_CLK_ENABLE();
    instance = USART1;
    af = GPIO_AF7_USART1;
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 5);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    break;
  case 2:
    __HAL_RCC_USART2_CLK_ENABLE();
    instance = USART2;
    af = GPIO_AF7_USART2;
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 5);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    break;
  case 3:
    __HAL_RCC_USART3_CLK_ENABLE();
    instance = USART3;
    af = GPIO_AF7_USART3;
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 5);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    break;
  }

  // GPIO Pins
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = af;

  GPIO_InitStruct.Pin = _config.txPin;
  HAL_GPIO_Init(_config.txPort, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = _config.rxPin;
  HAL_GPIO_Init(_config.rxPort, &GPIO_InitStruct);

  // UART Peripheral
  _uart.Instance = instance;
  _uart.Init.BaudRate = _config.baudrate;
  _uart.Init.WordLength = UART_WORDLENGTH_8B;
  _uart.Init.StopBits = UART_STOPBITS_1;
  _uart.Init.Parity = UART_PARITY_NONE;
  _uart.Init.Mode = UART_MODE_TX_RX;
  _uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  _uart.Init.OverSampling = UART_OVERSAMPLING_16;
  _uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  _uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  configASSERT(HAL_UART_Init(&_uart) != HAL_OK);
}

void UartShell::Run() {

  InitHardware();

  while(true) {
    PutString("Hello World");
    Delay(2000);
  }
}

inline bool UartShell::PutChar(char c) {
  return _txQueue.Enqueue(&c);
}

bool UartShell::PutString(const char* s) {
  while (*s) {
    if (!PutChar(*s)) return false;
    s++;
  }

  StartSend();
  return true;
}

void UartShell::StartSend(void) {
  if (!_txQueue.IsEmpty()) {
    SET_BIT(_uart.Instance->CR1, USART_CR1_TXEIE);
  }
}

void UartShell::ServiceInterrupt(void) {
  uint32_t isrflags = READ_REG(_uart.Instance->ISR);
  uint32_t cr1its   = READ_REG(_uart.Instance->CR1);

  BaseType_t higherPriorityTaskWoken = pdFALSE;
  char c;

  /* UART in mode Receiver ---------------------------------------------------*/
  if (((isrflags & USART_ISR_RXNE) != 0U) && ((cr1its & USART_CR1_RXNEIE) != 0U)) {

    // reading the data register clears the interrupt flag.
    c = (char)_uart.Instance->RDR;

    // TODO: put data back onto transmit

    _rxQueue.EnqueueFromISR(&c, &higherPriorityTaskWoken);
  }

  /* UART in mode Transmitter ------------------------------------------------*/
  if (((isrflags & USART_ISR_TXE) != 0U) && ((cr1its & USART_CR1_TXEIE) != 0U)) {

    if (_txQueue.DequeueFromISR(&c, &higherPriorityTaskWoken)) {
    _uart.Instance->TDR = c;
    } else {
      // Disable the interrupt if there is no more data.
      CLEAR_BIT(_uart.Instance->CR1, USART_CR1_TXEIE);
    }
  }

  if(higherPriorityTaskWoken) {
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
  }
}

}
}

