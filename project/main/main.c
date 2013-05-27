#include <stdio.h>
#include "util.h"
#include <time.h>

// Uplloaded files to github.com, YAY!!

#define YEAR_TO_DAYS(y) ((y)*365 + (y)/4 - (y)/100 + (y)/400)

#define uint8_t unsigned char
#define int32_t	unsigned long
#define uint16_t unsigned short

#define NVM_OFFSET_PROFILE_START_TIME 	0x00
#define NVM_OFFSET_PROFILE_TIME 		0x01
#define NVM_OFFSET_START_END_TEMP		0x02
#define NVM_OFFSET_CAVELIGHT_START_TIME	0x03
#define NVM_OFFSET_CAVELIGHT_TIME		0x04

uint8_t nvmReadValue(uint8_t test, uint32_t* dummy )
{
	if(test == NVM_OFFSET_START_END_TEMP)
		*dummy = 0x2214; 

	return 1;
}

uint8_t nvmRead4ByteValue(uint8_t test, uint32_t* dummy )
{
	if(test == NVM_OFFSET_PROFILE_START_TIME)
		*dummy = 864000; 
	else if(test == NVM_OFFSET_PROFILE_TIME)
		*dummy = 0;
	else if(test == NVM_OFFSET_CAVELIGHT_START_TIME)
		*dummy = 691200;
	else if(test == NVM_OFFSET_CAVELIGHT_TIME)
		*dummy = 0;
	
	return 1;
}

uint8_t nvmWrite4ByteValue(uint8_t test, uint32_t dummy )
{
	if(test == NVM_OFFSET_PROFILE_START_TIME)
		printf("NEW NVM_OFFSET_PROFILE_START_TIME %u \n\r", dummy);
	else if(test == NVM_OFFSET_PROFILE_TIME)
		printf("NEW NVM_OFFSET_PROFILE_TIME %u \n\r", dummy);
	else if(test == NVM_OFFSET_CAVELIGHT_START_TIME)
		printf("NEW NVM_OFFSET_CAVELIGHT_START_TIME %u \n\r", dummy);
	else if(test == NVM_OFFSET_CAVELIGHT_TIME)
		printf("NEW NVM_OFFSET_CAVELIGHT_TIME %u \n\r", dummy);	
		
	return 1;
}

uint8_t caveGetTargetTemp(uint8_t Percent, uint8_t *ReqTemp)
{
	uint8_t retval = 0;
	uint16_t temps,starttemp,stoptemp  = 0;

	if(nvmReadValue(NVM_OFFSET_START_END_TEMP, &temps))
	{
		starttemp = (( temps & 0xff00) >> 8);
		stoptemp = ( temps & 0x00ff);
		
		if(Percent)
		{
			if(starttemp>=stoptemp)
			{
				starttemp++;
				temps = ( starttemp - stoptemp );
				*ReqTemp =  ( temps * ( 100 - Percent ) / 100 ) + ( stoptemp ) ;			
			}
			else
			{
				stoptemp++;
				temps = ( stoptemp - starttemp);
				*ReqTemp =  ( temps * ( Percent ) / 100 ) + ( starttemp ) ;		
			}
		}
		else
			*ReqTemp = starttemp;

		retval = 1;
	}

	return retval;
}

uint8_t caveGetProgress(uint8_t *RetPercent)
{
	uint8_t retval = 0;
	uint32_t timeremaining, starttime = 0;
	
	if(nvmRead4ByteValue(NVM_OFFSET_PROFILE_TIME, &timeremaining) && nvmRead4ByteValue(NVM_OFFSET_PROFILE_START_TIME, &starttime))
	{
		if(timeremaining>starttime)
		{
			timeremaining -= starttime;
			
		}
	
		if(timeremaining)
		{
			*RetPercent = 100 - (( timeremaining / ( starttime / 100 ) ));
			
			if(*RetPercent == 0)
				*RetPercent = 1;
			else if(*RetPercent >= 100)
				*RetPercent = 99;
		}
		else
		{
			*RetPercent = 0;
		}
		
	}
	
	return retval;
}

