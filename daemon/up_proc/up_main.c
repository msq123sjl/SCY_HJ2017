#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "up_main.h"
#include "tcp_client.h"
#include "tinz_common_helper.h"
#include "tinz_base_def.h"
#include "tinz_pub_shm.h"


pstPara pgPara;
int gPrintLevel = 5;
UpMain* pserver = NULL;

int main(int argc, char *argv[])
{
	int iLoop;
	
    pgPara = (pstPara)getParaShm();
	
	pserver = (UpMain*)malloc(sizeof(UpMain));
	if(NULL == pgPara || NULL == pserver){
		DEBUG_PRINT_ERR(gPrintLevel, "[up_main:] getParaShm or malloc fail!!!\n")
		free(pserver);
		return 0;
	}
	memset(pserver,0,sizeof(sizeof(UpMain)));
	
	signal(SIGPIPE, SIG_IGN);
	
	qt_tcpclient_open(&pserver->Qtchannes);
	for(iLoop=0; iLoop < SITE_CNT; iLoop++){
		
		pserver->channes[iLoop].tcplink = &pgPara->SitePara[iLoop];
		tcpclient_open(&pserver->channes[iLoop]);
	}
	
	/*等待socket QT通道接收线程退出*/
	pthread_join(pserver->Qtchannes.thread_id, NULL);
	close(pserver->Qtchannes.dev_fd);
	
	/*等待socket 平台通道接收线程退出*/
	for(iLoop = 0; iLoop < SITE_CNT; iLoop++){
		if(!pserver->channes[iLoop].tcplink->ServerOpen || !pserver->channes[iLoop].tcplink->isConnected){continue;}
		pthread_join(pserver->channes[iLoop].thread_id, NULL);
		pserver->channes[iLoop].tcplink->isConnected = 0;
		close(pserver->channes[iLoop].dev_fd);
	}
	
	/*等待socket 发送线程退出*/
	
	free(pserver);
	return 0;
}
