#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>

#include "sqlite3.h"

#include "tinz_common_db_helper.h"
#include "tinz_common_helper.h"
#include "tinz_base_def.h"

/*
当synchronous设置为FULL (2), SQLite数据库引擎在紧急时刻会暂停以确定数据已经写入磁盘。这使系统崩溃或电源出问题时能确保数据库在重起后不会损坏。FULL synchronous很安全但很慢。
当synchronous设置为NORMAL, SQLite数据库引擎在大部分紧急时刻会暂停，但不像FULL模式下那么频繁。 NORMAL模式下有很小的几率(但不是不存在)发生电源故障导致数据库损坏的情况。但实际上，在这种情况 下很可能你的硬盘已经不能使用，或者发生了其他的不可恢复的硬件错误。
设置为synchronous OFF (0)时，SQLite在传递数据给系统以后直接继续而不暂停。若运行SQLite的应用程序崩溃， 数据不会损伤，但在系统崩溃或写入数据时意外断电的情况下数据库可能会损坏。另一方面，在synchronous OFF时 一些操作可能会快50倍甚至更多。在SQLite 2中，缺省值为NORMAL.而在3中修改为FULL。
*/
int tinz_db_open(tinz_db_ctx_t *ctx)
{
    if (SQLITE_OK != sqlite3_open((const char*)ctx->name, (sqlite3**)&ctx->db)) {
		DEBUG_PRINT_ERR(5,"open database failed:[%s]\n",sqlite3_errmsg((sqlite3*)ctx->db));
        return TINZ_ERROR;
    }

    if (SQLITE_OK != sqlite3_exec(ctx->db, "PRAGMA synchronous = OFF", NULL, NULL, &ctx->errmsg)) {
		DEBUG_PRINT_ERR(5, "set synchronous = OFF failed:[%s]", ctx->errmsg);
		sqlite3_free(ctx->errmsg);
        return TINZ_ERROR;
    }

    return TINZ_OK;
}

int tinz_db_close(tinz_db_ctx_t* ctx)
{
    if (SQLITE_OK != sqlite3_close(ctx->db)) {
		DEBUG_PRINT_ERR(5,"close database failed:[%s]", sqlite3_errmsg((sqlite3*)ctx->db));
        return TINZ_ERROR;
    }
    return TINZ_OK;
}

void tinz_db_exec(tinz_db_ctx_t* ctx, char *sql)
{
	DEBUG_PRINT_INFO(5,"sql[%s]\n",sql);
	if(SQLITE_OK != sqlite3_exec(ctx->db, sql, NULL, NULL,&ctx->errmsg)){
		DEBUG_PRINT_ERR(5, "SQL error: %s\n",ctx->errmsg);
		sqlite3_free(ctx->errmsg);
	}
}
//查找某数据表是否存在
int TableIsExist(tinz_db_ctx_t* ctx, char *tableName)
{
	int	cnt = 0;
	char sql[SQL_LEN];
	snprintf(sql,sizeof(sql)-1,"select count(*) from sqlite_master where type=\'table\' and name=\'%s\';",tableName);
	sqlite3_prepare(ctx->db, sql, -1, &ctx->stat, 0);
	if(SQLITE_ROW != sqlite3_step(ctx->stat)){
		cnt = sqlite3_column_int(ctx->stat,0);
	}
    sqlite3_finalize(ctx->stat);
	
    return cnt>0 ? TINZ_OK : TINZ_ERROR;
}

//创建实时数据表
void RtdTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
    char sql[SQL_LEN];	
	snprintf(sql,sizeof(sql)-1,"create table %s(\
								GetTime DATETIME(20),\
								Rtd NVARCHAR(20),\
								Flag NVARCHAR(5),\
								Total NVARCHAR(20));",tableName);
	tinz_db_exec(ctx,sql);
}

//创建统计数据表
void CountDataTableCreate(tinz_db_ctx_t* ctx, char *tableName)
{
	char sql[SQL_LEN];
	snprintf(sql,sizeof(sql)-1,"create table %s(\
								GetTime DATETIME(20),\
								Max NVARCHAR(20),\
								Min NVARCHAR(20),\
								Avg NVARCHAR(20),\
								Cou NVARCHAR(20));",tableName);
	tinz_db_exec(ctx,sql);
}


