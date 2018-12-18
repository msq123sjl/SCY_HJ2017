#ifndef __PROTOCL_GB212__
#define __PROTOCL_GB212__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinz_base_def.h"
#include "nginx_helper.h"

#define MAX_TCPDATA_LEN 1037
#define MIN_TCPDATA_LEN 80

#define QN_LEN	17

#define RESULT_SUCCESS 1
#define RESULT_FAILED  2
#define RESULT_CONDITION_ERR  3
#define RESULT_NODATA  100

#define REQUEST_READY    1
#define REQUEST_REFUSED  2
#define REQUEST_CODE_ERR 3

#define CN_SendTime         1011
//#define CN_SendAlarmTarget  1031
//#define CN_SendReportTime   1041
#define CN_SendRtdInterval  1061
#define CN_SendMinsInterval  1063
#define CN_SendRtdData      2011
#define CN_SendStatus       2021
#define CN_SendDayData      2031
#define CN_SendRunTimeData   2041
#define CN_SendHourData     2061
#define CN_SendMinsData     2051
#define CN_SendBootTime     2081
//#define CN_SendAlarmData    2071
//#define CN_SendAlarmEvent   2072
#define CN_SendReservedSampleInfo 3015
#define CN_SendSampleTime    3017    //上传采样时间周期
#define CN_SendSampleOverTime 3018   //上传出样时间
#define CN_SendMN            3019    //上传设备唯一标识
#define CN_SendInfo          3020    //上传现场机信息

#define CN_Set_OverTime_ReCount 1000
//#define CN_SetAlarmTime     1001
#define CN_GetTime          1011
#define CN_SetTime          1012
//#define CN_GetAlarmValue    1021
//#define CN_SetAlarmValue    1022
//#define CN_GetAlarmTarget   1031
//#define CN_SetAlarmTarget   1032
//#define CN_GetReportTime    1041
//#define CN_SetReportTime    1042
#define CN_GetRtdInterval   1061
#define CN_SetRtdInterval   1062
#define CN_GetMinsInterval  1063
#define CN_SetMinsInterval  1064
#define CN_SetPW            1072
#define CN_GetRtdData       2011
#define CN_StopRtdData      2012
#define CN_GetStatus        2021
#define CN_StopStatus       2022
#define CN_GetDayData       2031
#define CN_GetRunTimeData   2041
#define CN_GetMinsData      2051
#define CN_GetHourData      2061
//#define CN_GetAlarmData
#define CN_RequestRespond   9011
#define CN_ExeRespond    	9012
#define CN_NoticeRespond    9013
#define CN_DataRespond      9014

#define CN_Adjust           3011     //零点校准量程校准 用于上位机启动在线监控（监测）仪器仪表的零点校准和量程校准
#define CN_Sample           3012     //即时采样
#define CN_Control          3013     //启动清洗/反吹
#define CN_SampleMatch      3014    //比对采样 用于上位机启动在线监控（监测）仪器仪表比对采样
#define CN_ReservedSample   3015    //超标留样 用于上位机启动在线监控（监测）仪器仪表留样
#define CN_SetSampleTime    3016    //设置采样时间周期
#define CN_GetSampleTime    3017    //提取采样时间周期
#define CN_GetSampleOverTime 3018   //提取出样时间
#define CN_GetMN            3019    //提取设备唯一标识
#define CN_GetInfo          3020    //提取现场机信息
#define CN_SetPara          3021    //设置现场机参数
#define CN_RST              3999


typedef struct {
	uint8_t   	MNFlag;
	int		   	cn;
	int		   	st;
	ngx_str_t	qn;
	ngx_str_t	pw;
	
	uint8_t     flag;    	//bit[0] 1:应答，0:不应答 
							//bit[1]：是否有数据序号；1-数据包中包含包序号和总包号两部分,0-数据包中不包含包序号和总包号两部分
	uint8_t		pnum;		//总包数
	uint8_t     pno;		//当前数据包包号
							
	int8_t		ReCount;
	int			WarnTime;	//取值范围为 0-99999，单位为秒
	int			OverTime;	//取值范围为 0-99999，单位为秒
	int			ReportTime; //日数据上报时间信息 例如0100，表示1点整，在每天制定时间上报上一天数据
	int			RtdInterval;//实时采样数据上报间隔 例如30，以秒为单位。包括实时污染数据和设备状态
	ngx_str_t	CardNo;
	ngx_str_t	PolId;
	ngx_str_t	SystemTime;
	ngx_str_t	BeginTime;
	ngx_str_t	EndTime;
	ngx_str_t   AlarmTarget;
      
} ngx_ulog_url_t;

int messageProc(char *str,pstSerialPara com,TcpClientDev *tcp);


#endif
