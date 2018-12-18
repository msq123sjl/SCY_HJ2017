#ifndef __UP_MAIN__
#define __UP_MAIN__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp_client.h"
#include "tinz_base_def.h"

//#define 	MAX_HOST_COUNT	1
//#define 	MAX_QT_HOST_COUNT	1
typedef struct _UpMain{
	
	QtTcpClientDev 	Qtchannes;
	TcpClientDev 	channes[SITE_CNT];
	/*unsigned char*  tmpsendbuf1;
	unsigned char*  tmpsendbuf2;
	unsigned char*  tmprecvbuf;

	stUpLink up_link[MAX_HOST_COUNT];
	ChanDev channes[MAX_HOST_COUNT];
	Protocol protocols[MAX_HOST_COUNT];

	GSM_Module gsm_module;

	pstSamplingHarmonicState pRtuData;
	pstRealData	  pRealdata;*/
}UpMain;

#endif