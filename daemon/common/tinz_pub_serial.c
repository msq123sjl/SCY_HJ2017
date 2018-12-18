#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include "tinz_pub_serial.h"
#include "tinz_base_def.h"
#include "tinz_common_helper.h"

//get the baud_rate defination according to the baud rate
int get_baud_rate(unsigned int baud_rate)
{
	switch(baud_rate)
	{
		case 0:
			return B0;
		case 50:
			return B50;
		case 75:
			return B75;
		case 110:
			return B110;
		case 134:
			return B134;
		case 150:
			return B150;
		case 200:
			return B200;
		case 300:
			return B300;
		case 600:
			return B600;
		case 1200:
			return B1200;
		case 1800:
			return B1800;
		case 2400:
			return B2400;
		case 4800:
			return B4800;
		case 9600:
			return B9600;
		case 19200:
			return B19200;
		case 38400:
		    return B38400;
		case 57600:
		    return B57600;
	 	case 115200:
	   		return B115200;
		case 230400:
			return B230400;

		default:
			return -1;
	}
}

int uart_set_speed(int fd, int baud_rate, struct termios* opt)
{
	unsigned int bps_int;
	int 		 status;
	bps_int=get_baud_rate(baud_rate);
	//setup input/output baudrate
    cfsetispeed(opt,bps_int);
    cfsetospeed(opt,bps_int);
	status = tcsetattr(fd, TCSANOW, opt);
	if (status != 0)
	{
		perror("tcsetattr::set baud rate failed\n");
		return -1;
	}	
	return status;
}

int uart_init(pstSerialPara p_para)
{
	int 		 	status;
	struct termios 	new_opt;

	//get the current config -> new_opt
	tcgetattr(p_para->Devfd,&new_opt);
	bzero( &new_opt, sizeof(new_opt));

	tcflush(p_para->Devfd, TCIOFLUSH);
	/*设置波特率*/
	uart_set_speed(p_para->Devfd,p_para->BaudRate,&new_opt);


	//修改控制模式，保证程序不会占用串口？
	new_opt.c_cflag |= CLOCAL;
	//printf("c_cflag |= CLOCAL => %x\r\n", new_opt.c_cflag);

	//修改控制模式，使得能够从串口读取输入数据
	new_opt.c_cflag |= CREAD;
	//printf("c_cflag |= CREAD => %x\r\n", new_opt.c_cflag);

	new_opt.c_cflag |= HUPCL;
	//setup control flow
	switch(p_para->FlowCtrl)
	{
	case 0:
		//no control-flow
		new_opt.c_cflag &=~CRTSCTS;
		break;
	case 1:
		//hardware control-flow
		new_opt.c_cflag |=CRTSCTS;
		break;
	case 2:
		new_opt.c_iflag |= IXON | IXOFF | IXANY;
		break;
	default:
		new_opt.c_cflag &=~CRTSCTS;
	}
	//printf("c_cflag(no ctl-flow) = %x\r\n", new_opt.c_cflag);

	//setup bit size
	new_opt.c_cflag &=~CSIZE;
	switch(p_para->DataBits)
	{
	case 5:
		new_opt.c_cflag |=CS5;
		break;
	case 6:
		new_opt.c_cflag |=CS6;
		break;
	case 7:
		new_opt.c_cflag |=CS7;
		break;
	case 8:
		new_opt.c_cflag |=CS8;
		break;
	default:
		new_opt.c_cflag |=CS8;
	}
	printf("c_cflag |= CS8 => %x\r\n", new_opt.c_cflag);

	//setup parity
	switch(p_para->Parity)
	{
	case 0:
		new_opt.c_cflag &= ~PARENB;   /* Clear parity enable */
		new_opt.c_iflag &= ~INPCK;     /* Enable parity checking */
		break;

	case 1:
		new_opt.c_cflag |= (PARODD | PARENB);	/* 设置为奇效验*/
		new_opt.c_iflag |= INPCK;				/* Disable parity checking */
		break;

	case 2:
		new_opt.c_cflag |= PARENB;		/* Enable parity */
		new_opt.c_cflag &= ~PARODD;		/* 转换为偶效验*/
		new_opt.c_iflag |= INPCK;       /* Disable parity checking */
		break;

	default:
		new_opt.c_cflag &= ~PARENB;   /* Clear parity enable */
		new_opt.c_iflag &= ~INPCK;     /* Enable parity checking */
	}
	//printf("c_cflag &=~PARENB => %x\r\n", new_opt.c_cflag);


	//setup stop-bit
	if(p_para->StopBits==2)
	{
		new_opt.c_cflag |=CSTOPB;
	}
	else
	{
		new_opt.c_cflag &=~CSTOPB;
	}
	//printf("c_cflag &=~CSTOPB => %x\r\n", new_opt.c_cflag);

	/* Set input parity option */
	if (p_para->Parity != 0)
	{
		new_opt.c_iflag |= INPCK;
	}

	//修改输出模式：原始数据输出(raw 模式)
	new_opt.c_lflag &= ~(ICANON | ECHO | ISIG);				/*Input*/
	new_opt.c_oflag &= ~OPOST;								/*Output*/

	//修改控制字符：读取字符的最少个数为1 ？？？
	new_opt.c_cc[VMIN]=1;

	//修改控制字符：读取第一个字符的超时时间为1×100ms
	new_opt.c_cc[VTIME]=1;

	//试图去掉在接收时必须收到'\n'才返回的问题
	//忽略输入的回车
	//new_opt.c_iflag |= IGNCR;
	//new_opt.c_iflag &= ~(IXON|IXOFF|IXANY);

	//如果发生数据溢出，接收数据，但是不再读取
	tcflush(p_para->Devfd,TCIFLUSH);

	status = tcsetattr(p_para->Devfd,TCSANOW,&new_opt);
	if(status != 0)
	{
		perror("Cannot set the serial port parameters");
		return -1;
	}

	return status;
}

