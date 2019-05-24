#!/bin/bash
node="kmP6f7KFSTmyjN5rgYubvQ"
host="192.168.1.49"
for index in $(curl -s 'http://'$host':9200/_cat/shards' | grep UNASSIGNED | awk '{print $1}' | sort | uniq); do
    for shard in $(curl  -s 'http://'$host':9200/_cat/shards' | grep UNASSIGNED | grep $index | awk '{print $2}' | sort | uniq); do    
        echo  $index $shard
        curl -XPOST $host':9200/_cluster/reroute' -d '{
            "commands" : [ {
                  "allocate" : {
                      "index" : "$index",
                      "shard" : $shard,
                      "node" : "$node",
                      "allow_primary" : true
                  }
                }
            ]
        }' -H 'Content-Type: application/json'

        sleep 5
    done
done
