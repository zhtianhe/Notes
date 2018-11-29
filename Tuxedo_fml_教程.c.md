# tuxedo fml 编程

    本例是在上，使用的是tuxedo12 ,从本地读取一个文件tlr.txt，然后把文件内容以FML32
    缓冲区方式发送到服务器，由服务器处理后，返回内容增加一个序号。文件内容为：
` cat tlr.txt `
```txt
hwtt 29 18677150924
lhtt 28 13398813422
csqt 25 13234234564
```
服务器处理后，预期结果为：
` ./clientfml tlr.txt `
```TXT
AGE	29
AGE	28
AGE	25
NAME	hwtt
NAME	lhtt
NAME	csqt
PHONE	18677150924
PHONE	13398813422
PHONE	13234234564

-----<tpcall>126----
[160]hwtt--29--18677150924---33
[160]lhtt--28--13398813422---42
[160]csqt--25--13234234564---21
AGE	29
AGE	28
AGE	25
NO	33
NO	42
NO	21
NAME	hwtt
NAME	lhtt
NAME	csqt
PHONE	18677150924
PHONE	13398813422
PHONE	13234234564
```

## 利用模板制作ubb文件并制作二进制文件
UbbTemplate
```TUXCONFIG
*RESOURCES
IPCKEY          155442 ##IPCKEY: TUXEDO使用它标识公告板及其他的IPC资源。
                ##它不能与该服务器上其他的IPC资源的ID号冲突范围：32,769-262,142
DOMAINID        TDOM1  ##DOMAINID：该TUXEDO应用系统的唯一标识
                #MAXACCESSERS,MAXSERVERS,MAXSERVICES:
                ##这三个参数控制该TUXEDO应用系统对IPC资源的使用情况。
MAXACCESSERS    1500   ##在本系统的一个节点(一台服务器)上,同时可以有多少个进程可以访问
MAXSERVERS      4000   ##在本系统中,总共可以有多少个SERVER存在,包括进行管理的SERVER,
MAXSERVICES     20000  ##在本系统中,总共可以有多少个SEVICE存在, 默认值为100。
LDBAL           N      ## 要不要进行负载均衡,Y:要,N:不要,默认值为不要
MASTER          simple
MODEL           SHM    ##单机模式，就是不涉及多个的服务器互联，一般用这个模式
                #SHM:   单机或多台服务器但共用一个全局共享内存
		        #MP:    多台服务器但没有共用一个全局共享内存
		        #OPTION: LAN: 是多机（MP）部署模式
		        #MIGRATE:可对该系统进行迁移
*MACHINES	#该TUXEDO应用系统所包含的每台服务器都要在该节中进行配置
"${HOSTNAME}"	   # uname -n
            LMID=simple #在TUXEDO，要为该应用系统中的每台服务器指定一个逻辑服务器名。
            ##如在上面的例子中，TUXEDO应用服务器MYSYS对应的逻辑服务器名为：simple，
            TUXCONFIG="${TUXCONFIG}"  #"/home/tuxedo/Fmlsrc/tuxconfig"
            APPDIR  = "${APPDIR}"     #"/home/tuxedo/Fmlsrc"
            TUXDIR  = "${TUXDIR}"     #"/home/tuxedo/tuxedo12/tuxedo12.2.2.0.0"
            ULOGPFX = "${ULOGPFX}/ULOG"  #"/home/tuxedo/Fmlsrc/ULOG"
		    MAXWSCLIENTS = 5
    #		TLOGDEVICE="/home/tuxedo/Fmlsrc/TLOG"
    #		TLOGNAME=TLOG
    #		TLOGSIZE=500 # TLOG的大小，一般用crdl命令创建TLOGDEVICE的时候，
                         #-b的参数要比这个大20-30%，不然会出错
    # 		MAXWSCLIENTS=45 #最多可以有多少个远程客户端可以连接到当前主机
    #注意：TUXCONFIG，TUXDIR，APPDIR的值要与它们在环境变量中的设置的值一样

*GROUPS
GROUPWSL	LMID=simple     GRPNO=10        OPENINFO=NONE
MATHGRP         LMID=simple	GRPNO=11 	OPENINFO=NONE
GROUP1          LMID=simple     GRPNO=1         OPENINFO=NONE
#连接数据库
#               TMSNAME=TMS_ORA10g
#               TMSCOUNT=2 #               #OPENINFO="Oracle+XA:Oracle_XA+Acc=P/cpnspro/*****+SesTm=10+LOgDir=/app/log"

*SERVERS
DEFAULT: CLOPT="-A"

"WSL" 	SRVGRP="GROUPWSL" SRVID=10
        #RESTART=Y GRACE=86400 MAXGEN=100 REPLYQ=Y
        CLOPT="-A -- -n ${WSLADDR}-x 10 -m 4 -M 5 -c 1024 -I 30 -N 60"
		#CLOPT="-A -- -n //10.10.10.70:3200 -x 10 -m 4 -M 5 -c 1024 -I 30 -N 60"

"serverfml"  SRVGRP=MATHGRP SRVID=1
             #LOPT="-A" RQADDR="simpserv" RQPERM=0660 REPLYQ=Y RPPERM=0660
             #MIN=5 MAX=5 CONV=N MAXGEN=1 GRACE=86400 RESTART=N

*SERVICES ## 服务进程里面的服务名，需要一一填写，客户端才能找到
FMLTEST
```
## 环境变量配置
setenv.env
```sh
cat <<EOF
   本程序仅做练习使用 Tuxedo fml32 程序实例说明：
   前期准备需要安装 TUXEDO并使安装目录下的 tux.env 生效
   在本程序目录下加载环境变量
   执行: . setenv.env
   编译程序：make
   启动服务：tmboot -y
   运行本地客户端：clientfml  tlr.txt
   运行远程客户端：clientfmlw tlr.txt
   关闭服务：tmshutdow -ycw1
   清空垃圾文件： make cll
EOF

if [ -z ${TUXDIR} ];then
	echo TUXDIR is null ;
	exit -1;
else
	echo TUXDIR=${TUXDIR};
fi

#TUXDIR=/home/hwt/tux9.1/tuxedo9.1
APPDIR=$(pwd)
TUXCONFIG=$APPDIR/tuxconfig
FIELDTBLS32=fmlfile
FLDTBLDIR32=$APPDIR
TPXPARSFILE=stock_validate.par
#WSNADDR=//10.10.10.70:3200
WSNADDR=//$(ip addr  | grep -i "inet .* \(?: \|ens33\|eth0\)" \
           | cut -d/ -f1 | awk '{print $2}'):3200
export APPDIR TUXCONFIG FLDTBLDIR32 FIELDTBLS32 TPXPARSFILE WSNADDR

WSLADDR=//$(ip addr  | grep -i "inet .* \(?: \|ens33\|eth0\)" \
	   | cut -d/ -f1 | awk '{print $2}'):3200

echo ${APPDIR} $WSLADDR

sed  -e 's#${HOSTNAME}#'"`uname -n`#g" \
     -e 's#${TUXCONFIG}#'"${TUXCONFIG}#g" \
     -e 's#${APPDIR}#'"${APPDIR}#g" \
     -e 's#${TUXDIR}#'"${TUXDIR}#g" \
     -e 's#${ULOGPFX}#'"${TUXCONFIG}#g" \
     -e 's#${WSLADDR}#'"${WSLADDR}#g" \
     UbbTemplate > uBBconfigunix

tmloadcf -y uBBconfigunix

```

 使环境变量生效
 ` . setenv.env `

