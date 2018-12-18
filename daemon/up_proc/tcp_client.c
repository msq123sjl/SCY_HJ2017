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
#include "protocl_gb212.h"

extern int gPrintLevel;

static int recv_message_from_qt_server(QtTcpClientDev* pdev, char* pRecvBuf){
	int iRecvLen;
	iRecvLen = recv(pdev->dev_fd, pRecvBuf, RECVBUF_LEN, 0);

	return iRecvLen;
}

static int send_message_to_qt_server(QtTcpClientDev* pdev, char* pSendBuf){
	int iSendLen = 0;
	iSendLen = send(pdev->dev_fd, pSendBuf, strlen(pSendBuf), 0);
	if(iSendLen > 0){
		printf("write Date Len to server (%d) : %s\n",iSendLen, pSendBuf);
	}else{
		printf("retry\n");
	}
	return iSendLen;
}

static void qt_tcpclient_thread(void * arg)
{
	int iRecvLen;
    QtTcpClientDev *pdev = (QtTcpClientDev *)arg;

	char* pRecvBuf = (char*)malloc(RECVBUF_LEN);
	printf("GUI Send msg to TDC server:\n");
	while(1){
		
		memset(pRecvBuf,0,RECVBUF_LEN);
		iRecvLen = recv_message_from_qt_server(pdev, pRecvBuf);
		if(iRecvLen > 0){
			DEBUG_PRINT_INFO(gPrintLevel,"receive message from server (%d) :%c:%s:\n", iRecvLen, pRecvBuf[0],pRecvBuf);
		}
		else{
			DEBUG_PRINT_INFO(gPrintLevel,"receive message from server error\n");
		}
	}
	//close(pdev->dev_fd);
	free(pRecvBuf);
}

int qt_tcpclient_open(QtTcpClientDev *pdev){
	int iRet = -1;
	struct sockaddr_un 	ServAddr;
	
	pdev->dev_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	DEBUG_PRINT_INFO(gPrintLevel,"== dev_fd = %d\n", pdev->dev_fd);
 
	if (pdev->dev_fd < 0)
	{
		DEBUG_PRINT_ERR(gPrintLevel,"cannot create [%s] communication socket",QT_UNIX_DOMAIN);
		return TINZ_ERROR;
	}
 	/*if(0 == access(QT_UNIX_DOMAIN,F_OK)){
		delete_file(QT_UNIX_DOMAIN);
	}*/
	ServAddr.sun_family = AF_UNIX;
	strncpy(ServAddr.sun_path, QT_UNIX_DOMAIN, sizeof(ServAddr.sun_path) - 1);
	iRet = connect(pdev->dev_fd, (struct sockaddr*)&ServAddr, sizeof(struct sockaddr));
	if(-1 == iRet)
	{
		DEBUG_PRINT_ERR(gPrintLevel,"cannot connect to the server[%s]",QT_UNIX_DOMAIN);
		close(pdev->dev_fd);
		return TINZ_ERROR;
	}
	if(pthread_create(&pdev->thread_id,NULL,(void *)(&qt_tcpclient_thread),(void *)(pdev)) == -1)
	{
		DEBUG_PRINT_INFO(gPrintLevel,"pthread_create error!\n");
	}
	return TINZ_OK;
}


/*************平台通信**********************/
static int recv_message_from_server(TcpClientDev* pdev, char* pRecvBuf){
	int iRecvLen;
	iRecvLen = recv(pdev->dev_fd, pRecvBuf, RECVBUF_LEN, 0);

	return iRecvLen;
}

static int send_message_to_server(int dev_fd, char* pSendBuf){
	int iSendLen = 0;
	iSendLen = send(dev_fd, pSendBuf, strlen(pSendBuf), 0);
	if(iSendLen > 0){
		printf("write Date Len to server (%d) : %s\n",iSendLen, pSendBuf);
	}else{
		printf("retry\n");
	}
	return iSendLen;
}

static void tcpclient_thread_recv(void * arg)
{
	int iRecvLen;
    TcpClientDev *pdev = (TcpClientDev *)arg;

	char* pRecvBuf = (char*)malloc(RECVBUF_LEN);
	printf("GUI Send msg to TDC server:\n");
	while(1){
		
		memset(pRecvBuf,0,RECVBUF_LEN);
		iRecvLen = recv_message_from_server(pdev, pRecvBuf);
		if(iRecvLen > 0){
			messageProc(pRecvBuf, NULL, pdev);
			DEBUG_PRINT_INFO(gPrintLevel,"receive message from server (%d) :%c:%s:\n", iRecvLen, pRecvBuf[0],pRecvBuf);
		}else{
			DEBUG_PRINT_INFO(gPrintLevel,"receive message from server error\n");
		}
	}
	//close(pdev->dev_fd);
	free(pRecvBuf);
}

int tcpclient_open(TcpClientDev *pdev){
	struct sockaddr_in 	ServAddr;
	
	if(!pdev->tcplink->ServerOpen){
		return TINZ_ERROR;
	}
	/* 客户程序开始建立 sockfd描述符 */ 
	if((pdev->dev_fd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
	{ 
		DEBUG_PRINT_ERR(gPrintLevel,"Socket Error:\n"); 
		return TINZ_ERROR;
	} 
 
 	/* 客户程序填充服务端的资料 */ 
	bzero(&ServAddr,sizeof(ServAddr)); // 初始化,置0
	ServAddr.sin_family			=AF_INET;          // IPV4
	ServAddr.sin_port			=htons(pdev->tcplink->ServerPort);  // (将本机器上的short数据转化为网络上的short数据)端口号
	ServAddr.sin_addr.s_addr	=inet_addr((char*)pdev->tcplink->ServerIp); // IP地址
	
	/* 客户程序发起连接请求 */ 
	if(connect(pdev->dev_fd,(struct sockaddr *)(&ServAddr),sizeof(struct sockaddr))==-1) 
	{ 
		DEBUG_PRINT_ERR(gPrintLevel,"Connect Error:\n");
		close(pdev->dev_fd);
		return TINZ_ERROR;
	} 
	/* 连接成功了 */ 
	pdev->tcplink->isConnected = 1;
	DEBUG_PRINT_INFO(gPrintLevel,"tcp link[port:%d dstip:%s] create success\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp);
	pdev->packet_send_handle = (packet_send_cb)send_message_to_server;
	if(pthread_create(&pdev->thread_id,NULL,(void *)(&tcpclient_thread_recv),(void *)(pdev)) == -1)
	{
		DEBUG_PRINT_INFO(gPrintLevel,"pthread_create error!\n");
	}
	return TINZ_OK;
}
