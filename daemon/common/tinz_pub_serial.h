#ifndef __TINZ_PUB_SERIAL__
#define __TINZ_PUB_SERIAL__
#include "tinz_base_def.h"

#define UART_READ_LEN   256
int uart_init(pstSerialPara p_para);
int uart_open(pstSerialPara p_para);
void uart_write(pstSerialPara p_para, char *sendbuf, size_t len);
#endif
