
#include <hw/stm32f3xx/uart_shell.h>

namespace hw {
namespace stm32f3xx {

Uart::Uart(uint8_t uartIndex, uint32_t baudrate,
           GPIO_TypeDef* txPort, uint16_t txPin,
           GPIO_TypeDef* rxPort, uint16_t rxPin) {


  uint32_t af;
  USART_TypeDef* instance;

  switch (uartIndex) {
  case 1:
    __HAL_RCC_USART1_CLK_ENABLE();
    instance = USART1;
    af = GPIO_AF7_USART1;
    break;
  case 2:
    __HAL_RCC_USART2_CLK_ENABLE();
    instance = USART2;
    af = GPIO_AF7_USART2;
    break;
  case 3:
    __HAL_RCC_USART3_CLK_ENABLE();
    instance = USART2;
    af = GPIO_AF7_USART3;
    break;
  }

  // GPIO Pins
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = af;

  GPIO_InitStruct.Pin = txPin;
  HAL_GPIO_Init(txPort, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = rxPin;
  HAL_GPIO_Init(rxPort, &GPIO_InitStruct);

  _uart.Instance = instance;
  _uart.Init.BaudRate = baudrate;
  _uart.Init.WordLength = UART_WORDLENGTH_8B;
  _uart.Init.StopBits = UART_STOPBITS_1;
  _uart.Init.Parity = UART_PARITY_NONE;
  _uart.Init.Mode = UART_MODE_TX_RX;
  _uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  _uart.Init.OverSampling = UART_OVERSAMPLING_16;
  _uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  _uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  HAL_NVIC_EnableIRQ(USART3_IRQn);

  if (HAL_UART_Init(&_uart) != HAL_OK)
  {
    // TODO: error handle
  }
}

}
}

