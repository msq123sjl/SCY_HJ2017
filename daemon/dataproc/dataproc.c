#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "sqlite3.h"


#include "tinz_common_helper.h"
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_message.h"
#include "tinz_common_db_helper.h"

#include "dataproc.h"

stMeter 		Meter[METER_CNT];
pstPara 		pgPara;
int 			gPrintLevel = 5;
tinz_db_ctx_t 	scy_data;

void _proj_init(void)__attribute__((constructor));
void _proj_uninit(void)__attribute__((destructor));

void _proj_init(void){
	DEBUG_PRINT_INFO(gPrintLevel, "start!!!\n");
}
void _proj_uninit(void)
{
	DEBUG_PRINT_INFO(gPrintLevel, "stop!!!\n");
}

static void rtd_data_proc(time_t sec){
	char 	TableName[TABLE_NAME_LEN];
	char 	sql[SQL_LEN];
	char 	RtdBuf[DOUBLE_STRING_LEN];
	char 	DoubleBuf[DOUBLE_STRING_LEN];
	int		iLoop;
	struct tm *tm;
	
	tm = localtime(&sec);
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		if(pgPara->MeterPara[iLoop].isValid){

			snprintf(TableName,sizeof(TableName)-1,"Rtd_%s_%4d%02d",pgPara->MeterPara[iLoop].Code,\
												tm->tm_year + 1990, tm->tm_mon + 1);
			if(TINZ_OK != TableIsExist(&scy_data,TableName)){
	            RtdTableCreate(&scy_data,TableName);
	        }
			Meter[iLoop].RtdData.flag	= pgPara->MeterPara[iLoop].flag;
			Meter[iLoop].RtdData.Rtd 	= pgPara->MeterPara[iLoop].Rtd;
			Meter[iLoop].RtdData.total 	= pgPara->MeterPara[iLoop].total;
			DoubleToString(Meter[iLoop].RtdData.Rtd,pgPara->MeterPara[iLoop].Decimals,RtdBuf);
			if(pgPara->MeterPara[iLoop].CouFlag){
				DoubleToString(Meter[iLoop].RtdData.total,pgPara->MeterPara[iLoop].Decimals,DoubleBuf);
			}else{
				snprintf(DoubleBuf,sizeof(DoubleBuf)-1,"-");
			}
			snprintf(sql,sizeof(sql)-1,"insert into %s values (%4d-%02d-%02d %02d:%02d:%02d,%s,%c,%s);",TableName,\
										tm->tm_year + 1990, tm->tm_mon + 1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec,\
										RtdBuf,
										pgPara->MeterPara[iLoop].flag,
										DoubleBuf);
			tinz_db_exec(&scy_data,sql);
		} 
	}
}

static void min_data_proc(time_t sec){
	char 	TableName[TABLE_NAME_LEN];
	char 	sql[SQL_LEN];
	char 	CouBuf[DOUBLE_STRING_LEN];
	char 	MaxBuf[DOUBLE_STRING_LEN];
	char 	MinBuf[DOUBLE_STRING_LEN];
	char 	AvgBuf[DOUBLE_STRING_LEN];
	int		iLoop;
	struct tm 	*tm;
	pstMeter 	pMeter;
	double		Cou;
	
	tm = localtime(&sec);
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		pMeter = (pstMeter)&Meter[iLoop];
		if(pMeter->para->isValid){

			snprintf(TableName,sizeof(TableName)-1,"Mins_%s_%4d%02d",pMeter->para->Code,\
												tm->tm_year + 1990, tm->tm_mon + 1);
			if(TINZ_OK != TableIsExist(&scy_data,TableName)){
	            CountDataTableCreate(&scy_data,TableName);
	        }
			/*增量*/
			if(pMeter->para->CouFlag){
				Cou = (pMeter->MinData.totalStart >= 0 && pMeter->MinData.totalEnd > pMeter->MinData.totalStart) ? pMeter->MinData.totalEnd - pMeter->MinData.totalStart : 0;
				DoubleToString(Cou,pMeter->para->Decimals,CouBuf);
			}else{
				snprintf(CouBuf,sizeof(CouBuf)-1,"-");
			}
			/*最大值*/
			if(pMeter->para->MaxFlag){
				DoubleToString(pMeter->MinData.RtdMax ,pMeter->para->Decimals,MaxBuf);
			}else{
				snprintf(MaxBuf,sizeof(MaxBuf)-1,"-");
			}
			/*最小值*/
			if(pMeter->para->MinFlag){
				DoubleToString(pMeter->MinData.RtdMin,pMeter->para->Decimals,MinBuf);
			}else{
				snprintf(MinBuf,sizeof(MinBuf)-1,"-");
			}
			/*平均值*/
			if(pMeter->para->AvgFlag){
				DoubleToString(double_div_uint(pMeter->MinData.RtdSum, (unsigned int)pMeter->MinData.CNT),pMeter->para->Decimals,AvgBuf);
			}else{
				snprintf(AvgBuf,sizeof(AvgBuf)-1,"-");
			}
			snprintf(sql,sizeof(sql)-1,"insert into %s values (%4d-%02d-%02d %02d:%02d:00,%s,%s,%s,%s);",TableName,\
										tm->tm_year + 1990, tm->tm_mon + 1,tm->tm_mday,tm->tm_hour,tm->tm_min,\
										MaxBuf,
										MinBuf,
										AvgBuf,
										CouBuf);
			tinz_db_exec(&scy_data,sql);
		} 
	}
}

