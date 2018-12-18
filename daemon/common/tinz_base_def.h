#ifndef __TINZ_BASE_DEF__
#define __TINZ_BASE_DEF__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TINZ_ERROR -1
#define TINZ_OK		1

#define MN_LEN 25
#define PW_LEN 6


#define METER_NAME_LEN  20
#define CODE_LEN        6
#define UNIT_LEN        6

#define USER_NAME_LEN   10
#define USER_PWD_LEN    20

#define UART_DEVNAME_LEN 12

#define METER_CNT   32
#define SERIAL_CNT  6
#define SITE_CNT    4
#define USER_CNT    8

#define 	MAX_FILENAME_SIZE 	256

#define BIN4BCD(val) ((((val)/1000)<<12)+((((val)%1000)/100)<<8)+((((val)%1000)%100)/10<<4)+(val)%10)
#define BIN2BCD(val) ((((val)/10)<<4) + (val)%10)
#define BCD4BIN(val) (((val)&15)+(((val)>>4)&15)*10+(((val)>>8)&15)*100+(((val)>>12)&15)*1000)
#define BCD2BIN(val) (((val)&15) + ((val)>>4)*10)


typedef struct _GeneralPara
{
    u_char    MN[MN_LEN]; 
	u_char    PW[PW_LEN];
    uint16_t  RtdInterval;            //实时数据间隔（s）
    uint8_t   MinInterval;            //分钟数据间隔（min）
    uint8_t   CatchmentTime;          //集水时间（min）
    uint8_t   COD_CollectInterval;    //COD采集数据间隔（min）
    uint8_t   OverTime;               //上报周期（s）
    uint8_t   ReCount;                //超时重发次数
    uint16_t  AlarmTime;              //超限报警时间
    uint8_t   StType;                 //污染源类型
    uint8_t   RespondOpen;            //上位机应答
}stGeneralPara,*pstGeneralPara;

typedef struct _MeterPara
{
    uint8_t		isValid;
    u_char		Name[METER_NAME_LEN];       //因子名称  , 如电导率
    u_char  	Code[CODE_LEN];             //上报代码      w01014
    u_char  	Unit[UNIT_LEN];             //单位，如m3/h
    uint8_t   	UseChannel:6;                 //通道号
    uint8_t   	UseChannelType:2; //通道类型  1:串口 2:模拟
    uint8_t   	Address;            //设备地址
    uint8_t   	Protocol;           //协议号
    uint8_t   	Signal;             //接入信号，AD 模拟电压转实际值所用
    float   	RangeUp;            //量程上限
    float   	RangeLow;           //量程下限
    float   	AlarmUp;            //报警上限
    float   	AlarmLow;           //报警下限
    uint8_t   	MaxFlag:1;          //最大值标志
    uint8_t   	MinFlag:1;          //最小值标志
    uint8_t   	AvgFlag:1;          //平均值标志
    uint8_t   	CouFlag:1;          //累积值标志
    uint8_t   	Decimals:4;         //小数位数 
    char		flag;
    float		Rtd;
	double		total;
}stMeterPara,*pstMeterPara;

typedef struct _SerialPara
{
    uint8_t   isServerOpen:1;         // 0 关闭 1 打开
    uint8_t   isRS485:1;              // 0 232 1 485
    u_char    DevName[UART_DEVNAME_LEN];       //串口名称
    uint16_t  BaudRate;       //串口波特率
    uint8_t   DataBits;       //串口数据位
    uint8_t   Parity;         //串口校验位       0 无校验 1 奇校验 2 偶校验
    uint8_t   StopBits;       //串口停止位    
    uint8_t   FlowCtrl;       //流控制 
//    int Protocol;       //串口协议索引
//    bool use;           //串口是否使用
    int     TimeOut;        //串口读取超时(ms)
    int     Interval;       //串口通讯周期(ms)
//    int HardwareAddress;//串口硬件地址
    int     Devfd;
}stSerialPara,*pstSerialPara;

typedef struct _IOPara
{
    volatile int    Out_drain_open;         //开排水阀
    volatile int    Out_drain_close;        //关排水阀
    volatile int    Out_catchment_open;     //开集水阀
    volatile int    Out_catchment_close;    //关集水阀
    volatile int    Out_reflux_control;     //回流泵控制

    volatile int     In_drain_open;         //输入检测排水阀开
    volatile int     In_drain_close;        //输入检测排水阀关
    volatile int     In_catchment_open;     //输入检测集水阀开
    volatile int     In_catchment_close;    //输入检测集水阀关
    volatile int     In_reflux_open;        //输入检测回流泵开
    volatile int     In_reflux_close;       //输入检测回流泵关
    volatile int     In_power;              //输入检测市电
}stIOPara,*pstIOPara;

typedef struct _SitePara
{   
    uint8_t   ServerOpen:1;   
    uint8_t   isConnected:1;  
    uint16_t  ServerPort; //服务器端口
    u_char  ServerIp[16];   //服务器IP地址
}stSitePara,*pstSitePara;

typedef struct _UserPara
{
    uint8_t   UserType;       //用户类型
    u_char  UserName[USER_NAME_LEN];
    u_char  UserPwd[USER_PWD_LEN];
}stUserPara,*pstUserPara;

typedef struct _Para
{
    stGeneralPara   GeneralPara; 
    stMeterPara     MeterPara[METER_CNT];
    stSerialPara    SerialPara[SERIAL_CNT];
    stIOPara        IOPara;
    stSitePara      SitePara[SITE_CNT]; 
    stUserPara      UserPara[USER_CNT];
}stPara,*pstPara;
#endif

