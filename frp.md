
#  远程链内网 Win10 远程桌面 （frp + 公网IP）   


- [frp 下载地址](https://github.com/fatedier/frp/releases)
- [Windows10 64位系统设置FRPC开机自动启动](https://www.ctoclubs.com/2019/03/18/19/14/2447/2447.html)


## win10 frpc 配置
frpc.ini
```ini
[common]
#server_addr = 127.0.0.1
server_addr = 公网IP
server_port = 7000

[ssh]
type = tcp
local_ip = 127.0.0.1
local_port = 3389
remote_port = 3389
#local_port = 22
#remote_port = 6000
```

## 开机启动配置：

### 启动 frpc 的 VBS 脚本
frpc.vbs :将启动 frpc 程序并放入后台 
```vbs
set ws=WScript.CreateObject("WScript.Shell")
ws.Run "D:\App\frp_0.37.1_windows_amd64\frpc.exe -c D:\App\frp_0.37.1_windows_amd64\frpc.ini",0

```

### 检测 frpc.exe 运行状态
 
 frpc.bat ：每隔五秒检查一次，防止没有网络启动失败。

```bat
::将脚本放入后台
@echo off

　　if "%1" == "h" goto begin 
　　mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit 
　　:begin 

:start
::(5秒执行一次下面的指令）
choice /t 5 /d y /n >nul 

::（检查是否存在frpc.exe进程，如果检测到，下面比较的值为0，为0表示存在。）
tasklist|find /i "frpc.exe" 
if %errorlevel%==0 ( 
	@echo "yes"
) else (
	@echo "No" 
	start frpc.vbs
)

goto start  

```

##  WinSW 工具 将 bat 脚本提升为服务 开机启动
	
	win10 开机启动方法：
	- 加入任务执行计划，似乎没有执行成功。
	- 加入启动目录，只有登陆用户时才触发。

[WinSW 下载地址](https://github.com/winsw/winsw)

winsw.xml

```xml
<service>  
  <id>frp</id>  
  <name>frp</name>  
  <description>service runs frp</description>  
  <executable>frpc.bat</executable>  
  <arguments></arguments>
  <logmode>reset</logmode>
  <serviceaccount>
    <username>LocalSystem</username>
  </serviceaccount>
</service>

```

## 安装/启动/停止/卸载
```bat
WinSW-x64.exe install    winsw.xml 
WinSW-x64.exe start      winsw.xml
WinSW-x64.exe stop       winsw.xml
WinSW-x64.exe uninstall  winsw.xml
Win+R打开运行窗口,输入”Services.msc” 手动修改为自启动
```

# 参考

- [利用云服务器搭建远程办公访问（frp实现内网穿透）](https://www.cnblogs.com/fuzhuoxin/p/14146635.html)
- [Windows10 64位系统设置FRPC开机自动启动](https://www.ctoclubs.com/2019/03/18/19/14/2447/2447.html)

# 问题
 - 如何多用户同时登陆 win10 桌面？