static void hour_data_proc(time_t sec){
	char 	TableName[TABLE_NAME_LEN];
	char 	sql[SQL_LEN];
	char 	CouBuf[DOUBLE_STRING_LEN];
	char 	MaxBuf[DOUBLE_STRING_LEN];
	char 	MinBuf[DOUBLE_STRING_LEN];
	char 	AvgBuf[DOUBLE_STRING_LEN];
	int		iLoop;
	struct tm 	*tm;
	pstMeter 	pMeter;
	double		Cou;
	
	tm = localtime(&sec);
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		pMeter = &Meter[iLoop];
		if(pMeter->para->isValid){

			snprintf(TableName,sizeof(TableName)-1,"Hour_%s",pMeter->para->Code);
			if(TINZ_OK != TableIsExist(&scy_data,TableName)){
	            CountDataTableCreate(&scy_data,TableName);
	        }
			/*增量*/
			if(pMeter->para->CouFlag){
				Cou = (pMeter->HourData.totalStart >= 0 && pMeter->HourData.totalEnd > pMeter->HourData.totalStart) ? pMeter->HourData.totalEnd - pMeter->HourData.totalStart : 0;
				DoubleToString(Cou,pMeter->para->Decimals,CouBuf);
			}else{
				snprintf(CouBuf,sizeof(CouBuf)-1,"-");
			}
			/*最大值*/
			if(pMeter->para->MaxFlag){
				DoubleToString(pMeter->HourData.RtdMax ,pMeter->para->Decimals,MaxBuf);
			}else{
				snprintf(MaxBuf,sizeof(MaxBuf)-1,"-");
			}
			/*最小值*/
			if(pMeter->para->MinFlag){
				DoubleToString(pMeter->HourData.RtdMin,pMeter->para->Decimals,MinBuf);
			}else{
				snprintf(MinBuf,sizeof(MinBuf)-1,"-");
			}
			/*平均值*/
			if(pMeter->para->AvgFlag){
				DoubleToString(double_div_uint(pMeter->HourData.RtdSum, (unsigned int)pMeter->HourData.CNT),pMeter->para->Decimals,AvgBuf);
			}else{
				snprintf(AvgBuf,sizeof(AvgBuf)-1,"-");
			}
			snprintf(sql,sizeof(sql)-1,"insert into %s values (%4d-%02d-%02d %02d:00:00,%s,%s,%s,%s);",TableName,\
										tm->tm_year + 1990, tm->tm_mon + 1,tm->tm_mday,tm->tm_hour,\
										MaxBuf,
										MinBuf,
										AvgBuf,
										CouBuf);
			tinz_db_exec(&scy_data,sql);
		} 
	}
}

