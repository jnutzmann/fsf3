/*
 * uart.h
 *
 *  Created on: Mar 5, 2021
 *      Author: jnutzmann
 */

#ifndef HW_STM32F3XX_UART_SHELL_H_
#define HW_STM32F3XX_UART_SHELL_H_

#include "freertos/thread.hpp"
#include "stm32f3_hal/stm32f3xx_hal.h"

namespace hw {
namespace stm32f3xx {

class UartShell : freertos::Thread {

  public:
    UartShell::UartShell();

    void ServiceInterrupt(void);

  private:
    UART_HandleTypeDef _uart;
};


} // namespace stm32f3xx
} // namespace hw

#endif /* HW_STM32F3XX_UART_SHELL_H_ */
