/*
 * uart.h
 *
 *  Created on: Mar 5, 2021
 *      Author: jnutzmann
 */

#ifndef HW_STM32F3XX_UART_SHELL_H_
#define HW_STM32F3XX_UART_SHELL_H_

#include "freertos/thread.hpp"
#include "freertos/queue.hpp"
#include "stm32f3_hal/stm32f3xx_hal.h"

namespace hw {
namespace stm32f3xx {

class UartShell : public freertos::Thread {

  public:

    struct Config {
      uint8_t uartIndex;
      uint32_t baudrate;
      GPIO_TypeDef* txPort;
      uint16_t txPin;
      GPIO_TypeDef* rxPort;
      uint16_t rxPin;
    };

    UartShell(const Config & config);

    void ServiceInterrupt(void);

    bool PutString(const char* s);

  protected:
    virtual void Run(void);

  private:
    const Config & _config;

    UART_HandleTypeDef _uart;
    freertos::Queue* _rxQueue;
    QueueHandle_t _txQueue;

    void Init(void);

    inline bool PutChar(char c);

    void StartSend(void);
    bool ContinueSend(void);

};


} // namespace stm32f3xx
} // namespace hw

#endif /* HW_STM32F3XX_UART_SHELL_H_ */
