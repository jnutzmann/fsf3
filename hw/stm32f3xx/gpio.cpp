
#include "gpio.h"

namespace hw {
namespace stm32f3xx {

GpioPin::GpioPin(GPIO_TypeDef* port,
                 uint16_t pin,
                 Mode mode,
                 OutputType outputType,
                 OutputSpeed outputSpeed,
                 Pull pull)
   : _port(port),
     _pin(pin),
     _mode(mode),
     _outputType(outputType),
     _outputSpeed(outputSpeed),
     _pull(pull) {}


void GpioPin::ConfigureOutput(void) {

  GPIO_InitTypeDef gpio_init = {0};

  if (_port == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
  else if (_port == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
  else if (_port == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
  else if (_port == GPIOF) { __HAL_RCC_GPIOF_CLK_ENABLE(); }

  HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);

  gpio_init.Pin = _pin;

  unsigned m = (uint16_t) _mode;

  if(_mode == Mode::kOutput) {
    if (_outputType == OutputType::kOpenDrain) {
      m = m & 0x10;
    }
  }

  gpio_init.Mode = m;
  gpio_init.Pull = (uint32_t) _pull;
  gpio_init.Speed = (uint32_t) _outputSpeed;

  HAL_GPIO_Init(_port, &gpio_init);

}

void GpioPin::Toggle(void) {
  HAL_GPIO_TogglePin(_port, _pin);
}

}
}
