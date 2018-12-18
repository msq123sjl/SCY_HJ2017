/*   
    	config_file.h
  			
  	功能:	读取配置文件,类似ini配置文件
  			支持以#注释
  			
  	armording		2007.04.06
  */   
    
  #ifndef   _CONFIG_FILE_H_   
  #define   _CONFIG_FILE_H_  
  #include<time.h>
    

  
  /*   定义错误代码   */   
  #define   CFG_FILE_NOERROR   0 /*  正确  */   
  #define   CFG_FILE_NOFILE -1 /*   文件不存在或读取失败  */   
  #define   CFG_FILE_NOTFIND -2 /*  没找到   */   
    
    
  /*-   
  功能: 读取配置文件中的键值为字符串项
  
  参数:
  		section: 节名,输入
  		keyname: 键名,输入
  		keyvalue:键值,用于返回键值的缓冲区
  		len:	keyvalue的大小
  		filename:配置文件名
  		
  返回:错误码，0读取正确
  -*/   
  int   getconfigstring(const   char*     section,   
    const   char*     keyname,   
    char*               keyvalue,   
    unsigned   int   len,   
    const   char*     filename);   
    
    
  
    /*-   
  功能: 读取配置文件中的键值为数字的项
  
  参数:
  		section: 节名,输入
  		keyname: 键名,输入
  		keyvalue:键值,用于返回键值的指针
  		filename:配置文件名
  		
  返回:错误码，0读取正确
*/

  int   getconfigint(const   char*     section,   
    const   char*     keyname,   
    int*                   keyvalue,   
    const   char*     filename);     

  #endif   