## 程序实例

### 1、客户端程序

```C
#include <stdio.h>
#include <string.h>
#include <atmi.h>
#include <fml32.h>
#include <userlog.h>
#include "fmlfile.h"

int main(int argc, char *argv[])
{

	if(argc!=2)
	{
		(void)fprintf(stderr,"usage:%s filename\n",argv[0]);
		(void)exit(1);
	}

	FBFR32 *pF32；*pF32rec;
	long len;
	FLDLEN32 len2;

	typedef struct tlr
	{
		char name[10];
		long age;
		char phone[12];
		long no;
	}*pTLR;

	int counter,i,ret;
	FILE *fp;
	char *str1=(char *)malloc(100);

	fp=fopen(argv[1],"r");
	if(fp==NULL)
	{
		(void)fprintf(stderr,"%s: file no found!\n",argv[1]);
		(void)exit(1);
	}

	if (tpinit((TPINIT *)NULL) == -1)
	{
		(void)fprintf(stderr, "Failed to join application  -- %s\n",
				tpstrerror(tperrno));
		(void)userlog("Clientfml failed to join application  -- %s\n",
				tpstrerror(tperrno));
		(void)fclose(fp);
		(void)exit(1);
	}

	if ((pF32 = (FBFR32 *)tpalloc("FML32", NULL, Fneeded32(1,30))) == NULL)
	{
		(void)fprintf(stderr, "Failure to allocate FML32 buffer -- %s\n",
				tpstrerror(tperrno));
		(void)userlog("Clientfml failed to allocate FML32 buffer -- %s\n",
				tpstrerror(tperrno));
		(void)tpterm();
		(void)fclose(fp);
		(void)exit(1);
	}

	if ((pF32rec = (FBFR32 *)tpalloc("FML32", NULL, Fneeded32(1,32))) == NULL)
	{
		(void)fprintf(stderr, "Failure to allocate FML32 buffer -- %s\n",
				tpstrerror(tperrno));
		(void)userlog("Clientfml failed to allocate FML32 buffer -- %s\n",
				tpstrerror(tperrno));
		(void)fclose(fp);
		(void)tpterm();
		(void)exit(1);
	}



	pTLR ptlr[3];
	pTLR ptlr2[3];

	for(i=0;i<3;i++)
	{
		ptlr[i]=(pTLR)malloc(sizeof(struct tlr));
		ptlr2[i]=(pTLR)malloc(sizeof(struct tlr));
		if(ptlr[i]==NULL || ptlr2[i]==NULL )
		{
			(void)fprintf(stderr,"mem allocate error!\n");
			(void)fclose(fp);
			(void)exit(1);
		}
	}

	Finit32(pF32,Fsizeof32(pF32));
	Finit32(pF32rec,Fsizeof32(pF32rec));

	i=0;
	while(fgets(str1,100,fp)!=NULL)
	{
		if((ret=sscanf(str1, "%s %ld %s", ptlr[i]->name, &ptlr[i]->age
			, ptlr[i]->phone))<3)
		{
			(void)fprintf(stderr,"error in calling sscanf():%d\n",ret);
			goto str2;
		}

		(void)fprintf(stdout,"[%d] %s %ld %s \n",__LINE__
		, ptlr[i]->name, ptlr[i]->age, ptlr[i]->phone);

		i++;
	}


	for(i=0;i<3;i++)
	{
		if (Fchg32(pF32,NAME	,i, (char *)ptlr[i]->name	, 10) == -1 ||
			Fchg32(pF32,AGE		,i, (char *)&ptlr[i]->age	, 0) == -1 ||
			Fchg32(pF32,PHONE	,i, (char *)ptlr[i]->phone	, 10) == -1 )
		{
			printf("-----%d----\n",__LINE__);
			(void)fprintf(stderr, "Failure to change NAME field -- %s\n"
				, Fstrerror32(Ferror32));
			(void)userlog("Clientfml failed to change NAME field -- %s\n"
				, Fstrerror32(Ferror32));

			goto str2;
		}
	}

	Fprint32(pF32);
	printf("-----<tpcall>%d----\n",__LINE__);

	if (tpcall("FMLTEST", (char *)pF32, 0, (char **)&pF32rec, &len,0) == -1)
	{
		(void)fprintf(stderr, "Failure to call the FMLTEST service -- %s \n"
			, tpstrerror(tperrno));
		(void)userlog("Clientfml failed to call the FMLTEST service -- %s \n"
			, tpstrerror(tperrno));

		goto str2;
	}

	int numofocc=0;
	if ((numofocc = Foccur32(pF32rec,NO)) == -1) {
	    (void)userlog("FMLserver failed to the number of NAME "
			"occurrences -- %s\n", Fstrerror32(Ferror32));
		goto str2;
	 }

	for(i=0;i<numofocc;i++)
	{
		len2=12;
		if (Fget32(pF32rec,NAME	,i, (char *)ptlr2[i]->name	,&len2 ) == -1 ||
			Fget32(pF32rec,NO 	,i, (char *)&ptlr2[i]->no	, NULL ) == -1 )
		{
			(void)fprintf(stderr, "Failure to call get the NO field -- %s \n"
				, Fstrerror32(Ferror32));
			(void)userlog("Clientfml failed to get the NO field -- %s \n"
				, Fstrerror32(Ferror32));

			 goto str2;
			(void)exit(1);
		}

		(void)fprintf(stdout,"[%d]%s--%ld--%s---%ld\n",__LINE__
		, ptlr2[i]->name, ptlr[i]->age, ptlr[i]->phone, ptlr2[i]->no);

	}

	Fprint32(pF32rec);
	(void)tpfree((char *)pF32);
	(void)tpfree((char *)pF32rec);
	(void)free(str1);

	for(i=0;i<3;i++)
	{
		if (NULL!=ptlr[i])(void)free(ptlr[i]);
		if (NULL!=ptlr2[i])(void)free(ptlr2[i]);
	}

	(void)fclose(fp);
	(void)tpterm();

	return(0);

str2:
	(void)tpfree((char *)pF32);
	(void)tpfree((char *)pF32rec);
	(void)free(str1);

	for(i=0;i<3;i++)
	{
		if (NULL!=ptlr[i])(void)free(ptlr[i]);
		if (NULL!=ptlr2[i])(void)free(ptlr2[i]);
	}

	(void)fclose(fp);
	(void)tpterm();

	return(-1);
}

```

