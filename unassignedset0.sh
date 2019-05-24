#!/bin/bash
host="192.168.1.49"
for index in $(curl -s 'http://'$host':9200/_cat/shards' | grep UNASSIGNED | awk '{print $1}' | sort | uniq); do
	echo '\r\r'$index' =>'
	curl -XPUT 'http://'$host':9200/'$index'/_settings' -d '{ "number_of_replicas": 0 }' -H 'Content-Type: application/json'
	echo '\r\r'
        sleep 1
done
