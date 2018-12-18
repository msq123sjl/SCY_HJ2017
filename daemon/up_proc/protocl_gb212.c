#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "tcp_client.h"
#include "tinz_common_helper.h"
#include "nginx_helper.h"
#include "protocl_gb212.h"

extern pstPara pgPara;
extern int gPrintLevel;

ngx_ulog_url_t ngx_ulog_url_args;/* = {
    0,0,
    ngx_string("未知"),
    ngx_null_string,
    ngx_string("未知"),
    ngx_null_string,
    ngx_null_string,
};*/
	
ngx_str_t  ngx_ulog_lang_unknown = ngx_string("未知");
ngx_str_t  ngx_ulog_null = ngx_null_string;

//请求应答
void RequestRespond(int QnRtn,ngx_ulog_url_t *url_args,pstSerialPara com,TcpClientDev *tcp)
{
	char buf[MAX_TCPDATA_LEN];
	int nLen;
	int CRC16;
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000QN=%-17.17s;ST=91;CN=9011;PW=%-6.6s;MN=%-24.24s;Flag=4;CP=&&QnRtn=%d&&",\
								url_args->qn.data,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,QnRtn);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 6 && nLen == strlen(buf)){
		nLen = nLen - 6;
		buf[2] = (nLen/1000)+'0';
    	buf[3] = (nLen%1000/100)+'0';
    	buf[4] = (nLen%100/10)+'0';
    	buf[5] = (nLen%10)+'0'; 
		CRC16 = CRC16_GB212(&buf[6], nLen);
		snprintf(&buf[nLen + 6],7,"%.4X\r\n",CRC16);
		if(com!=NULL){
        	//com->write(str.toAscii());
        }
    	if(tcp!=NULL){
        	tcp->packet_send_handle(tcp->dev_fd,buf);
		}
	}else{
		DEBUG_PRINT_WARN(gPrintLevel, "RequestRespond send nLen[%d] ignore!!!", nLen);
	}								
}
//操作返回操作执行结果
void ExecuteRespond(int ExeRtn,ngx_ulog_url_t *url_args,pstSerialPara com,TcpClientDev *tcp)
{
	char buf[MAX_TCPDATA_LEN];
	int nLen;
	int CRC16;
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000QN=%-17.17s;ST=91;CN=9012;PW=%-6.6s;MN=%-24.24s;Flag=4;CP=&&ExeRtn=%d&&",\
								url_args->qn.data,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,ExeRtn);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 6 && nLen == strlen(buf)){
		CRC16 = CRC16_GB212(&buf[6], nLen);
		snprintf(&buf[nLen + 6],7,"%.4X\r\n",CRC16);
	
		nLen = nLen - 6;
		buf[2] = (nLen/1000)+'0';
    	buf[3] = (nLen%1000/100)+'0';
    	buf[4] = (nLen%100/10)+'0';
    	buf[5] = (nLen%10)+'0'; 
		
		if(com!=NULL){
        	//com->write(str.toAscii());
        }
    	if(tcp!=NULL){
        	tcp->packet_send_handle(tcp->dev_fd,buf);
		}
	}else{
		DEBUG_PRINT_WARN(gPrintLevel, "ExecuteRespond send nLen[%d] ignore!!!", nLen);
	}
}
//发送现场时间
static int SendCurrentTime(ngx_ulog_url_t *url_args,pstSerialPara com,TcpClientDev *tcp)
{
	char buf[MAX_TCPDATA_LEN];
	int nLen;
	int CRC16;
	time_t      now;
    struct tm   *tblock;
	now = time(NULL);
    tblock = localtime( &now );
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000QN=%-17.17s;ST=%02d;CN=%04d;PW=%-6.6s;MN=%-24.24s;Flag=4;CP=&&SystemTime=%4d%02d%02d%02d%02d%02d&&",\
								url_args->qn.data,pgPara->GeneralPara.StType,url_args->cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,\
								tblock->tm_year + 1900,tblock->tm_mon + 1,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 6 && nLen == strlen(buf)){
		CRC16 = CRC16_GB212(&buf[6], nLen);
		snprintf(&buf[nLen + 6],7,"%.4X\r\n",CRC16);
	
		nLen = nLen - 6;
		buf[2] = (nLen/1000)+'0';
    	buf[3] = (nLen%1000/100)+'0';
    	buf[4] = (nLen%100/10)+'0';
    	buf[5] = (nLen%10)+'0'; 
		
		if(com!=NULL){
        	//com->write(str.toAscii());
        }
    	if(tcp!=NULL){
        	tcp->packet_send_handle(tcp->dev_fd,buf);
		}
	}else{
		DEBUG_PRINT_WARN(gPrintLevel, "ExecuteRespond send nLen[%d] ignore!!!", nLen);
		return TINZ_ERROR;
	}
	return TINZ_OK;
}

