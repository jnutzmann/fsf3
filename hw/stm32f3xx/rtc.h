#ifndef HW_STM32F3XX_RTC_H_
#define HW_STM32F3XX_RTC_H_



#include "stm32f3_hal/stm32f3xx_hal.h"


namespace hw {
namespace stm32f3xx {

class Rtc {
  public:
    Rtc(void);

    int SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
    int GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);
    int SetDate(uint8_t month, uint8_t day, uint8_t year, uint8_t weekday);
    int GetDate(uint8_t* month, uint8_t* day, uint8_t* year, uint8_t* weekday);

    void SetBackupRegister(uint8_t index, uint32_t value);
    uint32_t GetBackupRegister(uint8_t index);

  private:
    RTC_HandleTypeDef _rtc;
};



} // namespace stm32f3xx
} // namespace hw


#endif /* HW_STM32F3XX_RTC_H_ */