### 2、服务端程序

```C
#include <stdio.h>
#include <string.h>
#include "fml32.h"
#include "atmi.h"
#include "userlog.h"
#include "fmlfile.h"

void FMLTEST(TPSVCINFO *msg)
{

	struct emp {
	char name[10];
	long no;
	};

  struct emp aa[3]={
  {"hwtt",33},
  {"csqt",21},
  {"lhtt",42}
  };
  struct emp bb[3];
  FBFR32 *fP32;
  FLDLEN32 len2;
  float inputnum, result;
  int counter, numofocc,i,j;

  fP32 = (FBFR32 *)msg->data;

  if ((numofocc = Foccur32(fP32,NAME)) == -1) {
    (void)userlog("FMLserver failed to the number of NAME occurrences -- %s\n"
		, Fstrerror32(Ferror32));
    tpreturn(TPFAIL, 0, (char *)fP32, 0L, 0);
  }

  if (numofocc == 0) {
    (void)userlog("FMLserver received 0 NAME occurrences -- %s\n",
        Fstrerror32(Ferror32));
    tpreturn(TPFAIL, 0, (char *)fP32, 0L, 0);
  }

  for (counter = 0; counter < numofocc; counter++)
  {
       len2=sizeof(bb[counter].name);
    if (Fget32(fP32,NAME, counter, (char *)bb[counter].name, &len2) == -1)
	{
        (void)userlog("FMLserver failed to get NAME occ %d field -- %s\n",
          counter, Fstrerror32(Ferror32));
        tpreturn(TPFAIL, 0, (char *)fP32, 0L, 0);
     }

 	(void)userlog("FMLserver failed to get NAME=%s \n",bb[counter].name);
   }

  Finit(fP32,sizeof(FLDLEN32 *));
  for(i=0;i<3;i++)
	 for(j=0;j<3;j++)
	 {
	 	if(strcmp(bb[i].name,aa[j].name)==0)
		{
        	 if (Fchg32(fP32,NAME,i, (char *)(aa[j].name),(FLDLEN32)0)==-1 ||
		 	     Fchg32(fP32,NO,i, (char *)(&aa[j].no), (FLDLEN32)0)==-1 )
		 	{
             	(void)userlog("FMLserver failed to add NO field -- %s\n"
					, Fstrerror32(Ferror32));
				tpreturn(TPFAIL, 0, (char *)fP32, 0L, 0);
         	}
	 	}

    }

  tpreturn(TPSUCCESS, 0, (char *)fP32, 0L, 0);

}

```

