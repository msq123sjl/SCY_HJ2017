#ifndef __TINZ_COMMON_HELPER__
#define __TINZ_COMMON_HELPER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DOUBLE_STRING_LEN 32

#define DEBUG_LEVEL	4
#define INFO_LEVEL	3 
#define WARN_LEVEL	2
#define ERR_LEVEL	1 

#define SHOW_DEBUG 1
#if SHOW_DEBUG
    #define DEBUG_PRINT_DEBUG(Level,...) {\
		if(DEBUG_LEVEL <= Level){\
			printf(__VA_ARGS__); \
		}\
	}
	#define DEBUG_PRINT_INFO(Level,...) {\
		if(INFO_LEVEL <= Level){\
			printf(__VA_ARGS__); \
		}\
	}
	#define DEBUG_PRINT_WARN(Level,...) {\
		if(WARN_LEVEL <= Level){\
			printf(__VA_ARGS__); \
		}\
	}
#else
    #define DEBUG_PRINT_DEBUG(Level,...)
	#define DEBUG_PRINT_INFO(Level,...)
	#define DEBUG_PRINT_WARN(Level,...)
#endif

#define DEBUG_PRINT_ERR(Level,...) {\
	if(ERR_LEVEL <= Level){\
		printf(__VA_ARGS__); \
	}\
}

#define HEXTOASCII(data_hex) ((data_hex < 10) ? data_hex + '0' : (data_hex - 10) + 'A')
#define ASCIITOHEX(high,low) (((high > 'A') ? (high - 'A' + 10) : (high - '0')) << 4 | ((low > 'A') ? (low - 'A' + 10) : (low - '0')))

#define tolower(c)      (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define toupper(c)      (u_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)
#define HEXCHECK(c)      (u_char) (((c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) ? 1 : 0)

int CRC16_GB212(char *databuff, int len);
uint16_t CRC16_Modbus(char *databuff, int len);
u_char XORValid(char *buffer ,int len);
u_char HexToAscii(u_char data_hex);
void DoubleToString(double value, int Decimals, char *buf);
inline double double_div_uint(double a, unsigned int b);
#endif