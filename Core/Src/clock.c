#include "clock.h"

#include "main.h"

#define CLOCK_INIT_TIMEOUT_MS 1000U
#define RTC_ASYNC_PREDIV       124U
#define RTC_SYNC_PREDIV        319U
#define RTC_BACKUP_MAGIC       0x434C4B31U

static bool clockReady;

static uint32_t to_bcd(uint8_t value)
{
	return ((uint32_t)(value / 10U) << 4U) | (value % 10U);
}

static uint8_t from_bcd(uint32_t value)
{
	return (uint8_t)(((value >> 4U) * 10U) + (value & 0x0FU));
}

static bool enter_init_mode(void)
{
	uint32_t startedAt = HAL_GetTick();

	RTC->WPR = 0xCAU;
	RTC->WPR = 0x53U;
	RTC->ISR |= RTC_ISR_INIT;

	while ((RTC->ISR & RTC_ISR_INITF) == 0U) {
		if ((HAL_GetTick() - startedAt) >= CLOCK_INIT_TIMEOUT_MS) {
			RTC->WPR = 0xFFU;
			return false;
		}
	}

	return true;
}

static bool exit_init_mode(void)
{
	uint32_t startedAt;

	RTC->ISR &= ~RTC_ISR_INIT;
	RTC->ISR &= ~RTC_ISR_RSF;
	startedAt = HAL_GetTick();

	while ((RTC->ISR & RTC_ISR_RSF) == 0U) {
		if ((HAL_GetTick() - startedAt) >= CLOCK_INIT_TIMEOUT_MS) {
			return false;
		}
	}

	return true;
}

bool Clock_SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	uint32_t hourBcd;
	uint32_t minuteBcd;
	uint32_t secondBcd;

	if (hour > 23U || minute > 59U || second > 59U) {
		return false;
	}

	if (!enter_init_mode()) {
		return false;
	}

	hourBcd = to_bcd(hour);
	minuteBcd = to_bcd(minute);
	secondBcd = to_bcd(second);

	RTC->CR &= ~RTC_CR_FMT;
	RTC->TR = (((hourBcd >> 4U) & 0x03U) << RTC_TR_HT_Pos)
			| ((hourBcd & 0x0FU) << RTC_TR_HU_Pos)
			| (((minuteBcd >> 4U) & 0x07U) << RTC_TR_MNT_Pos)
			| ((minuteBcd & 0x0FU) << RTC_TR_MNU_Pos)
			| (((secondBcd >> 4U) & 0x07U) << RTC_TR_ST_Pos)
			| ((secondBcd & 0x0FU) << RTC_TR_SU_Pos);

	if (!exit_init_mode()) {
		RTC->WPR = 0xFFU;
		return false;
	}
	RTC->WPR = 0xFFU;
	clockReady = true;
	return true;
}

bool Clock_Init(void)
{
	uint8_t buildHour;
	uint8_t buildMinute;
	uint8_t buildSecond;

	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_RTC_ENABLE();
	clockReady = false;

	if (RTC->BKP0R == RTC_BACKUP_MAGIC) {
		clockReady = true;
		return true;
	}

	if (!enter_init_mode()) {
		return false;
	}

	RTC->CR &= ~RTC_CR_FMT;
	RTC->PRER = (RTC_SYNC_PREDIV << RTC_PRER_PREDIV_S_Pos);
	RTC->PRER |= (RTC_ASYNC_PREDIV << RTC_PRER_PREDIV_A_Pos);
	RTC->DR = (1U << RTC_DR_WDU_Pos)
			| (1U << RTC_DR_MU_Pos)
			| (1U << RTC_DR_DU_Pos);
	if (!exit_init_mode()) {
		RTC->WPR = 0xFFU;
		return false;
	}
	RTC->WPR = 0xFFU;

	buildHour = (uint8_t)(((__TIME__[0] - '0') * 10) + (__TIME__[1] - '0'));
	buildMinute = (uint8_t)(((__TIME__[3] - '0') * 10) + (__TIME__[4] - '0'));
	buildSecond = (uint8_t)(((__TIME__[6] - '0') * 10) + (__TIME__[7] - '0'));

	if (!Clock_SetTime(buildHour, buildMinute, buildSecond)) {
		return false;
	}

	RTC->BKP0R = RTC_BACKUP_MAGIC;
	return true;
}

bool Clock_GetTime(ClockTime_t *time)
{
	uint32_t timeRegister;
	uint32_t hourBcd;
	uint32_t minuteBcd;
	uint32_t secondBcd;

	if (time == NULL || !clockReady) {
		return false;
	}

	timeRegister = RTC->TR;
	(void)RTC->DR;

	hourBcd = (((timeRegister & RTC_TR_HT) >> RTC_TR_HT_Pos) << 4U)
			| ((timeRegister & RTC_TR_HU) >> RTC_TR_HU_Pos);
	minuteBcd = (((timeRegister & RTC_TR_MNT) >> RTC_TR_MNT_Pos) << 4U)
			| ((timeRegister & RTC_TR_MNU) >> RTC_TR_MNU_Pos);
	secondBcd = (((timeRegister & RTC_TR_ST) >> RTC_TR_ST_Pos) << 4U)
			| ((timeRegister & RTC_TR_SU) >> RTC_TR_SU_Pos);

	time->hour = from_bcd(hourBcd);
	time->minute = from_bcd(minuteBcd);
	time->second = from_bcd(secondBcd);
	return true;
}