### 3、fml 域文件

fmlfile
```txt
#编写fmlfile文件，这里一定要注意了，字段必须大写，不然调试半天找不到错误原因的：
#mkfldhdr fmlfile
#name   number  type    flags   comments
*base   1000
NAME    1       string    -     -
AGE     2       long      -     -
PHONE   3       string    -     -
NO      4       long      -     -
FM64    5       fml32     -     -
```
制作头文件
` mkfldhdr32 fmlfile `
**注意：制作fml32 域头文件,注意：mkfldhdr32 和 mkfldhdr 的区别**
 制作fml32 头文件如下：
```
/*	fname	fldid            */
/*	-----	-----            */
#define	NAME	((FLDID32)167773161)	/* number: 1001	 type: string */
#define	AGE	((FLDID32)33555434)	/* number: 1002	 type: long */
#define	PHONE	((FLDID32)167773163)	/* number: 1003	 type: string */
#define	NO	((FLDID32)33555436)	/* number: 1004	 type: long */
#define	FM64	((FLDID32)335545325)	/* number: 1005	 type: fml32 */
```

### 4、Makefile

```Makefile
a
all:  cfg  mkf  stockclient stockserver

cfg:
	#tmloadcf -y uBBconfigunix

mkf:
	mkfldhdr32 fmlfile     #制作fml32 域头文件,注意：mkfldhdr32 和 mkfldhdr 的区别

stockclient:
	buildclient -o clientfml  -f clientfml.c      #本地客户端编译方法
	buildclient -o clientfmlw -f clientfml.c -w   #远程客户端编译方法

stockserver:
	buildserver -o serverfml -f serverfml.c -s FMLTEST  #服务端编译方法
	#buildserver -s QUERY -s BUY -s SELL -s FMLTEST -o serverfml -f serverfml.c

clean:
	rm -f clientfml
	rm -f clientfmlw
	rm -f serverfml
	rm -f *.o  accsss*
cll:
	rm -f clientfml
	rm -f clientfmlw
	rm -f serverfml
	rm -rf  fmlfile.h *.o ULOG.* .adm stderr stdout access* tuxconfig*  uBB*

```

