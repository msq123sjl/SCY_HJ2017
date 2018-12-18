#ifndef __TINZ_PUB_MESSAGE__
#define __TINZ_PUB_MESSAGE__

#define MSG_NAME_UPMAIN_TO_SQLITE		"/mnt/nandflash/msg/upmain_to_sqlite"
#define MSG_ID_UPMAIN_TO_SQLITE_ID		0x01

#define MSG_PATH_DATAPROC_TO_SQLITE		"/mnt/nandflash/msg/"
#define MSG_NAME_DATAPROC_TO_SQLITE		"/mnt/nandflash/msg/DataProc_to_sqlite"
#define MSG_ID_DATAPROC_TO_SQLITE		0x01

#define MAX_MSG_DATA_LEN 256
#define MSG_RTYCNT	3
struct _msgbuf
{
    long int mtype;
	char data[MAX_MSG_DATA_LEN];
};

struct _msg
{
    int msgid;;
	struct _msgbuf msgbuf;
};

int prepareMsg(char* ftokpath,char* ftokname,int ftokid,struct _msg* msg);
int MsgSend(struct _msg* msg);
void MsgRcv(struct _msg* msg, long int mtype);
void rmMsg(struct _msg* msg);

#endif

