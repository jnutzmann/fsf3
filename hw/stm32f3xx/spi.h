

#ifndef HW_STM32F3XX_SPI_H_
#define HW_STM32F3XX_SPI_H_



#include "stm32f3_hal/stm32f3xx_hal.h"

#include "gpio.h"

namespace hw {
namespace stm32f3xx {

class SpiMaster {
  public:
    SpiMaster(uint32_t dataSize,
              GPIO_TypeDef* miso_port, uint16_t miso_pin,
              GPIO_TypeDef* mosi_port, uint16_t mosi_pin,
              GPIO_TypeDef* sck_port,  uint16_t sck_pin);

    int Configure(void);


    int Transmit(uint8_t* pData, uint16_t size,
                                 uint32_t timeout = 100);

    int Receive(uint8_t* pData, uint16_t size,
                                uint32_t timeout = 100);

    int TransmitReceive(uint8_t* txData, uint8_t* rxData, uint16_t size,
                                    uint32_t timeout = 100);

  private:
    SPI_HandleTypeDef _spiHandle;
    uint32_t _dataSize;

    const static unsigned _sizeMap[17];

};

} // namespace stm32f3xx
} // namespace hw


#endif /* HW_STM32F3XX_SPI_H_ */