static void day_data_proc(time_t sec){
	char 	TableName[TABLE_NAME_LEN];
	char 	sql[SQL_LEN];
	char 	CouBuf[DOUBLE_STRING_LEN];
	char 	MaxBuf[DOUBLE_STRING_LEN];
	char 	MinBuf[DOUBLE_STRING_LEN];
	char 	AvgBuf[DOUBLE_STRING_LEN];
	int		iLoop;
	struct tm 	*tm;
	pstMeter 	pMeter;
	double		Cou;
	
	tm = localtime(&sec);
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		pMeter = &Meter[iLoop];
		if(pMeter->para->isValid){

			snprintf(TableName,sizeof(TableName)-1,"Day_%s",pMeter->para->Code);
			if(TINZ_OK != TableIsExist(&scy_data,TableName)){
	            CountDataTableCreate(&scy_data,TableName);
	        }
			/*增量*/
			if(pMeter->para->CouFlag){
				Cou = (pMeter->DayData.totalStart >= 0 && pMeter->DayData.totalEnd > pMeter->DayData.totalStart) ? pMeter->DayData.totalEnd - pMeter->DayData.totalStart : 0;
				DoubleToString(Cou,pMeter->para->Decimals,CouBuf);
			}else{
				snprintf(CouBuf,sizeof(CouBuf)-1,"-");
			}
			/*最大值*/
			if(pMeter->para->MaxFlag){
				DoubleToString(pMeter->DayData.RtdMax ,pMeter->para->Decimals,MaxBuf);
			}else{
				snprintf(MaxBuf,sizeof(MaxBuf)-1,"-");
			}
			/*最小值*/
			if(pMeter->para->MinFlag){
				DoubleToString(pMeter->DayData.RtdMin,pMeter->para->Decimals,MinBuf);
			}else{
				snprintf(MinBuf,sizeof(MinBuf)-1,"-");
			}
			/*平均值*/
			if(pMeter->para->AvgFlag){
				DoubleToString(double_div_uint(pMeter->DayData.RtdSum, (unsigned int)pMeter->DayData.CNT),pMeter->para->Decimals,AvgBuf);
			}else{
				snprintf(AvgBuf,sizeof(AvgBuf)-1,"-");
			}
			snprintf(sql,sizeof(sql)-1,"insert into %s values (%4d-%02d-%02d 00:00:00,%s,%s,%s,%s);",TableName,\
										tm->tm_year + 1990, tm->tm_mon + 1,tm->tm_mday,\
										MaxBuf,
										MinBuf,
										AvgBuf,
										CouBuf);
			tinz_db_exec(&scy_data,sql);
		} 
	}
}


static void min_data_calc(){
	int iLoop;
	pstMeter pMeter;
	for(iLoop=0; iLoop < METER_CNT; iLoop++){
		pMeter = &Meter[iLoop];
		if(pMeter->para->isValid && pMeter->RtdData.flag){
			pMeter->MinData.RtdMin = (pMeter->MinData.RtdMin > pMeter->RtdData.Rtd || pMeter->MinData.RtdMin < 0) ? pMeter->RtdData.Rtd : pMeter->MinData.RtdMin;
			pMeter->MinData.RtdMax = pMeter->MinData.RtdMax < pMeter->RtdData.Rtd ? pMeter->RtdData.Rtd : pMeter->MinData.RtdMax;
			pMeter->MinData.RtdSum += pMeter->RtdData.Rtd;
			pMeter->MinData.CNT++;
			pMeter->MinData.totalEnd = pMeter->para->total;
		}
	}
}

static void hour_data_calc(){
	int iLoop;
	pstMeter pMeter;
	for(iLoop=0; iLoop < METER_CNT; iLoop++){
		pMeter = &Meter[iLoop];
		if(pMeter->para->isValid && pMeter->RtdData.flag){
			pMeter->HourData.RtdMin = (pMeter->HourData.RtdMin > pMeter->RtdData.Rtd || pMeter->HourData.RtdMin < 0) ? pMeter->RtdData.Rtd : pMeter->HourData.RtdMin;
			pMeter->HourData.RtdMax = pMeter->HourData.RtdMax < pMeter->RtdData.Rtd ? pMeter->RtdData.Rtd : pMeter->HourData.RtdMax;
			pMeter->HourData.RtdSum += pMeter->RtdData.Rtd;
			pMeter->HourData.CNT++;
			pMeter->MinData.totalEnd = pMeter->para->total;
		}
	}

}

static void day_data_calc(){
	int iLoop;
	pstMeter pMeter;
	for(iLoop=0; iLoop < METER_CNT; iLoop++){
		pMeter = &Meter[iLoop];
		if(pMeter->para->isValid && pMeter->RtdData.flag){
			pMeter->DayData.RtdMin = (pMeter->DayData.RtdMin > pMeter->RtdData.Rtd || pMeter->DayData.RtdMin < 0) ? pMeter->RtdData.Rtd : pMeter->DayData.RtdMin;
			pMeter->DayData.RtdMax = pMeter->DayData.RtdMax < pMeter->RtdData.Rtd ? pMeter->RtdData.Rtd : pMeter->DayData.RtdMax;
			pMeter->DayData.RtdSum += pMeter->RtdData.Rtd;
			pMeter->DayData.CNT++;
			pMeter->MinData.totalEnd = pMeter->para->total;
		}
	}
}
static void rtd_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].RtdData.flag 	= 0;
		Meter[iLoop].RtdData.Rtd 	= 0;
		Meter[iLoop].RtdData.total 	= 0;
	}
}

