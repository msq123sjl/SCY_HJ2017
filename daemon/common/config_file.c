/*   
    	config_file.c
  			
  	功能:	读取配置文件,类似ini配置文件
  			支持以#注释

  	文件格式:
  		#this is remark text
  		[sectorname]
  		keyname = keyvalue
  		
  	armording		2007.04.06
  */   

#include <stdio.h>   
#include <stdlib.h>
#include <ctype.h>   
#include <string.h>   
#include <time.h>
#include <dirent.h>   

#include "config_file.h"   
#include "tinz_base_def.h" 
#include <unistd.h>

    
/*   Set   line   length   in   configuration   files   */     
#define   CFG_FILE_LINE 1024   
    
    
static   int   readline(char*   line,   FILE*   stream)   
{   
	int   flag   =   1;   
	char   buf[CFG_FILE_LINE];   
 	int   i,   k   =   0;   
	line[0]=0;
      	if(fgets(buf,CFG_FILE_LINE,stream)!=NULL){   
	//		printf("buf=%s",buf);
    		for(i=strlen(buf)-1;i>=0;i--){   
  			if(buf[i]=='\r'||buf[i]=='\n'||buf[i]=='#')   
  				buf[i]   =   '\0';   
 		}

		for(i=0;i<=strlen(buf);i++){   
  			if (flag&&(buf[i]=='\r'||buf[i]=='\n'||buf[i] == ' ' ||buf[i] =='\t') )   
 				 continue;   
  			else   {   
 				 flag   =   0;   
  				line[k++]   =   buf[i]; 
			}   
  		}
		line[k]=0;
 		return   0;   
	}
	return   -1;   
}   
 
static   int   getsection(const   char*   line,   char*   section)   
{   
  	unsigned   int   start,   end;   
	start=0;
	end=strlen(line)-1;

	if(end-start<2||line[start]!='['||line[end]!=']') return -1;
	
	for(start=1;start<strlen(line);start++){   
    		if(line[start]==' '||line[start]=='\t')	continue;	/*去掉前面空格*/
  		else   break;
	}   
	for(end=strlen(line)-2;end>1;end--){
 		if(line[end]==' '||line[end]=='\t')	continue;	/*去掉后面空格*/
 		else   break;   
  	}   
    	memcpy(section,line+start ,end-start+1);   
  	section[end-start+1]='\0';   
	
  	return   0;   
}   
     
static   int   getkey(const   char*   line,   char*   keyname,   char*   keyvalue)   
{   
  	unsigned   int   i,   start;   

	for(start=0;start<strlen(line);start++)   {   
		if(line[start]=='=')   break;
	}
	if(start>=strlen(line))   
		return   -1;   /*   找不到'=',   return   */   
	memcpy(keyname,line,start);   
 	keyname[start]='\0';   
    	/*   去掉后面的空格和tab */   
 	for(i=strlen(keyname)-1;i>=0;i--){   
		if(keyname[i]==' '||keyname[i]=='\t')
			keyname[i]='\0';   
		else   break;   
	}      
	/*   Find   key   value   */   
	for(start=start+1;start<strlen(line);start++){   
  		if(line[start]!=' '&&line[start]!='\t')     
 			 break;     
  	}   
    
  	strcpy(keyvalue,line+start);   

    	/*   去掉后面的空格和tab */   
	 for(i=strlen(keyvalue)-1;i>=0;i--){   
	 	if(keyvalue[i]==' '||keyvalue[i]=='\t')
			keyname[i]='\0';   
  		else   break;   
	}   
    
 	return   0;     
}   
    
    
int   getconfigstring(const   char*     section,   
    const   char*     keyname,   
    char*               keyvalue,   
    unsigned   int   len,   
    const   char*     filename)   
{   
  	int   step   =   0;   
 	FILE*   stream;   
  	char   sec[CFG_FILE_LINE];   
	char   ken[CFG_FILE_LINE];   
  	char   kev[CFG_FILE_LINE];   
  	char   line[CFG_FILE_LINE];   
  	if((stream=fopen(filename,"r"))==NULL)
		    return   CFG_FILE_NOFILE;   
	while(!feof(stream)){
   		 if(readline(line,stream)<0){   
 			fclose(stream);   
			return   CFG_FILE_NOTFIND;   
 		 }
		 if(strlen(line)>2){   
	  		if(step==0) {   /*  section   */   
	  			if(getsection(line,sec)==0){   
	 				 if(strcmp(sec,section)==0)step=1;   
	  			}   
	  		}     
	 		else{   /*   keyname,   keyvalue   */   
	  			if(getkey(line,ken,kev)==0){   
					if(strcmp(ken,keyname)==0){
	  					strncpy(keyvalue,kev,len);   
	  					fclose(stream);   
	  					return   CFG_FILE_NOERROR;   
	 				 }     
	  			}   
	 		 }    
  		}  
 	}   
  
	fclose(stream);       
  	return   CFG_FILE_NOTFIND;   
}   
 unsigned long String2long(char *kval)
{
 unsigned char i,len=0;
 unsigned char  temp[8]={0,0,0,0,0,0,0,0};
 unsigned long res=0;

   len=strlen(kval);
   if(len<1||len>8) return 0;
    for(i=0;i<len;i++){
	res *=0x10;
     if(kval[i]>='0'&&kval[i]<='9')
	temp[i]=kval[i]-'0';
     else if(kval[i]>='a'&&kval[i]<='f')
	temp[i]=kval[i]-'a'+0xa;
     else if(kval[i]>='A'&&kval[i]<='F')
	temp[i]=kval[i]-'A'+0xa;
     res+=temp[i];
    }
return res;
}
    
	int   getconfiglong(const	char*	  section, const   char*	 keyname,unsigned long*  keyvalue, const   char*	 filename) {   
		int   rs=CFG_FILE_NOTFIND;	 
		char   kev[20];   
			memset(kev,0,sizeof(kev));	
			
		rs=getconfigstring(section,keyname,kev,sizeof(kev),filename);	
		*keyvalue=0;
		if(rs==CFG_FILE_NOERROR)	 
			*keyvalue=String2long(kev);   
		return	 rs;   
	}	
    
int   getconfigint(const   char*     section,   
    const   char*     keyname,   
    int*                   keyvalue,   
    const   char*     filename)   
{   
 	int   rs=CFG_FILE_NOTFIND;   
	char   kev[20];   
    	memset(kev,0,sizeof(kev));  
        
	rs=getconfigstring(section,keyname,kev,sizeof(kev),filename);   
    *keyvalue=0;
  	if(rs==CFG_FILE_NOERROR)     
		*keyvalue=atoi(kev);   
  	return   rs;   
}   

//判断float是否有效1有效
int short_avail(unsigned short source,unsigned char avail)
{
	int i;
	unsigned char *p;
	p=(unsigned char *)&source;
	for(i=0;i<2;i++){
		if(*(p+i)!=avail)
			return	1;
	}
	return	0;
}


int delete_file(unsigned char *infile)
{
	char filename[MAX_FILENAME_SIZE];
	snprintf((char*)filename,sizeof(filename)-1,"%s.del",infile);
	if(infile[0]!=0&&access((char*)infile, F_OK) ==0){
		rename((char*)infile,filename);
		if(vfork()==0){
			remove(filename);
			exit(0);
		}
		printf("deleted file: %s\n",filename);
		return 1;
	}
	return -1;
}
