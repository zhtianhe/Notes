#!/bin/bash
IP=$1
PORT=$2
FileCfg=$HOME/conf/redis*.conf
IpStr=(	"10.10.10.76 6371" "10.10.10.76 6372" "10.10.10.76 6373" 
	"10.10.10.76 6374" "10.10.10.76 6375" "10.10.10.76 6376" 
	"10.10.10.76 6377" "10.10.10.76 6378" "10.10.10.76 6379" )

	
AUTH=$(sed -n '/^\<masterauth\>\|^\<requirepass\>/p' ${FileCfg}|awk -F' ' 'BEGIN{i=0}{if($1 ~ /masterauth/){S[0]=$2}} END{printf "-a " S[0];}') 

#函数处理部分
function checkoutIPort(){
    echo "1、check ${#IpStr[@]} IP Port"
    for((i=0;i<${#IpStr[@]};i++));do  
    	if [ $(echo "\n"|telnet ${IpStr[$i]}   2>/dev/null|grep Connected|wc -l ) -ge 1 ];then 
    		echo "[$i]: ${IpStr[$i]} OK";
    		IP=${IpStr[$i]%% *}
    		PORT=${IpStr[$i]##* }
    		#echo "  $IP--->$PORT"
    	else 
    		echo "[$i]: ${IpStr[$i]} -1 Err";
    	fi ;
    done
}

function checkoutServer(){
   echo "2、check redis-server"
   ps -aux | grep 'redis-server'|egrep -v 'grep'
#printf("[%02s]|%14s|%22s|%40s|%40s|%9s|%13s|%12s|%10s\n","NO","master/slave","IpPort","MyId","MastetId","ping-sent","pong-recv","config-epoch","link-state");
}

function checkoutCluster(){
    echo "3、check $IP $PORT cluster status"
    if [ ! -z $IP ];then
    echo -e "\e[32m$(printf "[%12s]|%14s|%22s|%40s|%8s|%9s|%13s|%12s|%10s" "NO" "master/slave" "IpPort" "MyId" "MastetId" "ping-sent" "pong-recv" "config-epoch" "link-state";)\e[0m"
    redis-cli -c -h $IP -p $PORT $AUTH \
    	CLUSTER NODES 2>/dev/null \
    	|awk 'BEGIN{
    	i=0;
    	j=0;
    }
    {       S[i][0]=$1;
    	S[i][1]=$2;
    	S[i][2]=$3;
    	S[i][3]=$4;
    	S[i][4]=$5;
    	S[i][5]=$6;
    	S[i][6]=$7;
    	S[i][7]=$8;
    	S[i][8]=$9;
    	if($3 ~/\<master\>/){
    		M[j][0]=$1;
    		M[j][1]=$9;
    		j++;
    	};
    i++;
    }
    END{
    for(j in M){
    	printf("[%12s]=[32m%40s[0m\n",M[j][1],M[j][0]);
    	for ( i in S ){
    		if( S[i][0] == M[j][0] ) {
    			printf("[%12s]|%14s|%22s|%40s|%8.8s|%9s|%13s|%12s|%10s\n",M[j][1],S[i][2],S[i][1],S[i][0],S[i][3],S[i][4],S[i][5],S[i][6],S[i][7]);
    		}else if(S[i][3] == M[j][0]){
    		printf("[%12s]|%14s|%22s|%40s|%8.8s|%9s|%13s|%12s|%10s\n",M[j][1],S[i][2],S[i][1],S[i][0],S[i][3],S[i][4],S[i][5],S[i][6],S[i][7]);
    	}
    
    }
    }
    
    }' |sort -n -t -  -k 2
    fi
}

function main(){
    echo "${LINENO}" "redis-cli -c -h $IP -p $PORT"
    checkoutIPort "$@"
    echo "${LINENO}" "redis-cli -c -h $IP -p $PORT"
    checkoutServer "$@"
    echo "${LINENO}" "redis-cli -c -h $IP -p $PORT"
    checkoutCluster "$@"
}

main "$@"
