
#include "tps92518.h"

using namespace hw::stm32f3xx;

namespace hw {

Tps92518::Tps92518(stm32f3xx::SpiMaster spi, stm32f3xx::GpioPin ss)
: _spi(spi),
  _ss(ss) {}

int Tps92518::Read(Register reg, tps92618ReadRsp* resp) {

  tps92618CmdFrame cmd = {
      .data = 0,
      .par  = 0,
      .addr = (uint8_t)reg,
      .cmd  = 0,
  };

  PopulateParity(&cmd);

  _ss.Write(GpioPin::State::kReset);
  int result = _spi.TransmitReceive((uint8_t*)&cmd, (uint8_t*)resp, 1, 10);
  _ss.Write(GpioPin::State::kSet);

  return result;
}

int Tps92518::Write(Register reg, uint16_t data, tps92618WriteRsp* resp) {
  tps92618CmdFrame cmd = {
        .data = data,
        .par  = 0,
        .addr = (uint8_t)reg,
        .cmd  = 1,
    };

    PopulateParity(&cmd);

    _ss.Write(GpioPin::State::kReset);
    int result = _spi.TransmitReceive((uint8_t*)&cmd, (uint8_t*)resp, 1, 10);
    _ss.Write(GpioPin::State::kSet);

    return result;
}


static void PopulateParity(tps92618CmdFrame* cmdFrame) {

  bool parity = 0;
  cmdFrame->par = 0;

  uint16_t n = *((uint16_t*)cmdFrame);

  while (n) {
    parity = !parity;
    n = n & (n-1);
  }

  cmdFrame->par = !parity;
}


} // namespace hw



