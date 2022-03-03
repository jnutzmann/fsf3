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

#define MAX_CMD_LENGTH 32
#define CMD_BUFFER_LENGTH (MAX_CMD_LENGTH+1) // Add one for the null character

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
      uint8_t txQueueLength;
      uint8_t rxQueueLength;
    };

    UartShell(const Config & config);

    void ServiceInterrupt(void);

    bool PutString(const char* s);
    int Write(int file, char *ptr, int len);


  protected:
    virtual void Run(void);

  private:
    const Config & _config;

    UART_HandleTypeDef _uart;

    freertos::Queue _txQueue;
    freertos::Queue _rxCmdQueue;
    volatile char _rxBuffer[CMD_BUFFER_LENGTH];
    volatile uint8_t _rxBufferIndex;

    void InitHardware(void);

    inline bool PutChar(char c);
    void StartSend(void);
    bool ContinueSend(void);

};


} // namespace stm32f3xx
} // namespace hw

#endif /* HW_STM32F3XX_UART_SHELL_H_ */