int uart_open(pstSerialPara p_para){
	
	DEBUG_PRINT_INFO(5,"UART[%s|%d|%d|%d|%d|%d]",p_para->DevName,p_para->BaudRate,p_para->DataBits,p_para->Parity,p_para->StopBits,p_para->FlowCtrl);
	if(p_para->DevName[0] != 0){
		DEBUG_PRINT_ERR(5,"DevName is NULL");
		return -1;
	}
	//O_SYNC
	//fsync(p_para->Devfd);
	if((p_para->Devfd = open((char*)p_para->DevName,O_RDWR|O_NOCTTY|O_NONBLOCK))==-1)			//O_RDWR | O_NOCTTY | O_NDELAY |
	{
		DEBUG_PRINT_ERR(5,"Cannot open the desired port[%s]",p_para->DevName);
		return -1;
	}
	return 1;
}

void uart_write(pstSerialPara p_para, char *sendbuf, size_t len){
    size_t nwrite;
	tcflush(p_para->Devfd, TCIOFLUSH);
	/*
		TCIFLUSH  清除输入队列
   		TCOFLUSH  清除输出队列
   		TCIOFLUSH 清除输入、输出队列
	*/
    if(!p_para->isRS485){
        nwrite = write(p_para->Devfd,sendbuf,sizeof(sendbuf));
		tcdrain(p_para->Devfd);
        //flush(p_para->Devfd);
    }else{
        nwrite = write(p_para->Devfd,sendbuf,sizeof(sendbuf));
		tcdrain(p_para->Devfd);
        //flush(p_para->Devfd);
    }
}

/*unsigned char	read_relay_re(int fd,unsigned char *buf,unsigned short timeout){
	int	read_try=80,i;
	unsigned char	 sp=0;
	unsigned char	temp[MAX_RELAYCMD_LEN];
	int frm_sz;

	if(timeout==0)timeout = 1;
	timeout *= 20;
	if(read_try>timeout)read_try = timeout;
	while(1){
read_read:
		frm_sz=read(fd,temp,256);
		if(frm_sz<0){
			if(read_try){
				usleep(50000);
				read_try--;
				goto read_read;
			}
			return sp;	
		}
		read_try=20;
		for(i=0;i<frm_sz;i++){
			buf[sp++]=temp[i];
		}
	}
	return	sp;
		
}*/