
#include "hw/stm32f3xx/uart_shell.h"

#include "freertos/ticks.hpp"
#include "freertos/src/CMSIS_RTOS_V2/cmsis_os.h"

#include "shell/cli.h"

#include <stdio.h>

namespace hw {
namespace stm32f3xx {

UartShell::UartShell(const Config & config)
            : freertos::Thread("SHELL", 128*3, osPriorityBelowNormal),
              _config(config),
			  _txQueue(config.txQueueLength, sizeof(char)),
			  _rxCmdQueue(config.rxQueueLength, CMD_BUFFER_LENGTH),
			  _rxBufferIndex(0) {
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

  configASSERT(HAL_UART_Init(&_uart) == HAL_OK);

  // Enable the receive interrupt
  SET_BIT(_uart.Instance->CR1, USART_CR1_RXNEIE);
}

class StatsCommand : public shell::Command {
  public:
    StatsCommand() :
      Command("stats", "display stack usage") {}

    void Callback(char* commandInput) {
      char outputBuff[160];
      vTaskList(outputBuff);
      printf("%s\r\n", outputBuff);
    }
};
static StatsCommand statsCmd;


void UartShell::Run() {

  InitHardware();

  shell::commandInterperterInstance.Register((shell::Command*) &statsCmd);

  printf("\r\r-----\r\nHello!  Welcome to the Sunglasses CLI!\r\n> ");

  char cmdBuf[CMD_BUFFER_LENGTH];

  while(true) {
    _rxCmdQueue.Dequeue(cmdBuf);

    if (cmdBuf[0]) {
      printf("\r\n\n");
      shell::commandInterperterInstance.Interpert(cmdBuf);
    }

    printf("\r\n> ");
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

int UartShell::Write(int file, char *ptr, int len) {
  for (int i=0; i < len; i++) {
    if (!_txQueue.Enqueue(ptr+i)) return -1;
  }
  StartSend();
  return len;
}

void UartShell::StartSend(void) {
  if (!_txQueue.IsEmpty()) {
    SET_BIT(_uart.Instance->CR1, USART_CR1_TXEIE);
  }
}

void UartShell::ServiceInterrupt(void) {
  uint32_t isrflags = READ_REG(_uart.Instance->ISR);

  BaseType_t higherPriorityTaskWoken = pdFALSE;
  char c;

  if (isrflags & USART_ISR_RXNE) {

    // reading the data register clears the interrupt flag.
    c = (char)_uart.Instance->RDR;

    if (c == '\r' || _rxBufferIndex >= MAX_CMD_LENGTH) {
      _rxBuffer[_rxBufferIndex] = 0; // Terminate the string.

      _rxBufferIndex = 0;            // Reset the buffer

      // Enqueue for future processing.
      _rxCmdQueue.EnqueueFromISR((char*)_rxBuffer, &higherPriorityTaskWoken);

    } else if (c == 127) {
      if (_rxBufferIndex > 0) {
        // backspace
        c = '\b'; _txQueue.EnqueueFromISR(&c, &higherPriorityTaskWoken);
        c = ' ';  _txQueue.EnqueueFromISR(&c, &higherPriorityTaskWoken);
        c = '\b'; _txQueue.EnqueueFromISR(&c, &higherPriorityTaskWoken);

        SET_BIT(_uart.Instance->CR1, USART_CR1_TXEIE);

        _rxBufferIndex--;
      } // else do nothing, as there is nothing to delete
    } else {
        // Repeat the character back
        _txQueue.EnqueueFromISR(&c, &higherPriorityTaskWoken);
        SET_BIT(_uart.Instance->CR1, USART_CR1_TXEIE);

        _rxBuffer[_rxBufferIndex++] = c;
    }
  }

  if (isrflags & USART_ISR_TXE) {

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

