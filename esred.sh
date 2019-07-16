NODE="eMs61uH" 
IFS=$'\n' 
for line in $(curl -s '192.168.1.57:9200/_cat/shards' | fgrep UNASSIGNED); do INDEX=$(echo $line | (awk '{print $1}')) SHARD=$(echo $line | (awk '{print $2}')) curl -H 'Content-Type: application/json' -XPOST '192.168.1.57:9200/_cluster/reroute' -d '{
     "commands": [
        {
            " allocate_replica ": {
                "index": "'$INDEX'",
                "shard": '$SHARD',
                "node": "'$NODE'",
                "allow_primary": true
          }
        }
    ]
  }' 
done
