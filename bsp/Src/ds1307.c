#include "ds1307.h"

I2C_Handle DS1307_I2CHandle;

static void ds1307_i2c_pin_config(void);
static void ds1307_i2c_config(void);
static void ds1307_write(uint8_t value, uint8_t reg_addr);
static uint8_t ds1307_read(uint8_t reg_addr);
static uint8_t binary_to_bcd(uint8_t value);
static uint8_t bcd_to_binary(uint8_t value);

static void ds1307_i2c_pin_config(void)
{
	GPIO_Handle I2CPins;

	I2CPins.pGPIOx = DS_1307_I2C_GPIO_PORT;
	I2CPins.GPIO_PinConfig.GPIO_PIN_MODE = GPIO_MODE_ALT_FUNC;
	I2CPins.GPIO_PinConfig.GPIO_PIN_ALT_FUNC_MODE = GPIO_AF_NO_4;
	I2CPins.GPIO_PinConfig.GPIO_PIN_OUTPUT_TYPE = GPIO_OPEN_DRAIN;
	I2CPins.GPIO_PinConfig.GPIO_PIN_PUPD_MODE = GPIO_NO_PUPD;
	I2CPins.GPIO_PinConfig.GPIO_PIN_SPEED = GPIO_SPEED_HIGH;

	I2CPins.GPIO_PinConfig.GPIO_PIN_NUMBER =
	DS_1307_I2C_SCL_PIN;
	GPIO_Init(&I2CPins);

	I2CPins.GPIO_PinConfig.GPIO_PIN_NUMBER =
	DS_1307_I2C_SDA_PIN;
	GPIO_Init(&I2CPins);

	return;
}

static void ds1307_i2c_config(void)
{
	I2C_PeriClockControl(DS_1307_I2C, ENABLE);

	DS1307_I2CHandle.pI2Cx = DS_1307_I2C;
	DS1307_I2CHandle.I2C_Config.SCLSpeed =
	DS_1307_I2C_SPEED;
	DS1307_I2CHandle.I2C_Config.SCLSpeedMode =
	DS_1307_I2C_SPEED_MODE;
	DS1307_I2CHandle.I2C_Config.DeviceAddress =
	DS_1307_I2C_ADRESS;
	DS1307_I2CHandle.I2C_Config.ACKControl = I2C_ACK_DISABLE;
	DS1307_I2CHandle.I2C_Config.FMDutyCycle = I2C_FM_DUTY_MODE_2;

	I2C_Init(&DS1307_I2CHandle);

	return;
}

static void ds1307_write(uint8_t value, uint8_t reg_addr)
{
	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = value;

	I2C_MasterSendData(&DS1307_I2CHandle, tx, 2, DS_1307_I2C_ADRESS,
			I2C_DISABLE_RS);

	return;
}

static uint8_t ds1307_read(uint8_t reg_addr)
{
	uint8_t rx;

	I2C_MasterSendData(&DS1307_I2CHandle, &reg_addr, 1, DS_1307_I2C_ADRESS,
			I2C_DISABLE_RS);
	I2C_MasterReceiveData(&DS1307_I2CHandle, &rx, 1, DS_1307_I2C_ADRESS,
			I2C_DISABLE_RS);

	return rx;
}

static uint8_t binary_to_bcd(uint8_t value)
{
	uint8_t m, n;
	uint8_t bcd;

	bcd = value;

	if (value >= 10) {
		m = value / 10;
		n = value % 10;
		bcd = (uint8_t) ((m << 4) | n);
	}

	return bcd;
}

static uint8_t bcd_to_binary(uint8_t value)
{
	uint8_t m, n;
	uint8_t binary;

	m = (uint8_t) ((value << 4) * 10);
	n = (value & (uint8_t) (0x0F));

	binary = m + n;

	return binary;
}

uint8_t ds1307_init(void)
{
	ds1307_i2c_pin_config();
	ds1307_i2c_config();

	I2C_Peripheral_Control(DS_1307_I2C, ENABLE);

	ds1307_write(0x00, DS_1307_SEC);

	uint8_t clock_state = ds1307_read(DS_1307_SEC);

	return (clock_state >> 7) & (0x1);
}

void ds1307_set_current_time(RTC_Time *rtc_time)
{
	uint8_t seconds = binary_to_bcd(rtc_time->seconds);
	uint8_t minutes = binary_to_bcd(rtc_time->minutes);
	uint8_t hours = binary_to_bcd(rtc_time->hours);

	seconds = (uint8_t) (seconds & ~(1U << 7));

	if (rtc_time->time_format == TIME_FORMAT_24_HRS) {
		hours = (uint8_t) (hours & ~(1U << 6));
	} else {
		hours |= (1U << 6);

		if (rtc_time->time_format == TIME_FORMAT_12_HRS_PM) {
			hours |= (1U << 5);
		} else {
			hours = (uint8_t) (hours & ~(1U << 5));
		}
	}

	ds1307_write(seconds, DS_1307_SEC);
	ds1307_write(minutes, DS_1307_MIN);
	ds1307_write(hours, DS_1307_HOURS);

	return;
}

void ds1307_get_current_time(RTC_Time *rtc_time)
{
	uint8_t seconds = bcd_to_binary(ds1307_read(DS_1307_SEC));
	uint8_t minutes = bcd_to_binary(ds1307_read(DS_1307_MIN));
	uint8_t hours = bcd_to_binary(ds1307_read(DS_1307_HOURS));

	seconds = (uint8_t) (seconds & ~(1U << 7));

	if (hours & (1U << 6)) {
		if (hours & (1U << 5)) {
			rtc_time->time_format = TIME_FORMAT_12_HRS_AM;
		} else {
			rtc_time->time_format = TIME_FORMAT_12_HRS_PM;
		}

		hours = (uint8_t) (hours & ~(0x3 << 5));
	} else {
		rtc_time->time_format = TIME_FORMAT_24_HRS;
	}

	rtc_time->seconds = bcd_to_binary(seconds);
	rtc_time->minutes = bcd_to_binary(minutes);
	rtc_time->hours = bcd_to_binary(hours);

	return;
}

void ds1307_set_current_date(RTC_Date *rtc_date)
{
	uint8_t day = binary_to_bcd(rtc_date->day);
	uint8_t date = binary_to_bcd(rtc_date->date);
	uint8_t month = binary_to_bcd(rtc_date->month);
	uint8_t year = binary_to_bcd(rtc_date->year);

	ds1307_write(day, DS_1307_DAY);
	ds1307_write(date, DS_1307_DATE);
	ds1307_write(month, DS_1307_MON);
	ds1307_write(year, DS_1307_YEAR);

	return;
}

void ds1307_get_current_date(RTC_Date *rtc_date)
{
	uint8_t day = bcd_to_binary(ds1307_read(DS_1307_DAY));
	uint8_t date = bcd_to_binary(ds1307_read(DS_1307_DATE));
	uint8_t month = bcd_to_binary(ds1307_read(DS_1307_MON));
	uint8_t year = bcd_to_binary(ds1307_read(DS_1307_YEAR));

	rtc_date->day = (Start_Day) day;
	rtc_date->date = date;
	rtc_date->month = (Start_Month) month;
	rtc_date->year = year;

	return;
}
