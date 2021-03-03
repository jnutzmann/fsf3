

#include "spi.h"

namespace hw {
namespace stm32f3xx {

const unsigned SpiMaster::_sizeMap[] = {
              0, // 0
              0, // 1
              0, // 2
              0, // 3
              SPI_DATASIZE_4BIT,
              SPI_DATASIZE_5BIT,
              SPI_DATASIZE_6BIT,
              SPI_DATASIZE_7BIT,
              SPI_DATASIZE_8BIT,
              SPI_DATASIZE_9BIT,
              SPI_DATASIZE_10BIT,
              SPI_DATASIZE_11BIT,
              SPI_DATASIZE_12BIT,
              SPI_DATASIZE_13BIT,
              SPI_DATASIZE_14BIT,
              SPI_DATASIZE_15BIT,
              SPI_DATASIZE_16BIT
};


SpiMaster::SpiMaster(uint32_t dataSize,
                     GPIO_TypeDef* miso_port, uint16_t miso_pin,
                     GPIO_TypeDef* mosi_port, uint16_t mosi_pin,
                     GPIO_TypeDef* sck_port,  uint16_t sck_pin) {

  _dataSize = dataSize;

  // Enable the clock for the peripheral
  __HAL_RCC_SPI1_CLK_ENABLE();

  // Set up the GPIO for the SPI pins
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;

  GPIO_InitStruct.Pin = miso_pin;
  HAL_GPIO_Init(miso_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = mosi_pin;
  HAL_GPIO_Init(mosi_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = sck_pin;
  HAL_GPIO_Init(sck_port, &GPIO_InitStruct);

  Configure();

}

int SpiMaster::Configure(void) {
  _spiHandle.Instance  = SPI1;

  _spiHandle.Init.Mode = SPI_MODE_MASTER;
  _spiHandle.Init.Direction = SPI_DIRECTION_2LINES;
  _spiHandle.Init.DataSize = _sizeMap[_dataSize];
  _spiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
  _spiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
  _spiHandle.Init.NSS = SPI_NSS_SOFT;
  _spiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  _spiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
  _spiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
  _spiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  _spiHandle.Init.CRCPolynomial = 7;
  _spiHandle.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  _spiHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

  if (HAL_SPI_Init(&_spiHandle) != HAL_OK)
  {
    return 1;
  }

  return 0;
}


int SpiMaster::Transmit(uint8_t* pData, uint16_t size,
                             uint32_t timeout) {
  return HAL_SPI_Transmit(&_spiHandle, pData, size, timeout);
}

int SpiMaster::Receive(uint8_t* pData, uint16_t size,
                            uint32_t timeout) {
  return HAL_SPI_Receive(&_spiHandle, pData, size, timeout);
}

int SpiMaster::TransmitReceive(uint8_t* txData, uint8_t* rxData, uint16_t size,
                                uint32_t timeout) {
  return HAL_SPI_TransmitReceive(&_spiHandle, txData, rxData, size, timeout);
}

} // namespace stm32f3xx
} // namespace hw
