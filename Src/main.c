#include <stdio.h>
#include "stm32f411x.h"
#include "ds1307.h"
#include "stm32f411x_gpio_driver.h"
#include "stm32f411x_i2c_driver.h"

#define SYSTICK_TIM_CLK 16000000UL

RTC_Time rtc_time;
RTC_Date rtc_date;

void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*) 0xE000E014;
	uint32_t *pSCSR = (uint32_t*) 0xE000E010;

	uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz) - 1;

	*pSRVR &= ~(0x00FFFFFFFF);

	*pSRVR |= count_value;

	*pSCSR |= (1U << 1);
	*pSCSR |= (1U << 2);

	*pSCSR |= (1U << 0);

}

int main(void)
{

	if (ds1307_init()) {
		while (1)
			;
	}

	init_systick_timer(1U);

	rtc_time.seconds = 0U;
	rtc_time.minutes = 0U;
	rtc_time.hours = 14U;
	rtc_time.time_format = TIME_FORMAT_24_HRS;

	rtc_date.day = SATURDAY;
	rtc_date.date = 29U;
	rtc_date.month = AUGUST;
	rtc_date.year = 26U;

	ds1307_set_current_time(&rtc_time);
	ds1307_set_current_date(&rtc_date);

	ds1307_get_current_time(&rtc_time);
	ds1307_get_current_date(&rtc_date);

	while (1)
		;

	return 0;
}

void SysTick_Handler(void)
{
	ds1307_get_current_time(&rtc_time);
	ds1307_get_current_date(&rtc_date);

	return;
}