static inline int parse_url(char *str,ngx_ulog_url_t *url_args){
	/*
	包头 			字符 			2 			##
	数据段长度 		十进制 		4 			数据段的ASCII字符数 例如：长255，则写为“0255”
	数据段			字符 			0～1024 
	CRC校验		十六进制	 	4 			数据段的校验结果，例如4B30，如CRC校验错，即执行失败
	包尾   		字符 			2 			固定为<CR><LF>（回车、换行）
	*/
	char *pos;
	char *end = NULL;
	char flag,flag_arg;
	int  data_len;
	int  CRC16;
	ngx_str_t	name;
	ngx_str_t   value;
	/*报文长度校验*/
	if(strlen(str) < 12){
		DEBUG_PRINT_WARN(gPrintLevel, "GB212 LENGTH[%d]\n",strlen(str));
		return TINZ_ERROR;
	}
	/*查找报文头部*/
	pos = strstr(str,"##");
	if(NULL == pos){
		DEBUG_PRINT_WARN(gPrintLevel, "GB212 START ERR!!!\n");
		return TINZ_ERROR;
	}
	pos += 2;   //头
	data_len = (int)ngx_atoi((u_char*)pos, 4);
	pos += 4;    //数据段长度
	/*查找包尾，校验报文长度*/
	end = strstr(pos,"/r/n");
	if(NULL == end || end - pos != data_len + 4){  //+4 CRC字节数
		DEBUG_PRINT_WARN(gPrintLevel, "GB212 end ERR!!!\n");
		return TINZ_ERROR;
	}
	/*CRC校验*/
	if(NGX_ERROR == (CRC16 = ngx_hextoi((u_char*)(end - 4), 4))){
		DEBUG_PRINT_WARN(gPrintLevel, "GB212 CRC DATA ERR!!!\n");
		return TINZ_ERROR;
	}
	if(CRC16 != CRC16_GB212(pos, data_len)){
		DEBUG_PRINT_WARN(gPrintLevel, "GB212 CRC ERR!!!\n");
		return TINZ_ERROR;
	}
	end = end - 4;  //CRC
	for(;pos < end;pos++){
		if(*pos == '&'){
			continue;
		}
		name.data 	= (u_char*)pos;
		name.len	= 0;
		value = ngx_ulog_null;
		flag  = 1;
		for(;pos < end;pos++){
			if(flag){*pos = ngx_tolower(*pos);}
			if (*pos == '=') {
				flag = 0;
                name.len = pos - (char*)name.data;
                
                if (pos + 1 < end) {
                    value.data = (u_char*)(pos + 1);
                }
                else {
                    break;
                }
            }

            if (*pos == ';' || *pos == '&' || pos == end) {

                if (value.data != NULL) {
                    value.len = pos - (char*)value.data;
                }
                break;
            }
		}
		//存在字段但值为空
		if(value.len > 0 && name.len > 0){
            //判断获取的字符串是哪个参数
            flag_arg = 0;
            switch (name.len) {
				case 2:
					if(ngx_str2cmp(name.data, 'm', 'n')){
						if(value.len != MN_LEN - 1 || strcmp((char*)value.data,(char*)pgPara->GeneralPara.MN)){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 MN [%-14.14s][%s] ERR!!!\n",value.data,pgPara->GeneralPara.MN);
							return TINZ_ERROR;
						}
						url_args->MNFlag = 1;
						//[0-9A-Za-z]
						break;
					}
					if(ngx_str2cmp(name.data, 's', 't')){
						//[0-9]
						if((int)pgPara->GeneralPara.StType != (url_args->st = ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 ST [%-7.7s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
					}
					if(ngx_str2cmp(name.data, 'c', 'n')){
						//[0-9]
						if(NGX_ERROR == (url_args->cn = ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 CN [%-7.7s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
					}
					if(ngx_str2cmp(name.data, 'q', 'n')){
						if(value.len != sizeof("YYYYMMDDHHMMSSZZZ") || NGX_ERROR == ngx_isnumbers(value.data, value.len)){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 QN [%-17.17s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						url_args->qn = value;
						//[0-9A-Za-z]
						break;
					}
					if(ngx_str2cmp(name.data, 'p', 'w')){
						if(value.len != sizeof("111111")){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 PW [%-6.6s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						url_args->pw = value;
						//[0-9A-Za-z]
						break;
					}
					flag_arg = 1;
					break;
				case 3:
					if(ngx_str3cmp(name.data, 'p', 'n','0')){
						if(value.len > 0 || NGX_ERROR == (url_args->pno = (uint8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 pno [%-10.10s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;
				case 4:
					if(ngx_str4cmp(name.data, 'f', 'l','a','g')){
						if(value.len > 0 || NGX_ERROR == (url_args->flag = (uint8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 flag [%-10.10s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						//[0-9]
						break;
					}
					if(ngx_str4cmp(name.data, 'p', 'n','u','m')){
						if(value.len > 0 || NGX_ERROR == (url_args->pnum = (uint8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 pnum [%-10.10s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;
				case 5:
					if(ngx_str5cmp(name.data, 'p', 'o','l','i','d')){
						url_args->PolId = value;
						//[0-9A-Za-z]
						break;
					}

					flag_arg = 1;
					break;
				case 6:
					if(ngx_str6cmp(name.data, 'c', 'a','r','d','n','o')){
						url_args->CardNo = value;
						//[0-9A-Za-z]
						break;
					}
					flag_arg = 1;
					break;					
				case 7:
					if(ngx_str7cmp(name.data, 'e', 'n','d','t','i','m','e')){
						
						if(value.len != sizeof("YYYYMMDDHHMMSS") || NGX_ERROR == ngx_isnumbers(value.data, value.len)){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 EndTime [%-14.14s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						url_args->EndTime = value;
						//[0-9]
						break;
					}
					if(ngx_str7cmp(name.data, 'r', 'e','c','o','u','n','t')){
						if(!(value.len == 1 || value.len == 2) || NGX_ERROR == (url_args->ReCount = (int8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 recount [%-2.2s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;						
				case 8:
					if(ngx_str8cmp(name.data, 'w', 'a','r','n','t','i','m','e')){
						if((value.len < 1 || value.len > 5) || NGX_ERROR == (url_args->WarnTime  = ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 WarnTime [%-5.5s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						//[0-9]
						break;
					}
					if(ngx_str8cmp(name.data, 'o', 'v','e','r','t','i','m','e')){
						if((value.len < 1 || value.len > 5) || NGX_ERROR == (url_args->OverTime  = ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 OverTime [%-5.5s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}						
						//[0-9]
						break;
					}
					if(ngx_str8cmp(name.data, 'c', 'a','r','d','t','y','p','e')){
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;
				case 9:
					if(ngx_str9cmp(name.data, 'b', 'e','g','i','n','t','i','m','e')){
						if(value.len != sizeof("YYYYMMDDHHMMSS") || NGX_ERROR == ngx_isnumbers(value.data, value.len)){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 BeginTime [%-14.14s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						url_args->BeginTime = value;
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;	
				case 10:
					if(ngx_str10cmp(name.data, 's', 'y','s','t','e','m','t','i','m','e')){
						if(value.len != sizeof("YYYYMMDDHHMMSS") || NGX_ERROR == ngx_isnumbers(value.data, value.len)){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 SystemTime [%-14.14s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						url_args->SystemTime = value;
						//[0-9]
						break;
					}
					if(ngx_str10cmp(name.data, 'r', 'e','p','o','r','t','t','i','m','e')){
						if((value.len < 1 || value.len > 4) || NGX_ERROR == (url_args->ReportTime  = ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 ReportTime [%-4.4s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}	
						//[0-9]
						break;
					}	
					flag_arg = 1;
					break;
				case 11:
					if(ngx_str11cmp(name.data, 'a', 'l','a','r','m','t','a','r','g','e','t')){
						url_args->AlarmTarget = value;
						//[0-9]
						break;
					}
					if(ngx_str11cmp(name.data, 'r', 't','d','i','n','t','e','r','v','a','l')){
						if((value.len < 1 || value.len > 4) || NGX_ERROR == (url_args->RtdInterval  = ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 RtdInterval [%-4.4s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}	
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;	
				default:
					flag_arg = 1;
					break;
			}
			if(flag_arg){
				DEBUG_PRINT_WARN(gPrintLevel, "parse [%-20.20s]\n",name.data);
			}
		}
	}
	if(url_args->cn ==0 || url_args->qn.len == 0 || url_args->flag < 0){
		return TINZ_ERROR;
	}
	return TINZ_OK;
}

//收到平台信息并处理
int messageProc(char *str,pstSerialPara com,TcpClientDev *tcp)
{
	ngx_ulog_url_t url_args;
	if(TINZ_ERROR == parse_url(str, &url_args)){
		return TINZ_ERROR;
	}
	if(url_args.qn.len != QN_LEN \
		|| url_args.cn < 0 \
		|| 1 != url_args.MNFlag \
		|| url_args.st < 0 \
		|| url_args.flag < 0){
		return TINZ_ERROR;
	}

	switch(url_args.cn){
	/*初始化*/
		case CN_Set_OverTime_ReCount:
			if(url_args.flag & 0x01){
				RequestRespond(REQUEST_READY,&url_args, com, tcp);
			}
			if(url_args.OverTime < 0 || url_args.ReCount < 0){
				ExecuteRespond(RESULT_CONDITION_ERR, &url_args,com, tcp);
				return TINZ_ERROR;
			}
			pgPara->GeneralPara.OverTime = url_args.OverTime;
			pgPara->GeneralPara.ReCount  = url_args.ReCount;
			ExecuteRespond(RESULT_SUCCESS, &url_args, com, tcp);
			break;
	/*参数命令*/
		case CN_GetTime:
			if(url_args.flag & 0x01){
				if(url_args.PolId.len > 0){
					RequestRespond(REQUEST_READY,&url_args, com, tcp);
				}else{
					RequestRespond(REQUEST_REFUSED,&url_args, com, tcp);
				}
				if(TINZ_OK == SendCurrentTime(&url_args, com, tcp)){
					ExecuteRespond(RESULT_SUCCESS, &url_args,com, tcp);
				}
				else{
					ExecuteRespond(RESULT_FAILED, &url_args,com, tcp);
				}
			}
			break;
		case CN_SetTime:
			break;	
		case CN_GetRtdInterval:
			break;	
		case CN_SetRtdInterval:
			break;
		case CN_GetMinsInterval:
			break;	
		case CN_SetMinsInterval:
			break;
		case CN_SetPW:
			break;
	/*数据命令*/
		/*实时数据*/
		case CN_GetRtdData:
			break;	
		case CN_StopRtdData:
			break;
		/*设备状态*/
		case CN_GetStatus:
			break;
		case CN_StopStatus:
			break;	
		/*日数据*/
		case CN_GetDayData:
			break;
		case CN_GetRunTimeData:
			break;
		/*分钟数据*/
		case CN_GetMinsData:
			break;	
		/*小时数据*/
		case CN_GetHourData:
			break;
	/*控制命令*/
		case CN_Adjust:
			break;
		case CN_Sample:
			break;	
		case CN_Control:
			break;				
		case CN_SampleMatch:
			break;
		case CN_ReservedSample:
			break;	
		case CN_SetSampleTime:
			break;	
		case CN_GetSampleTime:
			break;
		case CN_GetSampleOverTime:
			break;	
		case CN_GetMN:
			break;
		case CN_GetInfo:
			break;
		case CN_SetPara:
			break;	
		case CN_RST:
			break;		
		default:
			if(url_args.flag & 0x01){
				RequestRespond(REQUEST_CODE_ERR,&url_args, com, tcp);
			}
			break;
	}
    return TINZ_OK;
}

