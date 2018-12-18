#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/stat.h>
#include"tinz_pub_shm.h"
#include"tinz_base_def.h"
#include"tinz_common_helper.h"

struct SHM_DESC shm_para={-1,0};

//创建参数共享内存段
//return value =-1 共享内存不成功；＝0共享内存成功，文件读写不成功，＝1内存和文件都成功
int prepareShm(char* ftokpath,char* ftokname,int ftokid,char* fsname,int shm_len,struct SHM_DESC* shm){	
	char bfirst=0;
	key_t   shm_key;       
	FILE*  fd=0;
	char tempchar[128];
	int minLen=0;
	if(shm->shm_mem)return 1;
		
	shm->shm_id=-1;
	shm->shm_mem=0;

	fd=fopen(ftokname,"rb+");
	if(fd<=0){
		if(access(ftokpath,0)){
			mkdir(ftokpath,S_IRWXU);
		}
		fd=fopen(ftokname,"wb+");
		if(fd<=0){
			sprintf(tempchar,"ftok error %s:",ftokname);
	        perror(tempchar);
			return -1;
		}
	}
	fclose(fd);
	fd=0;
	shm_key=ftok(ftokname,ftokid);
	if(shm_key == -1) {
		 sprintf(tempchar,"ftok error %s:",ftokname);
		 perror(tempchar);
   		 //perror("ftok error :");
		 return -1;
	}
	shm->shm_id= shmget(shm_key,shm_len,0);
	if(shm->shm_id==-1){
		shm->shm_id= shmget(shm_key,shm_len,IPC_CREAT|IPC_EXCL|0666);
		bfirst=1;
	}	

	if(shm->shm_id==-1){
		perror("shmget error:");
		return -1;
	}
	shm->shm_mem=(char*)shmat(shm->shm_id,NULL,0);
	if(shm->shm_mem==(char*)-1){
		perror("shmat error:");
		if(bfirst)	shmctl(shm->shm_id,IPC_RMID,NULL);
		shm->shm_mem=0;
		return -1;
	}
	if(bfirst&&fsname>0){
		fd=fopen(fsname,"rb");
		if(fd>0){
			fseek(fd,0,SEEK_END);
			minLen=ftell(fd);
			if(minLen<=0){
				fclose(fd);
				return 0;
			}
			if(minLen>shm_len)minLen=shm_len;
			fseek(fd,0,SEEK_SET);
			if(fread(shm->shm_mem,minLen,1,fd)!=1){
				fclose(fd);
				return 0;
			}
			fclose(fd);
		}
		else {
			return 0;
		}
	}
	return 1;
}

char * getParaShm(){
	if(prepareShm(SHM_PATH_PARA,SHM_NAME_PARA,SHM_PARA_ID,FS_NAME_PARA,sizeof(stPara),&shm_para)==0){
		DEBUG_PRINT_INFO(5, "initParaShm[%s]\n",SHM_NAME_PARA);
		initParaShm();
		syncParaShm();
	}
	return shm_para.shm_mem;
}

void rmParaShm(){
	shmctl(shm_para.shm_id,IPC_RMID,NULL);
}

void syncParaShm(){
	int ret,iLoop;
	FILE*  fd=0;
	pstPara para=(pstPara)shm_para.shm_mem;
    pstPara para_tmp = (pstPara)malloc(sizeof(stPara));
    memcpy(para_tmp,para,sizeof(stPara));
    for(iLoop = 0; iLoop < SERIAL_CNT; iLoop++){
        para_tmp->SerialPara[iLoop].Devfd=-1;
    }
    for(iLoop = 0; iLoop < SITE_CNT; iLoop++){
        para_tmp->SitePara[iLoop].isConnected = 0;
    }
    
	if(para==0)return;	
	fd=fopen(FS_NAME_PARA,"rb+");
	if(fd<=0){
		if(access(FS_PATH_PARA,0)){
			mkdir(FS_PATH_PARA,S_IRWXU);
		}
		fd=fopen(FS_NAME_PARA,"wb+");
		if(fd<=0){
			DEBUG_PRINT_ERR(5,"open fs_para.dat file failure.\n");
			return;
		}
	}
	fseek(fd,0,SEEK_SET);
	ret=fwrite(para_tmp,sizeof(stPara),1,fd);
	fflush(fd);
	fclose(fd);
    free(para_tmp);
	DEBUG_PRINT_INFO(5,"save para %s.\n",ret==1?"succeed":"failure");
}

void initParaShm(){
	int iLoop=0;
	pstPara para=(pstPara)shm_para.shm_mem;
	memset(para,0,sizeof(stPara));	
	/*基本设置*/
	snprintf((char*)para->GeneralPara.MN,MN_LEN-1,"%s","88888880000001");
	para->GeneralPara.RtdInterval 			= 60;
	para->GeneralPara.MinInterval 			= 5;
	para->GeneralPara.CatchmentTime 		= 5;
	para->GeneralPara.COD_CollectInterval 	= 3;
	para->GeneralPara.OverTime				= 60;
	para->GeneralPara.ReCount				= 3;
	para->GeneralPara.AlarmTime				= 30;
	para->GeneralPara.StType				= 0;
	para->GeneralPara.RespondOpen			= 0;
	/*因子设置*/
		//默认值是0或空
	/*串口设置*/
	for(iLoop=0; iLoop < SERIAL_CNT; iLoop++){
		snprintf((char*)para->SerialPara[iLoop].DevName, UART_DEVNAME_LEN - 1, "/dev/ttyS%d",iLoop+1);
		para->SerialPara[iLoop].isServerOpen    = 0;
		para->SerialPara[iLoop].isRS485			= 0;
		para->SerialPara[iLoop].BaudRate		= 9600;
		para->SerialPara[iLoop].DataBits		= 8;
		para->SerialPara[iLoop].Parity			= 0;
		para->SerialPara[iLoop].StopBits		= 0;
		para->SerialPara[iLoop].FlowCtrl		= 0;
		para->SerialPara[iLoop].TimeOut			= 1000;
		para->SerialPara[iLoop].Interval		= 1000;
		para->SerialPara[iLoop].Devfd			= -1;
	}
	/*开关量设置 
	    Input:  10~15,18~23 
	    Output: 6~9,16,17,24,25
	*/
	para->IOPara.Out_drain_open         = 6;
    para->IOPara.Out_drain_close        = 7;
    para->IOPara.Out_catchment_open     = 8;
    para->IOPara.Out_catchment_close    = 9;
    para->IOPara.Out_reflux_control     = 16;
    para->IOPara.In_drain_open          = 10;
    para->IOPara.In_drain_close         = 11;
    para->IOPara.In_catchment_open      = 12;
    para->IOPara.In_catchment_close     = 13;
    para->IOPara.In_reflux_open         = 14;
    para->IOPara.In_reflux_close        = 15;
    para->IOPara.In_power               = 18;
	/*站点设置*/
    for(iLoop = 0; iLoop < SITE_CNT; iLoop++){
        para->SitePara[iLoop].ServerOpen   = 0;
        para->SitePara[iLoop].isConnected  = 0;
        para->SitePara[iLoop].ServerPort   = 8810;
		snprintf((char*)para->SitePara[iLoop].ServerIp,sizeof(para->SitePara[iLoop].ServerIp)-1,"%s","192.168.1.200");
    }
	/*用户设置*/
}
