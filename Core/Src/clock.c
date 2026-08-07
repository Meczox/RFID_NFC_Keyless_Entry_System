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

static uint8_t month_from_build_date(void)
{
	if (__DATE__[0] == 'J' && __DATE__[1] == 'a') {
		return 1U;
	}
	if (__DATE__[0] == 'F') {
		return 2U;
	}
	if (__DATE__[0] == 'M' && __DATE__[2] == 'r') {
		return 3U;
	}
	if (__DATE__[0] == 'A' && __DATE__[1] == 'p') {
		return 4U;
	}
	if (__DATE__[0] == 'M' && __DATE__[2] == 'y') {
		return 5U;
	}
	if (__DATE__[0] == 'J' && __DATE__[2] == 'n') {
		return 6U;
	}
	if (__DATE__[0] == 'J' && __DATE__[2] == 'l') {
		return 7U;
	}
	if (__DATE__[0] == 'A' && __DATE__[1] == 'u') {
		return 8U;
	}
	if (__DATE__[0] == 'S') {
		return 9U;
	}
	if (__DATE__[0] == 'O') {
		return 10U;
	}
	if (__DATE__[0] == 'N') {
		return 11U;
	}
	return 12U;
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

bool Clock_SetDate(uint8_t day, uint8_t month, uint16_t year)
{
	uint32_t yearBcd;
	uint32_t monthBcd;
	uint32_t dayBcd;

	if (day < 1U || day > 31U || month < 1U || month > 12U
			|| year < 2000U || year > 2099U) {
		return false;
	}

	if (!enter_init_mode()) {
		return false;
	}

	yearBcd = to_bcd((uint8_t)(year - 2000U));
	monthBcd = to_bcd(month);
	dayBcd = to_bcd(day);

	RTC->DR = (((yearBcd >> 4U) & 0x0FU) << RTC_DR_YT_Pos)
			| ((yearBcd & 0x0FU) << RTC_DR_YU_Pos)
			| (((monthBcd >> 4U) & 0x01U) << RTC_DR_MT_Pos)
			| ((monthBcd & 0x0FU) << RTC_DR_MU_Pos)
			| (((dayBcd >> 4U) & 0x03U) << RTC_DR_DT_Pos)
			| ((dayBcd & 0x0FU) << RTC_DR_DU_Pos)
			| (1U << RTC_DR_WDU_Pos);

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
	uint8_t buildDay;
	uint8_t buildMonth;
	uint8_t buildHour;
	uint8_t buildMinute;
	uint8_t buildSecond;
	uint16_t buildYear;

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
	if (!exit_init_mode()) {
		RTC->WPR = 0xFFU;
		return false;
	}
	RTC->WPR = 0xFFU;

	buildDay = (uint8_t)(((__DATE__[4] == ' ' ? '0' : __DATE__[4]) - '0') * 10
			+ (__DATE__[5] - '0'));
	buildMonth = month_from_build_date();
	buildYear = (uint16_t)((__DATE__[7] - '0') * 1000
			+ (__DATE__[8] - '0') * 100
			+ (__DATE__[9] - '0') * 10
			+ (__DATE__[10] - '0'));
	buildHour = (uint8_t)(((__TIME__[0] - '0') * 10) + (__TIME__[1] - '0'));
	buildMinute = (uint8_t)(((__TIME__[3] - '0') * 10) + (__TIME__[4] - '0'));
	buildSecond = (uint8_t)(((__TIME__[6] - '0') * 10) + (__TIME__[7] - '0'));

	if (!Clock_SetDate(buildDay, buildMonth, buildYear)) {
		return false;
	}

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

bool Clock_GetDate(ClockDate_t *date)
{
	uint32_t dateRegister;
	uint32_t yearBcd;
	uint32_t monthBcd;
	uint32_t dayBcd;

	if (date == NULL || !clockReady) {
		return false;
	}

	(void)RTC->TR;
	dateRegister = RTC->DR;

	yearBcd = (((dateRegister & RTC_DR_YT) >> RTC_DR_YT_Pos) << 4U)
			| ((dateRegister & RTC_DR_YU) >> RTC_DR_YU_Pos);
	monthBcd = (((dateRegister & RTC_DR_MT) >> RTC_DR_MT_Pos) << 4U)
			| ((dateRegister & RTC_DR_MU) >> RTC_DR_MU_Pos);
	dayBcd = (((dateRegister & RTC_DR_DT) >> RTC_DR_DT_Pos) << 4U)
			| ((dateRegister & RTC_DR_DU) >> RTC_DR_DU_Pos);

	date->year = (uint16_t)(2000U + from_bcd(yearBcd));
	date->month = from_bcd(monthBcd);
	date->day = from_bcd(dayBcd);
	return true;
}

bool Clock_GetDateTime(ClockDateTime_t *dateTime)
{
	return dateTime != NULL
			&& Clock_GetDate(&dateTime->date)
			&& Clock_GetTime(&dateTime->time);
}
