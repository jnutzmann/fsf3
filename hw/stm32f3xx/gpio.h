#ifndef HW_STM32F3XX_GPIO_H_
#define HW_STM32F3XX_GPIO_H_


#include "stm32f3_hal/stm32f3xx_hal.h"


namespace hw {
namespace stm32f3xx {

class GpioPin {
  public:
    enum class Mode : unsigned {
      kInput      = GPIO_MODE_INPUT,
      kOutput     = GPIO_MODE_OUTPUT_PP,
      kAlternate  = GPIO_MODE_AF_PP,
      kAnalog     = GPIO_MODE_ANALOG
    };

    enum class OutputType : bool {
      kPushPull   = false,
      kOpenDrain  = true
    };

    enum class OutputSpeed : unsigned {
      kLow        = GPIO_SPEED_FREQ_LOW,
      kMedium     = GPIO_SPEED_FREQ_MEDIUM,
      kHigh       = GPIO_SPEED_FREQ_HIGH
    };

    enum class Pull : unsigned {
      kNoPull     = GPIO_NOPULL,
      kPullUp     = GPIO_PULLUP,
      kPullDown   = GPIO_PULLDOWN
    };

    enum class State : unsigned {
      kReset = 0x0,
      kSet   = 0x1
    };


    GpioPin(GPIO_TypeDef* port,
            uint16_t pin,
            Mode mode,
            OutputType outputType,
            OutputSpeed outputSpeed,
            Pull pull,
            State defaultState);

    void Configure(void);
    void Toggle(void);
    void Write(State state);
    State Read(void);


  protected:
    GPIO_TypeDef* _port;
    uint16_t _pin;
    Mode _mode;
    OutputType _outputType;
    OutputSpeed _outputSpeed;
    Pull _pull;
    State _defaultState;
};

} // namespace stm32f3xx
} // namespace hw


#endif
