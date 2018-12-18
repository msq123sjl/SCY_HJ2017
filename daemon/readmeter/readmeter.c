#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>

#include "readmeter.h"
#include "tinz_common_helper.h"
#include "tinz_pub_serial.h"
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "protocol.h"


pstPara pgPara;
int gPrintLevel = 5;
//与设备进行通讯
void MessageFromCom(int port)
{
	int		iLoop;
    uint16_t  Address;
    for(iLoop = 0; iLoop < METER_CNT; iLoop++){
        if(pgPara->MeterPara[iLoop].isValid \
			&& 1 == pgPara->MeterPara[iLoop].UseChannelType\
			&& port + 2 == pgPara->MeterPara[iLoop].UseChannel){
            Address 	= pgPara->MeterPara[iLoop].Address;
			switch(pgPara->MeterPara[iLoop].Protocol)//通讯协议
			{
#if 0 
				case 1://天泽VOCs
				        Protocol_2(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 2://声级计
				        Protocol_3(port,Decimals,Name,Code,Unit);
				        break;
				case 3://风速
				         Protocol_4(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 4://风向
				        Protocol_5(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 5://ES-642浓度
				        Protocol_6(port,Decimals,Name,Code,Unit);
				        break;
				case 6://ES-642温度
				        Protocol_7(port,Decimals,Name,Code,Unit);
				        break;
				case 7://ES-642湿度
				        Protocol_8(port,Decimals,Name,Code,Unit);
				        break;
				case 8://气压
				        Protocol_9(port,Decimals,Name,Code,Unit);
				        break;
				case 9://经度
				        Protocol_10(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 10://纬度
				        Protocol_11(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 11://瑾熙流量计
				        Protocol_12(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 12://光华流量计
				        Protocol_13(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 13://TOC-4100CJ
				        Protocol_14(port,Address,Decimals,Name,Code,Unit);
				        break;
#endif
				case 14://承德流量计
						Protocol_15(port,Address,&pgPara->MeterPara[iLoop]);
				        //Protocol_15(port,Address,Decimals,Name,Code,Unit);
				        break;
#if 0
				case 15://SJFC-200
				        Protocol_16(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 16://天泽温度
				        Protocol_17(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 17://天泽湿度
				        Protocol_18(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 18://天泽气压
				        Protocol_19(port,Address,Decimals,Name,Code,Unit);
				        break;
				case 19://光华流量计-总量
				        Protocol_20(port,Address,Decimals,Name,Code,Unit);
				    break;
				case 20://PH-P206
				    Protocol_21(port,Address,Decimals,Name,Code,Unit);
				break;
				case 21://明渠流量计
				    Protocol_22(port,Address,Decimals,Name,Code,Unit);
				break;
#endif
				case 22://电导率
				    Protocol_23(port,Address,&pgPara->MeterPara[iLoop]);
				break;
#if 0
				case 23://微兰COD
				    Protocol_24(port,Address,Decimals,Name,Code,Unit,alarm_max);
				break;
				case 24://雨水刷卡设备
				    Protocol_25(port);
				break;
				case 25://哈希COD
				    Protocol_26(port,Address,Decimals,Name,Code,Unit);
				break;
				case 26://哈希氨氮
				    Protocol_27(port,Address,Decimals,Name,Code,Unit);
				break;
				case 27://KS-3200采样仪
				        Protocol_28();
				    break;
				case 28://ABB流量计
				    Protocol_29(port,Address,Decimals,Name,Code,Unit);
				    break;
				case 29://雨水采样仪
				    Protocol_30();
				    break;
				case 30://南控液位计            
				    Protocol_31(port,Address,Decimals,Name,Code,Unit,alarm_min);
				    break;
				case 31://CE9628JM流量计
				     Protocol_32(port,Address,Decimals,Name,Code,Unit);
				    break;
#endif
				default: break;
			}
			usleep(pgPara->SerialPara[port].Interval*1000);
		}
    }
}

void uart_start(void *arg)
{   
	int *port = (int*)arg;
    while(1)
    {
        MessageFromCom(*port);
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
	int iLoop;
	int COM3ToServerOpen = 0;
	pthread_t uart_thread_id[SERIAL_CNT];
    pgPara = (pstPara)getParaShm();
	
	for(iLoop = 0; iLoop < SERIAL_CNT; iLoop++){
		uart_open(&pgPara->SerialPara[iLoop]);
		uart_init(&pgPara->SerialPara[iLoop]);
		if(1 == iLoop && pgPara->SerialPara[iLoop].isServerOpen){
			COM3ToServerOpen = 1;
			continue;
		}
		if(pthread_create(&uart_thread_id[iLoop],NULL,(void *)(&uart_start),(void *)(&iLoop)) == -1)
		{
			DEBUG_PRINT_INFO(gPrintLevel,"pthread_create error!\n");
		}
	}
	for(iLoop = 0; iLoop < SERIAL_CNT; iLoop++){
		if(1 == iLoop && 1 == COM3ToServerOpen){continue;}
		pthread_join(uart_thread_id[iLoop], NULL);
	}

	return 0;
}


