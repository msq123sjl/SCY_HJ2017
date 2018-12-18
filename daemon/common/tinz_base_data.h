#ifndef __TINZ_BASE_DATA__
#define __TINZ_BASE_DATA__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tinz_base_def.h"

typedef struct _MeterRtdData
{
	char		flag;
	float		Rtd;
	double		total;
}stMeterRtdData,*pstMeterRtdData;

typedef struct _MeterData
{
	uint16_t	CNT;
	float		RtdMax;
	float		RtdMin;
	double		RtdSum;
	double		totalStart;
	double		totalEnd;
}stMeterData,*pstMeterData;

typedef struct _Meter
{
	pstMeterPara 	para;
	stMeterRtdData	RtdData;
	stMeterData 	MinData;
	stMeterData 	HourData;
	stMeterData 	DayData;
}stMeter,*pstMeter;


#endif