uint8_t caveSetProgress(uint8_t Percent)
{
	uint8_t retval = 0;
	uint32_t starttime, ledtime = 0;

	if(Percent <= 100)
	{
		Percent = 100 - Percent;

		if(nvmRead4ByteValue(NVM_OFFSET_PROFILE_START_TIME, &starttime))
		{
			if(Percent == 99)
			{
				// Insert START time in time remaining in both heater and cavelight
				if( nvmWrite4ByteValue(NVM_OFFSET_PROFILE_TIME, starttime) )
				{
					if(nvmRead4ByteValue(NVM_OFFSET_CAVELIGHT_START_TIME, &ledtime))
						if(nvmWrite4ByteValue(NVM_OFFSET_CAVELIGHT_TIME, ledtime))
							retval = 1;
				}
			}
			else if( ( Percent < 99 ) && ( Percent > 0) )
			{
				if(nvmWrite4ByteValue(NVM_OFFSET_PROFILE_TIME, ( ( starttime / 100 ) ) * Percent))
				{
					// Figure out of the new profile time is at a state where the LED is supposed to light
					if(nvmRead4ByteValue(NVM_OFFSET_CAVELIGHT_START_TIME, &ledtime))
					{
						// The start time substracted with the remaining seconds equals the amount of seconds processed
						// so far. If the LED time it bigger than that, then the profile should be in a state where 
						// The LED is supposed to be lit.
						if(	( starttime - ( ( ( starttime / 100 ) ) * Percent ) ) < ledtime)
							ledtime = ledtime - ( starttime - ( ( ( starttime / 100 ) ) * Percent ) ); // Insert led time substracted with seconds passed
						else
							ledtime = 0;
						
						if(nvmWrite4ByteValue(NVM_OFFSET_CAVELIGHT_TIME, ledtime))
							retval = 1;
					}
				}	
			}
			else
			{
				if(nvmWrite4ByteValue(NVM_OFFSET_PROFILE_TIME, 0))
					if(nvmWrite4ByteValue(NVM_OFFSET_CAVELIGHT_TIME, 0))
						retval = 1;

			}
		}
	}
	return retval;
}



main()
{
uint32_t percent = 0;
uint8_t requestettemp = 0;
unsigned long power = 0;
unsigned long heaterpower = 170000;
unsigned long heaterseconds = 31536000;

	unsigned char k = 0;
	unsigned long i = 0;
	unsigned long timestamp = 857549550;
	CalenderData_TypeDef caltest;
	CalenderData_TypeDef caltest2;

	unsigned long tester = 0;

	printf("Days gone in 1969 %d \n\r", YEAR_TO_DAYS(1969));
	
	timestamp = miscCalenderToUnix(95,3,5,8,12,30);

/*	
	for(i = 0; i<52;i++)
	{
		//utilUnixToCalender(timestamp, &caltest);
		untime(timestamp, &caltest2);
		
		//printf("MIN : %d-%d-%d, %d:%d:%d Weekday: %d\n\r", caltest.mday, caltest.mon, caltest.year, caltest.hour, caltest.min, caltest.sec, caltest.wday);
		printf("TIME : %d-%d-%d, %d:%d:%d Weekday: %d\n\r", caltest2.mday, caltest2.mon, caltest2.year, caltest2.hour, caltest2.min, caltest2.sec, caltest2.wday);
		timestamp += ( 3600 * 24 );
	}
*/
/*
	power = ( heaterpower ) * ( heaterseconds / 3600 );
	power += ( heaterpower / 60 ) * ( (heaterseconds % 3600) / 60 ) ;
	
	printf("milliWatts = %u \n\r", power );
	printf("Watts = %u \n\r", ( power / 1000 ) );
	printf("KWatts = %u \n\r", ( power / 1000000 ) );
*/	
	caveGetProgress(&percent);
	printf("PROCENT = %u \n\r", percent);
	caveGetTargetTemp(percent, &requestettemp);
	printf("TARGET TEMP : %u \n\r", requestettemp);
	
	while(getchar() != 'q'){}
}
