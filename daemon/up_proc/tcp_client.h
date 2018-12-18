#ifndef __TCP_CLIENT__
#define __TCP_CLIENT__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinz_base_def.h"

#define QT_UNIX_DOMAIN "/mnt/nandflash/tmp/fortune"
#define RECVBUF_LEN 2048
#define SENDBUF_LEN 2048

/*typedef struct __TCP_PARA
{
    char    		ServerOpen;
    char    		ServerIP[16];
    unsigned short	ServerPort;
    char 			isConnected;

}Tcp_para;*/

typedef void (*packet_send_cb)(int dev_fd, char* pSendBuf);

typedef struct _TcpClientDev{
	int 				dev_fd;
	pthread_t 			thread_id;
	pstSitePara			tcplink; 
	packet_send_cb		packet_send_handle;
/*	int chan_index;
	int chan_id;	
	stPortPara      local_port;
	stHostpara	host_addr;
	void* 		helper;
	unsigned char useflag;
	unsigned char cascade;
	int prot_id;
	
	int devfd;
	int confd;
	int status;
	int timeout;
	int maxsendlen;

	int timecounter;
	time_t frameReceiveTime;

	unsigned char* recvbuf;
	int dealPtr;
	int recvPtr;
	
	void* pPrivateData;	
	
	int (*open) (struct _ChanDev* pdev);
	int (*isopen) (struct _ChanDev* pdev);
	int (*timer) (struct _ChanDev* pdev);
	int (*read) (struct _ChanDev* pdev,unsigned char * buf,int maxlen);
	int (*write) (struct _ChanDev* pdev,unsigned char * buf,int maxlen);
	int (*ready) (struct _ChanDev* pdev,int flag);
	int (*close) (struct _ChanDev* pdev);
	int (*release) (struct _ChanDev* pdev);
	int (*prot_release_notify) (struct _ChanDev* pdev);
	time_t time;

	int threadcreated;
	pthread_t thread; 
	pthread_mutex_t thread_mutex;*/
} TcpClientDev;

typedef struct _QtTcpClientDev{
	
	int 				dev_fd;
	pthread_t 			thread_id;
/*	int chan_index;
	int chan_id;	
	stPortPara      local_port;
	stHostpara	host_addr;
	void* 		helper;
	unsigned char useflag;
	unsigned char cascade;
	int prot_id;
	
	int devfd;
	int confd;
	int status;
	int timeout;
	int maxsendlen;

	int timecounter;
	time_t frameReceiveTime;

	unsigned char* recvbuf;
	int dealPtr;
	int recvPtr;
	
	void* pPrivateData;	
	
	int (*open) (struct _ChanDev* pdev);
	int (*isopen) (struct _ChanDev* pdev);
	int (*timer) (struct _ChanDev* pdev);
	int (*read) (struct _ChanDev* pdev,unsigned char * buf,int maxlen);
	int (*write) (struct _ChanDev* pdev,unsigned char * buf,int maxlen);
	int (*ready) (struct _ChanDev* pdev,int flag);
	int (*close) (struct _ChanDev* pdev);
	int (*release) (struct _ChanDev* pdev);
	int (*prot_release_notify) (struct _ChanDev* pdev);
	time_t time;

	int threadcreated;
	pthread_t thread; 
	pthread_mutex_t thread_mutex;*/
} QtTcpClientDev;

int tcpclient_open(TcpClientDev *pdev);
int qt_tcpclient_open(QtTcpClientDev *pdev);
#endif