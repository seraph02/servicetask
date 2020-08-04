#!/bin/sh
key="86111111111111";
fl=./test.txt
count=`cat $fl | wc -l`
for i in `cat $fl`
do
    key=$i
    curl -H "Content-Type: application/json" -XPUT 'http://192.168.1.185:9200/'$key'-00001' -d '{  "aliases": {    "'$key'": {      "is_write_index": true    }  }}'
done