### 5、编译：
` make `

### 6、启动服务

` tmboot -y `
```txt
Booting all admin and server processes in /home/tuxedo/Fmlsrc/tuxconfig
INFO: Oracle Tuxedo, Version 12.2.2.0.0, 64-bit, Patch Level (none)

Booting admin processes ...

exec BBL -A :
	process id=6079 ... Started.

Booting server processes ...

exec WSL -A -- -n //10.10.10.70:3200 -x 10 -m 4 -M 5 -c 1024 -I 30 -N 60 :
	process id=6082 ... Started.
exec serverfml -A :
	process id=6087 ... Started.
3 processes started.
```
 进程启动检测
 ` tmadmin `
 ```sh
tmadmin - Copyright (c) 1996-2016 Oracle.
All Rights Reserved.
Distributed under license by Oracle.
Tuxedo is a registered trademark.

> psc
Service Name Routine Name Prog Name  Grp Name  ID    Machine  # Done Status
------------ ------------ ---------  --------  --    -------  ------ ------
FMLTEST      FMLTEST      serverfml  MATHG+     1     simple       1 AVAIL

> psr
Prog Name Queue Name 2ndQueue Name Grp Name ID RqDone Load Done Current Service
--------- ---------- ----------    -------- -- ------ --------- ---------------
BBL       155442                   simple    0      0         0 (  IDLE )
serverfml 00011.00001              MATHGRP   1      1        50 (  IDLE )
WSL       00010.00010              GROUPWSL 10      0         0 (  IDLE )

```


### 7、执行客户端程序

1) 执行本地客户端
` ./clientfml tlr.txt `
```TXT
    -----<tpcall>126----
[160]hwtt--29--18677150924---33
[160]lhtt--28--13398813422---42
[160]csqt--25--13234234564---21
```

2) 执行远程客户端
 ` ./clientfmlw tlr.txt  `
  **注意：需要在该远程客户端配置 WSNADDR=//ServerIP:Port**

### 8、停止服务
` tmshutdown -ycw1  `
```TXT
[tuxedo@c700 Fmlsrc]$ tmstop
Shutting down all admin and server processes in /home/tuxedo/Fmlsrc/tuxconfig

Shutting down server processes ...

	Server Id = 1 Group Id = MATHGRP Machine = simple:	shutdown succeeded
	Server Id = 10 Group Id = GROUPWSL Machine = simple:	shutdown succeeded

Shutting down admin processes ...

	Server Id = 0 Group Id = simple Machine = simple:	shutdown succeeded
3 processes stopped.

```
## 小结：
总结以下问题：
1.指针使用前要提前分配空间（malloc）
2.FML格式文件字段要大写
3.如果读取的是"|"分隔的文件，sscanf中应该用"%[^|]|%ld|%s"这样的格式
程序已经调试通过了，主要是练习FML的用法，希望能对大家学习有帮助。
 注意：
 1、fmlfile 转换为 fmlfile.h 时FLDID 和 FLDID32 的区别;
 2、ubb 中路径的修改
 3、远程客户和服务端配置;
    1）客户端仅需要配置TUXDIR 、WSNADDR 环境变量
    2）远程客户端编译时添加 -w 参数
    3）服务端仅需要在 ubb 中的
    在 *MACHINES  添加 MAXWSCLIENTS 配置
    在 *SERVERS   添加 WSL 配置

 ---------------------------------------------------------
