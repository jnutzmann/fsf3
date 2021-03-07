#ifndef HW_TPS92518_H_
#define HW_TPS92518_H_

#include <stdint.h>
#include "stm32f3xx/spi.h"
#include "stm32f3xx/gpio.h"

namespace hw {

typedef struct {
    uint16_t data : 9;
    uint16_t par  : 1;
    uint16_t addr : 5;
    uint16_t cmd  : 1;
} tps92618CmdFrame;

typedef struct {
    uint16_t data : 9;
    uint16_t tw   : 1;
    uint16_t uv1  : 1;
    uint16_t uv2  : 1;
    uint16_t pc   : 1;
    uint16_t rsvd : 2;
    uint16_t spe  : 1;
} tps92618ReadRsp;

typedef struct {
    uint16_t data : 9;
    uint16_t addr : 5;
    uint16_t cmd  : 1;
    uint16_t spe  : 1;
} tps92618WriteRsp;


class Tps92518 {
  public:

    enum class Register : uint16_t {
    CONTROL           = 0x00,
    STATUS            = 0x01,
    THERM_WARN_LMT    = 0x02,
    LED1_PKTH_DAC     = 0x03,
    LED2_PKTH_DAC     = 0x04,
    LED1_TOFF_DAC     = 0x05,
    LED2_TOFF_DAC     = 0x06,
    LED1_MAXOFF_DAC   = 0x07,
    LED2_MAXOFF_DAC   = 0x08,
    VTHERM            = 0x09,
    LED1_MOST_RECENT  = 0x0A,
    LED1_LAST_ON      = 0x0B,
    LED1_LAST_OFF     = 0x0C,
    LED2_MOST_RECENT  = 0x0D,
    LED2_LAST_ON      = 0x0E,
    LED2_LAST_OFF     = 0x0F,
    RESET             = 0x10
    };

    Tps92518(stm32f3xx::SpiMaster* spi, stm32f3xx::GpioPin* ss);

    int Read(Register reg, tps92618ReadRsp* resp);
    int Write(Register reg, uint16_t data, tps92618WriteRsp* resp);

  private:
    stm32f3xx::SpiMaster* _spi;
    stm32f3xx::GpioPin* _ss;
};

} // namespace hw

#endif /* HW_TPS92518_H_ */
