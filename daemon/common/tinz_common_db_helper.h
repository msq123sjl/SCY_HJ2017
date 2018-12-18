#ifndef __TINZ_COMMON_DB_HELPER__
#define __TINZ_COMMON_DB_HELPER__
#include "sqlite3.h"
#define SCY_DATA		"/mnt/nandflash/scy_data.db"
#define SCY_DATA_bak	"/mnt/sdcard/scy_data_bak.db"

#define TABLE_NAME_LEN	32
#define SQL_LEN			256
typedef struct {
    sqlite3    *db;
	sqlite3_stmt *stat;
	char 		*errmsg;
    char   		name[64];
} tinz_db_ctx_t;

int TableIsExist(tinz_db_ctx_t* ctx, char *tableName);
void RtdTableCreate(tinz_db_ctx_t* ctx, char *tableName);
void CountDataTableCreate(tinz_db_ctx_t* ctx, char *tableName);
void tinz_db_exec(tinz_db_ctx_t* ctx, char *sql);
int tinz_db_close(tinz_db_ctx_t* ctx);
int tinz_db_open(tinz_db_ctx_t *ctx);

#endif

