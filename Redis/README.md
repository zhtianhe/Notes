

.
├── redis-benchmark   redis 自带测试工具
├── redis-check-aof
├── redis-check-rdb
├── redis-cli         redis 客户端
├── redis-create.sh   创建集群脚本
├── redis-sentinel -> redis-server
├── redis-server       redis 客户端
├── redis-start-all.sh redis 启动脚本
├── redis-status.sh    redis 集群查看脚本
└── redis-stop.sh      redis 停止脚本


# 创建集群 三主六从 ( 3x 一主两从 )

` redis-create.sh `

# 查看集群状态
` redis-status.sh `

```
77 redis-cli -c -h  -p 
1、check 9 IP Port
[0]: 10.10.10.76 6371 OK
[1]: 10.10.10.76 6372 OK
[2]: 10.10.10.76 6373 OK
[3]: 10.10.10.76 6374 OK
[4]: 10.10.10.76 6375 OK
[5]: 10.10.10.76 6376 OK
[6]: 10.10.10.76 6377 OK
[7]: 10.10.10.76 6378 OK
[8]: 10.10.10.76 6379 OK
79 redis-cli -c -h 10.10.10.76 -p 6379
2、check redis-server
redis      2510  0.1  0.4 243316 16168 ?        Ssl  10:06   0:08 redis-server 10.10.10.76:6371 [cluster]
redis      2513  0.1  0.5 249460 20324 ?        Ssl  10:06   0:08 redis-server 10.10.10.76:6372 [cluster]
redis      2518  0.1  0.4 243316 16000 ?        Ssl  10:06   0:08 redis-server 10.10.10.76:6373 [cluster]
redis      2529  0.1  0.4 243316 16200 ?        Ssl  10:06   0:07 redis-server 10.10.10.76:6374 [cluster]
redis      2535  0.1  0.2 165488 10040 ?        Ssl  10:06   0:08 redis-server 10.10.10.76:6375 [cluster]
redis      2541  0.1  0.5 249460 20296 ?        Ssl  10:06   0:08 redis-server 10.10.10.76:6376 [cluster]
redis      2548  0.1  0.4 243316 16160 ?        Ssl  10:06   0:08 redis-server 10.10.10.76:6377 [cluster]
redis      2554  0.1  0.2 165488 10248 ?        Ssl  10:06   0:07 redis-server 10.10.10.76:6378 [cluster]
redis      2562  0.1  0.2 165488 10072 ?        Ssl  10:06   0:08 redis-server 10.10.10.76:6379 [cluster]
81 redis-cli -c -h 10.10.10.76 -p 6379
3、check 10.10.10.76 6379 cluster status
[          NO]|  master/slave|                IpPort|                                    MyId|MastetId|ping-sent|    pong-recv|config-epoch|link-state
[      0-5460]=313f6c01b1394bd282a6765f2b1b27589c7b6d56
[      0-5460]|        master|10.10.10.76:6378@16378|313f6c01b1394bd282a6765f2b1b27589c7b6d56|       -|        0|1624246059904|          15| connected
[      0-5460]|         slave|10.10.10.76:6371@16371|c6992aa5a096b2e1b1f1007b36f8bb45e7b47bd6|313f6c01|        0|1624246061000|          15| connected
[      0-5460]|         slave|10.10.10.76:6377@16377|1fc76c712f90c7db84dda24342f463b9fb9b81ec|313f6c01|        0|1624246062017|          15| connected
[  5461-10922]=a6eda68d451fcac41fa8ff9425f9b6ba4e42be13
[  5461-10922]|        master|10.10.10.76:6375@16375|a6eda68d451fcac41fa8ff9425f9b6ba4e42be13|       -|        0|1624246058000|          17| connected
[  5461-10922]|         slave|10.10.10.76:6372@16372|b7d364a111018ce6fc7012b378abcb5432f95df2|a6eda68d|        0|1624246058000|          17| connected
[  5461-10922]|         slave|10.10.10.76:6376@16376|2d63dae47f61f834250a32b7e774e73eff67b1f4|a6eda68d|        0|1624246058000|          17| connected
[ 10923-16383]=ff401b04b2ab0a95cc939620273ff92a0789f6be
[ 10923-16383]| myself,master|10.10.10.76:6379@16379|ff401b04b2ab0a95cc939620273ff92a0789f6be|       -|        0|1624246059000|          14| connected
[ 10923-16383]|         slave|10.10.10.76:6373@16373|bda2cfe32cc0fe26067b55c5fdbd414cdb7a1cce|ff401b04|        0|1624246058845|          14| connected
[ 10923-16383]|         slave|10.10.10.76:6374@16374|1c3d9b93f2ca0cee5347028183bc947916a00700|ff401b04|        0|1624246060949|          14| connected
````
