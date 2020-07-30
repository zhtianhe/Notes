#include<stdio.h>
#include<msgpub.h>
#include<msgapi.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/time.h>
#include<sys/resource.h>

//logFlag:0-off,1-debug,2-info,3-warn,4-error,5-fatal,6-panic
int LogFlag=0; //日志调试开关
#define ErrLog(format,...){\
	if(0<LogFlag && LogFlag<=4)\
	printf("\nErrLog|F:%-16s +%-4d |%s|[" format "]\n",\
			__FILE__,__LINE__,__FUNCTION__, ## __VA_ARGS__);\
}

#define Warn(format,...){\
	if (0<LogFlag && LogFlag<=3)\
	printf("\nWarn|F:%-16s +%-4d |%s|" format "|\n",\
			__FILE__,__LINE__,__FUNCTION__, ## __VA_ARGS__);\
}

#define Info(format,...){\
	if (0<LogFlag && LogFlag<=2)\
	printf("\nInfo|F:%-16s +%-4d |%s|" format "|\n",\
			__FILE__,__LINE__,__FUNCTION__, ## __VA_ARGS__);\
}

#define Debug(format,...){\
	if(	0<LogFlag && LogFlag<=1)\
	printf("\nDeBug|F:%-16s +%-4d |%s|[" format "]\n",\
			__FILE__,__LINE__,__FUNCTION__, ## __VA_ARGS__);\
}


#define __test__ 0  //1-调用打开调试信息 调用的是C函数  0-关闭调试
#if __test__


struct _opt{
   
	};  
typedef struct _opt SV;
static SV sv;

void DoHelp(char *c)
{
	printf("\t---------------------------------------------------------------------\n");
	printf("\t                              帮助信息                               \n");
	printf("\t---------------------------------------------------------------------\n");
	printf("\t---------------------------------------------------------------------\n");

}

static void SvOption(int argc,char * argv[]){
	memset(&sv,0x00,sizeof(SV *));
	if (argc > 1)
		{
			if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "?") == 0)
			{
				DoHelp(argv[0]);
				exit(0);
			}
			else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "-v") == 0  )
			{
				printf("VERSION: %s\n", "1.0.0");
				exit(0);
			}

			int c;
			opterr=0;
			while ((c = getopt(argc, argv,"s:m:c:d:"))!= -1) //X:-表示X选项有参数, X-表示X选项无参数
			{
				switch (c)
				{
					case '?':
					printf("Try \"%s --help\" for more information.\n", argv[0]);
					exit(1);

					case 's':
					strcpy(sv.ServerName,optarg);
					break;

					case 'm':
					strcpy(sv.MqName,optarg);
					break;

					case 'd':
					strcpy(sv.SysCode,optarg);
					break;

					case 'c':
					strcpy(sv.MsgData,optarg);
					break;

				}
			}
	}
#endif

typedef struct {//报文属性设置
	int  CLogFlag			   ;//|日志调试开关     |//logFlag:0-off,1-debug,2-info,3-warn,4-error,5-fatal,6-panic
	int  RetCode                 ;//|调用cpds 函数返回值
	char RetStr[120]              ;//|调用cpds 错误说明
}CpdsMsg;

void CpdsMsgPrintf(CpdsMsg MsgPkg){
	Debug("配置文件标签|ServerName    :[%s]\n",MsgPkg.ServerName     );
	Debug("配置文件队列|DataType      :[%s]\n",MsgPkg.DataType       );
	Debug("cpds 返回值 |RetCode      :[%d]\n",MsgPkg.RetCode          );
	Debug("cpds 错误说明|RetStr       :[%s]\n",MsgPkg.RetStr         );
}

///*****************************************************************************
// * 函 数 名  : CpdsMsgErrDesc
// * 负 责 人  : 张天合
// * 创建日期  : 2019年8月9日
// * 函数功能  : 数据共享send recv 函数错误码
// * 输入参数  : int ret
// * 输出参数  : 无
// * 返 回 值  : char 返回描述信息
// * 调用关系  :
// * 其    它  :
//
//*****************************************************************************
static char *CpdsMsgErrDesc(int ret){
	switch ( ret ){
		case  0 : return "成功"                                               ;
		case -2 : return "队列缓存超限"                                       ;
		case -3 : return "获取队列名失败"                                     ;
		case -4 : return "消息发送失败"                                       ;
		case -5 : return "接收消息失败"                                       ;
		case -6 : return "接收消息超时"                                       ;
		case -7 : return "创建队列失败"                                       ;
		case -8 : return "创建主题失败"                                       ;
		case -9 : return "未知的目的地类型(队列或主题不符合命名规范)"         ;
		case -10: return "创建发送者失败"                                     ;
		case -11: return "创建接收者失败"                                     ;
		case -12: return "创建会话失败"                                       ;
		case -13: return "创建错误跟踪失败"                                   ;
		case -14: return "创建工厂失败"                                       ;
		case -15: return "给工厂赋地址失败"                                   ;
		case -16: return "创建连接失败"                                       ;
		case -17: return "开始连接错误"                                       ;
		case -18: return "未配置环境变量CPDS_CONF_HOME"                       ;
		case -19: return "获取消息筛选功能开启选项selector_switch失败"        ;
		case -20: return "未配置消息筛选器"                                   ;
		case -21: return "获取SERVERURL失败"                                  ;
		case -22: return "获取用户名失败"                                     ;
		case -23: return "获取密码失败"                                       ;
		case -24: return "获取消息筛选条件selector_conf失败"                  ;
		case -25: return "已开启消息筛选功能，但未配置筛选条件selector_name"  ;
		case -26: return "创建持久化订阅者失败"                               ;
		case -27: return "获取持久化订阅者名称失败"                           ;
		case -28: return "持久化订阅者名称为空"                               ;
		case -29: return "获取recv_timeout配置失败"                           ;
		default:  return "数据共享消息类型错信息未知"                         ;
	}
}


#if __test__     //测试使用
int teat_mian (int argc,char * argv[])
{
	
	return 0;
}
#endif

