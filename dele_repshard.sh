curl -s '192.168.1.57:9200/_cat/shards' | fgrep UNASSIGNED | awk '{print $1}' >aa.txt
sort -n aa.txt | uniq > bb.txt



for line in `cat bb.txt`
do
 echo $line'\n'
 curl -H 'Content-Type: application/json' -XPUT '192.168.1.57:9200/'$line'/_settings' -d '{ "index" : { "number_of_replicas" :0 } }'
done