static void min_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].MinData.CNT	= 0;
		Meter[iLoop].MinData.RtdMax	= 0;
		Meter[iLoop].MinData.RtdMin = -1;
		Meter[iLoop].MinData.RtdSum = 0;
		Meter[iLoop].MinData.totalStart = Meter[iLoop].MinData.totalEnd;
	}
}

static void hour_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].HourData.CNT		= 0;
		Meter[iLoop].HourData.RtdMax	= 0;
		Meter[iLoop].HourData.RtdMin 	= -1;
		Meter[iLoop].HourData.RtdSum 	= 0;
		Meter[iLoop].HourData.totalStart = Meter[iLoop].HourData.totalEnd;
	}
}

static void day_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].DayData.CNT	= 0;
		Meter[iLoop].DayData.RtdMax	= 0;
		Meter[iLoop].DayData.RtdMin = -1;
		Meter[iLoop].DayData.RtdSum = 0;
		Meter[iLoop].DayData.totalStart = Meter[iLoop].DayData.totalEnd;
	}
}

static void meter_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].para = (pstMeterPara)&pgPara->MeterPara[iLoop];
	}
	rtd_data_init();
	min_data_init();
	hour_data_init();
	day_data_init();
}

int main(int argc, char* argv[])
{
	time_t 	old_sec,old_min_sec,old_hour_sec,old_day_sec,timestamp;   
	struct _msg *pmsg_dataproc_1;
	/*共享内存*/
	DEBUG_PRINT_INFO(gPrintLevel, "getParaShm start\n");
	pgPara = (pstPara)getParaShm();
	DEBUG_PRINT_INFO(gPrintLevel, "MN[%s]\n",pgPara->GeneralPara.MN);
	/*消息队列*/
	DEBUG_PRINT_INFO(gPrintLevel, "prepareMsg start\n");
	pmsg_dataproc_1 = (struct _msg*)malloc(sizeof(struct _msg));
	memset(pmsg_dataproc_1,0,sizeof(struct _msg));
	if(TINZ_ERROR == prepareMsg(MSG_PATH_DATAPROC_TO_SQLITE,MSG_NAME_DATAPROC_TO_SQLITE, MSG_ID_DATAPROC_TO_SQLITE, pmsg_dataproc_1)){
		exit(0);
	}
	/*数据库*/
	DEBUG_PRINT_INFO(gPrintLevel, "open [%s]\n",SCY_DATA);
	snprintf(scy_data.name,sizeof(scy_data.name)-1,SCY_DATA);
	if(TINZ_ERROR == tinz_db_open(&scy_data)){
		exit(0);	
	}
	/*数据初始化*/
	DEBUG_PRINT_INFO(gPrintLevel, "data init\n");
	meter_data_init();
	/*等待整分钟*/
	do{
		time(&timestamp);
		usleep(100000);
	}while(timestamp%60 != 0);
	DEBUG_PRINT_INFO(gPrintLevel, "data proc start\n");
	old_sec = timestamp;
	old_min_sec = timestamp;
   	while(1){
		/*实时数据入库*/
		if(timestamp > old_sec + pgPara->GeneralPara.RtdInterval){
			rtd_data_proc(old_sec);	
			min_data_calc();
			hour_data_calc();
			day_data_calc();
			rtd_data_init();
			old_sec = timestamp;
		}
		/*分钟数据*/
		if(timestamp > old_min_sec + pgPara->GeneralPara.MinInterval){
			min_data_proc(old_min_sec);
			min_data_init();
			old_min_sec = timestamp;
		}
		/*小时数据*/
		if(timestamp/(60*60) >= old_hour_sec/(60*60) + 1){
			hour_data_proc(old_hour_sec);
			hour_data_init();
			old_hour_sec = timestamp;
		}
		/*天数据*/
		if(timestamp/(24*60*60) >= old_day_sec/(24*60*60) + 1){
			day_data_proc(old_day_sec);
			day_data_init();
			old_day_sec = timestamp;
		}
		/**/
		MsgRcv(pmsg_dataproc_1, 0);
		if(pmsg_dataproc_1->msgbuf.mtype > 0){
			tinz_db_exec(&scy_data, pmsg_dataproc_1->msgbuf.data);
		}
		usleep(1000);
		time(&timestamp);
	}
	tinz_db_close(&scy_data);
	return 0;
}


