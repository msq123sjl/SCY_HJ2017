#ifndef __TINZ_PUB_SHM__
#define __TINZ_PUB_SHM__

#define FS_NAME_PARA		"/mnt/nandflash/para/fs_para.dat"
#define FS_PATH_PARA		"/mnt/nandflash/para/"

#define SHM_NAME_PARA		"/mnt/nandflash/shm/shm_para"
#define SHM_PATH_PARA		"/mnt/nandflash/shm/"

#define SHM_PARA_ID	0x01

struct  SHM_DESC{
	int shm_id;
	char *shm_mem;
};

char * getParaShm();
void rmParaShm();
void syncParaShm();
void initParaShm();
#endif