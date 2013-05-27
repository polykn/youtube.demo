
#include "util.h"
#include <stdio.h>



#define YEAR_TO_DAYS(y) ((y)*365 + (y)/4 - (y)/100 + (y)/400)

int const YearDayConstants[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365}; // Days to add, depending on the month
int const WeekDayConstants[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};



unsigned char utilMultiplier(unsigned char m, unsigned char v)
{
	return m * v;
}

unsigned char utilUnixToCalender(unsigned long Unix, CalenderData_TypeDef *Cal)
{
	unsigned long days = Unix / 86400;
	unsigned long years;
	unsigned long addleap = 0;
	unsigned char c = 0;
	unsigned long timeseconds = 0;

	timeseconds = ( Unix - ( days * 86400 ) );
	days++; // Increment the current day
	
	for(years = 1970;1;years++)
	{
		if(!(years%4)) // LEAP YEAR
			addleap = 1;
		else
			addleap = 0;
			
		if(days > ( 365 + addleap ) )
			days -= 365 + addleap;
		else
			break;
	}
	if(addleap && days == 60)
	{
		printf("LEAPDAY!!! \n\r");
		Cal->mon = 2;
		Cal->mday = 29;
	}
	else
	{
		if(addleap && days>60)
			days--; // If the leadday has passed on a leap year then increment the days value

		for(c = 1; c<=12; c++)
		{
			if(days<=YearDayConstants[c])
			{
				Cal->mon = c;
				Cal->mday = days - YearDayConstants[c-1];
				break;
			}
		}
	}
	if(years>= 2000)
		Cal->year = years-2000;
	else
		Cal->year = years-1900;
		
	Cal->hour = ( timeseconds / 3600 );
	timeseconds -= ( Cal->hour * 3600);
	Cal->min = ( timeseconds / 60 );
	timeseconds -= ( Cal->min * 60);
	Cal->sec = timeseconds;
	
	return 1;
}





void untime(unsigned long unixtime, CalenderData_TypeDef *Cal)
{
	unsigned int y = 0;
    /* First take out the hour/minutes/seconds - this part is easy. */
    Cal->sec = unixtime % 60;
    unixtime /= 60;
    Cal->min = unixtime % 60;
    unixtime /= 60;
    Cal->hour = unixtime % 24;
    unixtime /= 24;

    /* unixtime is now days since 01/01/1970 UTC
     * Rebaseline to the Common Era 
	 * Days from year 0 to 1969(719162) + 
	 * (365-28(offset 28 feb)) 337*/
    unixtime += 719499;

	/*
	*	Find the year by checking the days since year 
	*/
    for (Cal->year = 1969; unixtime > YEAR_TO_DAYS(Cal->year + 1) + 30; Cal->year++)
        ;
		
    /* OK we have our "year", so subtract off the days accounted for by full years. */
    unixtime -= YEAR_TO_DAYS(Cal->year);

    /* unixtime is now number of days we are into the year (remembering that March 1
     * is the first day of the "year" still). */

    /* Roll forward looking for the month.  1 = March through to 12 = February. */
    for (Cal->mon = 1; Cal->mon < 12 && unixtime > 367*(Cal->mon+1)/12; Cal->mon++)
		;

    /* Subtract off the days accounted for by full months */
    unixtime -= 367*Cal->mon/12;

    /* unixtime is now number of days we are into the month */

    /* Adjust the month/year so that 1 = January, and years start where we
     * usually expect them to. */
    Cal->mon += 2;
    if (Cal->mon > 12)
    {
        Cal->mon -= 12;
        Cal->year++;
    }

    Cal->mday = unixtime;
	
	if(Cal->year >= 2000)
		Cal->year -= 2000;
	else
		Cal->year -= 1900;
	
	
	y = Cal->year;
	/* Get the weekday Cal->yearfrom the year, month and day of month */
    y -= Cal->mon < 3;
    Cal->wday = (y + y/4 - y/100 + y/400 + WeekDayConstants[Cal->mon-1] + Cal->mday) % 7;
	if(!Cal->wday) // The algoritm returns sunday as 0, the STM RTC Library has sunday value to 7
		Cal->wday = 7;
}

uint32_t miscCalenderToUnix(uint8_t Year, uint8_t Month, uint8_t Day, uint8_t Hour, uint8_t Minute, uint8_t Seconds)
{
	uint32_t unix = 0;
	uint32_t cnt_i = 0;
	uint32_t days = 0;
	uint32_t addyears = 0;
	
	if(Year > 70 && Year <= 99)
		addyears = 1900;
	else
		addyears = 2000;
	
	days=YearDayConstants[Month-1];

	if(Month>2 && !((Year+addyears)%4)) // If current year is leap year, add a day
		days++;

	unix += (uint32_t)Seconds;
	//unix +=	(uint32_t)((uint32_t)Minute*60);
	//unix += (uint32_t)((uint32_t)Hour*3600);
	//unix += (uint32_t)(((uint32_t)Day-1)*86400);
	//unix += (uint32_t)((uint32_t)days*86400);
	//unix += (uint32_t)((((uint32_t)Year+addyears)-1970)*31536000);

	for(cnt_i=1970;cnt_i<(Year+addyears);cnt_i++) // Calculate leapyears
	{
		if(!(cnt_i%4))
			unix+=86400;
	}

	return unix;
}

