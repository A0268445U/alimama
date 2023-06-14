echo "this is NODE-$NODE_ID"
if [[ "$NODE_ID" == "1" || "$NODE_ID" == "2" || "$NODE_ID" == "3" ]]; then
	# 启动节点1对应服务
	cd entrance
	./entrance $NODE_ID
	echo "Register Executed"
	sleep 1000;
else
	# 启动其他节点对应服务
  sleep 1000;
fi

#cd entrance
#./entrance $NODE_ID
#echo "Register Executed"

#sleep 1000


