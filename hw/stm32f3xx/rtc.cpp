/*
 * rtc.cpp
 *
 *  Created on: Mar 2, 2021
 *      Author: jnutzmann
 */


#include "rtc.h"

namespace hw {
namespace stm32f3xx {

Rtc::Rtc() {

    __HAL_RCC_RTC_ENABLE();

    _rtc.Instance = RTC;
    _rtc.Init.HourFormat = RTC_HOURFORMAT_24;
    _rtc.Init.AsynchPrediv = 127;
    _rtc.Init.SynchPrediv = 255;
    _rtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    _rtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    _rtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    HAL_RTC_Init(&_rtc);
}

int Rtc::SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {

  RTC_TimeTypeDef time = {0};
  time.Hours = hours;
  time.Minutes = minutes;
  time.Seconds = seconds;

  return HAL_RTC_SetTime(&_rtc, &time, RTC_FORMAT_BIN);
}

int Rtc::GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds) {

  RTC_TimeTypeDef time;

  int result = HAL_RTC_GetTime(&_rtc, &time, RTC_FORMAT_BIN);

  *hours = time.Hours;
  *minutes = time.Minutes;
  *seconds = time.Seconds;

  return result;
}

int Rtc::SetDate(uint8_t month, uint8_t day, uint8_t year, uint8_t weekday) {

  RTC_DateTypeDef date = {0};
  date.Month = month;
  date.Date = day;
  date.Year = year;
  date.WeekDay = weekday;

  return HAL_RTC_SetDate(&_rtc, &date, RTC_FORMAT_BIN);
}

int Rtc::GetDate(uint8_t* month, uint8_t* day, uint8_t* year, uint8_t* weekday) {
  RTC_DateTypeDef date;

  int result = HAL_RTC_GetDate(&_rtc, &date, RTC_FORMAT_BIN);

  *month = date.Month;
  *day = date.Date;
  *year = date.Year;
  *weekday = date.WeekDay;

  return result;
}

void Rtc::SetBackupRegister(uint8_t index, uint32_t value) {
  HAL_RTCEx_BKUPWrite(&_rtc, index, value);
}

uint32_t Rtc::GetBackupRegister(uint8_t index) {
  return HAL_RTCEx_BKUPRead(&_rtc, index);
}




} // namespace stm32f3xx
} // namespace hw

