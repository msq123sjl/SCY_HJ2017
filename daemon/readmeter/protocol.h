#ifndef __PROTOCOL__
#define __PROTOCOL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define C10_ADDR0000_LEN 24

#define FLOWMETER_CD_RTD_LEN 18
#define FLOWMETER_CD_SUM_LEN 22
void Protocol_15(int port,int Address,pstMeterPara pMeterPara);
void Protocol_23(int port,int Address,pstMeterPara pMeterPara);//C10电导率
#endif

