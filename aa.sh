#!/bin/sh
key="aa";
fl=./test.txt
count=`cat $fl | wc -l`
for i in `cat $fl`
do
    key=$i
    curl -H "Content-Type: application/json" -XPOST 'http://localhost:9200/task/taskinfo?pretty=true' -d '{"key" : ["'$key'"],"ctime" : '`date +%s`',"status" : 1,"type" : 1,"info" : ["wechat"],"progress" : 0}'
done
 

