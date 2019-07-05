#!/bin/bash
pidnum=`ps -ef|grep java |grep "file.encoding=utf-8"|grep -v grep|awk '{print $2}'`
if [ "$pidnum" != "" ]
	then
	for i in $pidnum
	do
	echo "$i"
	kill $i
	wait $i
	done
fi

