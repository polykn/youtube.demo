#ifndef _UTIL_H
#define _UTIL_H

#define uint32_t unsigned long
#define uint8_t unsigned char

typedef struct
{
    unsigned int sec;    /* seconds after the minute (0 to 61) */
    unsigned int min;    /* minutes after the hour (0 to 59) */
    unsigned int hour;   /* hours since midnight (0 to 23) */
    unsigned int mday;   /* day of the month (1 to 31) */
    unsigned int mon;    /* months since January (0 to 11) */
    unsigned int year;   /* years since 1900 */
    unsigned int wday;   /* days since Sunday (0 to 6 Sunday=0) */
} CalenderData_TypeDef;

unsigned char utilUnixToCalender(unsigned long Unix, CalenderData_TypeDef *Cal);
unsigned char utilMultiplier(unsigned char m, unsigned char v);
uint32_t miscCalenderToUnix(uint8_t Year, uint8_t Month, uint8_t Day, uint8_t Hour, uint8_t Minute, uint8_t Seconds);
void untime(unsigned long unixtime, CalenderData_TypeDef *Cal);

#endif