#pragma once

#include <stdint.h>
#include "stm32f411x.h"
#include "stm32f411x_i2c_driver.h"
#include "stm32f411x_gpio_driver.h"

#define DS_1307_I2C_ADRESS 0x68

#define DS_1307_I2C           I2C1
#define DS_1307_I2C_GPIO_PORT GPIOB
#define DS_1307_I2C_SDA_PIN   GPIO_PIN_NO_9
#define DS_1307_I2C_SCL_PIN   GPIO_PIN_NO_8
#define DS_1307_I2C_SPEED     I2C_SCL_SPEED_SM
#define DS_1307_I2C_SPEED_MODE     I2C_SM_MODE

#define DS_1307_SEC   0x00
#define DS_1307_MIN   0x01
#define DS_1307_HOURS 0x02
#define DS_1307_DAY   0x03
#define DS_1307_DATE  0x04
#define DS_1307_MON   0x05
#define DS_1307_YEAR  0x06

typedef enum {
	TIME_FORMAT_12_HRS_AM = 0U,
	TIME_FORMAT_12_HRS_PM = 1U,
	TIME_FORMAT_24_HRS = 2U,
} Time_Formats;

typedef enum {
	MONDAY = 0U,
	TUESDAY = 1U,
	WEDNESDAY = 2U,
	THURSDAY = 3U,
	FRIDAY = 4U,
	SATURDAY = 5U,
	SUNDAY = 6U,
} Start_Day;

typedef enum {
	JANUARY = 0U,
	FEBRUARY = 1U,
	MARCH = 2U,
	APRIL = 3U,
	MAY = 4U,
	JUNE = 5U,
	JULY = 6U,
	AUGUST = 7U,
	SEPTEMBER = 8U,
	OCTOBER = 9U,
	NOVEMBER = 10U,
	DECEMBER = 11U,
} Start_Month;

typedef struct {
	Start_Day day;
	uint8_t date;
	Start_Month month;
	uint8_t year;
} RTC_Date;

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	Time_Formats time_format;
} RTC_Time;

uint8_t ds1307_init(void);

void ds1307_set_current_time(RTC_Time *rtc_time);
void ds1307_get_current_time(RTC_Time *rtc_time);

void ds1307_set_current_date(RTC_Date *rtc_date);
void ds1307_get_current_date(RTC_Date *rtc_date);
