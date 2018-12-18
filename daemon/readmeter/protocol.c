 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include "readmeter.h"
#include "tinz_common_helper.h"
#include "tinz_pub_serial.h"
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "protocol.h"

extern int gPrintLevel;
extern pstPara pgPara;

static int find_start_and_end(u_char *readbuf,int nread,u_char start,u_char end,int *start_index,int *end_index,int datalen){
	int iLoop;
	for(iLoop = 0; iLoop < nread; iLoop++){
		if(start == readbuf[iLoop]){
			if(nread - iLoop >= datalen){
				if(end == readbuf[iLoop + datalen - 1]){
					*start_index = iLoop;
					*end_index 	= iLoop + datalen - 1;
					return 1;
				}
			}else{
				return 0;
			}
		}
	}
	return 0;
}
#if 0
//天泽VOCs
void Protocol_2(int port,int meter,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    u_char sendbuf[8];
    u_char readbuf[UART_READ_LEN];
    uint16_t check;
    size_t nread;
    
    sendbuf[0] = pgPara->MeterPara[meter].Address;
    sendbuf[1] = 0x03;
    sendbuf[2] = 0x00;
    sendbuf[3] = 0x00;
    sendbuf[4] = 0x00;
    sendbuf[5] = 0x10;
    check = CRC16_Modbus(sendbuf,6);
    sendbuf[6] = (u_char)(check & 0xff);
    sendbuf[7] = (u_char)((check>>8) & 0xff);
    uart_write(&pgPara->SerialPara[port],sendbuf,sizeof(sendbuf));
    usleep(pgPara->SerialPara[port].TimeOut * 1000);

    nread=read(pgPara->SerialPara[port].Devfd,readbuf,UART_READ_LEN);
    
    double rtd=0;
    QString flag="D";
    uchar s[4];
    float f1,f2;
    QByteArray readbuf;
    QByteArray sendbuf;

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x10;
    int check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    if(readbuf.length()>=37){
        qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
        s[0]=readbuf[6];
        s[1]=readbuf[5];
        s[2]=readbuf[4];
        s[3]=readbuf[3];
        f1=*(float *)s;//采集浓度,低字节前

        s[0]=readbuf[34];
        s[1]=readbuf[33];
        s[2]=readbuf[32];
        s[3]=readbuf[31];
        f2=*(float *)s;//气体系数
        flag='N';
        rtd=f1*f2;
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}
#endif
//承德流量计
void Protocol_15(int port,int Address,pstMeterPara pMeterPara)
{
	u_char 	Rtd_buf[4];
	u_char 	sendbuf[14];
    u_char 	readbuf[UART_READ_LEN];
    u_char 	check;
    size_t 	nread;
	int 	iLoop,start_index,end_index;
	float	Rtd;
	double  total=0;
	/*瞬时流量*/
    sendbuf[0]=0x68;
    sendbuf[1]=(u_char)(Address & 0xff);
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x00;
    sendbuf[6]=0x00;
    sendbuf[7]=0x68;
    sendbuf[8]=0x01;
    sendbuf[9]=0x02;
    sendbuf[10]=0x18;
    sendbuf[11]=0xC0;
    check=0;
    for(iLoop=0;iLoop<12;iLoop++)
    {
        check+=sendbuf[iLoop];
    }
    sendbuf[12]=(u_char)check;
    sendbuf[13]=0x16;
	uart_write(&pgPara->SerialPara[port],(char*)sendbuf,sizeof(sendbuf));
	DEBUG_PRINT_INFO(gPrintLevel,"FLOWMETER_CD_RTD COM[%d] SEND[%s]",port+2,sendbuf);
    usleep(pgPara->SerialPara[port].TimeOut * 1000);

	/*接收解析指令*/
    nread=read(pgPara->SerialPara[port].Devfd,readbuf,UART_READ_LEN);
	DEBUG_PRINT_INFO(gPrintLevel,"FLOWMETER_CD_RTD COM[%d] RECV_LEN[%d] RECV[%s]",port+2,nread,readbuf);
	if(nread >= FLOWMETER_CD_RTD_LEN){
		/*查找起始、结束位置*/
		if(1 == find_start_and_end(readbuf, nread, 0x68, 0x16, &start_index, &end_index, FLOWMETER_CD_RTD_LEN)){
			/*匹配地址*/
			if((Address & 0xff) == readbuf[start_index + 1]){
				/*标识*/
				if(0x68 == readbuf[start_index + 7] && 0x81 == readbuf[start_index + 8] \
					&& sendbuf[10] == readbuf[start_index + 10] && sendbuf[11] == readbuf[start_index + 11]){
					/*校验*/
					check=0;
				    for(iLoop = start_index; iLoop < end_index - 1; iLoop++)
				    {
				        check += readbuf[iLoop];
				    }
					if(check == readbuf[end_index - 1]){
						Rtd_buf[0] = readbuf[start_index + 12];
						Rtd_buf[1] = readbuf[start_index + 13];
						Rtd_buf[2] = readbuf[start_index + 14];
						Rtd_buf[3] = readbuf[start_index + 15];
						Rtd=*(float *)Rtd_buf;
        				//flag="N";
					}else{
						DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_RTD RECV[%s] XORValid ERR!!!",readbuf);
				}
				}else{
					DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_RTD RECV[%s] MARK ERR!!!",readbuf);
				}	
			}else{
				DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_RTD RECV[%s] parse address ERR!!!",readbuf);
			}
		}else{
			DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_RTD RECV[%s] parse start or end ERR!!!",readbuf);
		}
	}else{
		DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_RTD RECV bytes[%d] ERR",nread);
	}
	/*总量*/
    sendbuf[0]=0x68;
    sendbuf[1]=(u_char)(Address & 0xff);
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x00;
    sendbuf[6]=0x00;
    sendbuf[7]=0x68;
    sendbuf[8]=0x01;
    sendbuf[9]=0x02;
    sendbuf[10]=0x03;
    sendbuf[11]=0xC0;
    check=0;
    for(iLoop=0;iLoop<12;iLoop++)
    {
        check+=sendbuf[iLoop];
    }
    sendbuf[12]=(u_char)check;
    sendbuf[13]=0x16;
	uart_write(&pgPara->SerialPara[port],(char*)sendbuf,sizeof(sendbuf));	
	DEBUG_PRINT_INFO(gPrintLevel,"FLOWMETER_CD_SUM COM[%d] SEND[%s]",port+2,sendbuf);
    usleep(pgPara->SerialPara[port].TimeOut * 1000);

	/*接收解析指令*/
    nread=read(pgPara->SerialPara[port].Devfd,readbuf,UART_READ_LEN);
	DEBUG_PRINT_INFO(gPrintLevel,"FLOWMETER_CD_SUM COM[%d] RECV_LEN[%d] RECV[%s]",port+2,nread,readbuf);
	if(nread >= FLOWMETER_CD_SUM_LEN){
		/*查找起始、结束位置*/
		if(1 == find_start_and_end(readbuf, nread, 0x68, 0x16, &start_index, &end_index, FLOWMETER_CD_SUM_LEN)){
			/*匹配地址*/
			if((Address & 0xff) == readbuf[start_index + 1]){
				/*标识*/
				if(0x68 == readbuf[start_index + 7] && 0x81 == readbuf[start_index + 8] \
					&& sendbuf[10] == readbuf[start_index + 10] && sendbuf[11] == readbuf[start_index + 11]){
					/*校验*/
					check=0;
				    for(iLoop = start_index; iLoop < end_index - 1; iLoop++)
				    {
				        check += readbuf[iLoop];
				    }
					if(check == readbuf[end_index - 1]){
						total = BCD2BIN(readbuf[start_index + 19]) * 100000000000LL \
						 		+ BCD2BIN(readbuf[start_index + 18]) * 1000000000 \
								+ BCD2BIN(readbuf[start_index + 17]) * 10000000 \
								+ BCD2BIN(readbuf[start_index + 16]) * 100000 \
								+ BCD2BIN(readbuf[start_index + 15]) * 1000 \
								+ BCD2BIN(readbuf[start_index + 14]) * 10 \
								+ BCD2BIN(readbuf[start_index + 13]) * 0.1 \
								+ BCD2BIN(readbuf[start_index + 12]) * 0.001;
						
        				//flag="N";
					}else{
						DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_SUM RECV[%s] XORValid ERR!!!",readbuf);
					}
				}else{
					DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_SUM RECV[%s] MARK ERR!!!",readbuf);
				}	
			}else{
				DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_SUM RECV[%s] parse address ERR!!!",readbuf);
			}
		}else{
			DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_SUM RECV[%s] parse start or end ERR!!!",readbuf);
		}
	}else{
		DEBUG_PRINT_WARN(gPrintLevel,"FLOWMETER_CD_SUM RECV bytes[%d] ERR",nread);
	}
}

//C10电导率
void Protocol_23(int port,int Address,pstMeterPara pMeterPara)
{
	u_char Rtd_buf[4];
	u_char sendbuf[14];
    u_char readbuf[UART_READ_LEN];
    u_char check,dataValid;
    size_t nread;
	int iLoop,start_index,end_index;
	/*发送指令*/
	sendbuf[0]=0x40;
	sendbuf[1]=((Address >> 4) & 0x0f)+0x30;
	sendbuf[2]=(Address&0x0f)+0x30;
	sendbuf[3]='C';
	sendbuf[4]='8';
	sendbuf[5]='0';
	sendbuf[6]='0';
	sendbuf[7]='0';
	sendbuf[8]='0';
	sendbuf[9]='0';
	sendbuf[10]='8';
	check = XORValid((char*)(&sendbuf[1]), 10);
	sendbuf[11] = HEXTOASCII(((check  >> 4) & 0x0F));		//取高位数；
	sendbuf[12] = HEXTOASCII((check & 0x0F));		//取高位数；
	sendbuf[13]=0x0D;
	uart_write(&pgPara->SerialPara[port],(char*)sendbuf,sizeof(sendbuf));	
	DEBUG_PRINT_INFO(gPrintLevel,"C10 COM[%d] SEND[%s]",port+2,sendbuf);
    usleep(pgPara->SerialPara[port].TimeOut * 1000);
	
	/*接收解析指令*/
    nread=read(pgPara->SerialPara[port].Devfd,readbuf,UART_READ_LEN);
	DEBUG_PRINT_INFO(gPrintLevel,"C10 COM[%d] RECV_LEN[%d] RECV[%s]",port+2,nread,readbuf);
	if(nread >= C10_ADDR0000_LEN){
		/*查找起始、结束位置*/
		if(1 == find_start_and_end(readbuf, nread, 0x40, 0x0D, &start_index, &end_index, C10_ADDR0000_LEN)){
			/*匹配地址*/
			if(Address == (((readbuf[start_index+1] - 0x30) << 4) + (readbuf[start_index+2] - 0x30))){
				/*校验*/
				check = XORValid((char*)(&readbuf[start_index + 1]), C10_ADDR0000_LEN - 4);
				readbuf[end_index - 1] = toupper(readbuf[end_index - 1]);
				readbuf[end_index - 2] = toupper(readbuf[end_index - 2]);
				if(ASCIITOHEX(readbuf[end_index - 2], readbuf[end_index - 1]) == check){
					/*合法性检查 小写转成大写*/
					dataValid = 1;
					for(iLoop = start_index + 5; iLoop <= start_index + 12; iLoop++){
						readbuf[iLoop] = toupper(readbuf[iLoop]);
						if(0 == HEXCHECK(readbuf[iLoop])){
							DEBUG_PRINT_WARN(gPrintLevel,"C10 RECV[%s] data ERR!!!",readbuf);
							dataValid = 0;
							break;
						}
					}
					if(dataValid){
						Rtd_buf[0] = ASCIITOHEX(readbuf[start_index+11],readbuf[start_index+12]);
						Rtd_buf[1] = ASCIITOHEX(readbuf[start_index+9],readbuf[start_index+10]);
						Rtd_buf[2] = ASCIITOHEX(readbuf[start_index+7],readbuf[start_index+8]);
						Rtd_buf[3] = ASCIITOHEX(readbuf[start_index+5],readbuf[start_index+6]);
						pMeterPara->Rtd = *(float*)Rtd_buf;
						//flag='N';
		            	//CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
	            	}
				}else{
					DEBUG_PRINT_WARN(gPrintLevel,"C10 RECV[%s] XORValid ERR!!!",readbuf);
				}
			}else{
				DEBUG_PRINT_WARN(gPrintLevel,"C10 RECV[%s] parse address ERR!!!",readbuf);
			}
		}else{
			DEBUG_PRINT_WARN(gPrintLevel,"C10 RECV[%s] parse start or end ERR!!!",readbuf);
		}
	
	}else{
		DEBUG_PRINT_WARN(gPrintLevel,"C10 RECV bytes[%d] ERR",nread);
	}
    sleep(2); //为什么要sleep
}

