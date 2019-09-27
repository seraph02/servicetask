显示所有索引为task type为typeinfo的前10条
http://localhost:9200/task/taskinfo/_search?pretty=true
创建INDEX
curl -XPUT "http://localhost:9200/task/"
添加
curl -H "Content-Type: application/json" -XPOST 'http://localhost:9200/task/taskinfo?pretty=true' -d '{"progress": 0,"key":["1550654901"], "ctime": 1550654901, "status": 1, "info": ["qq"]}'

http://localhost:9200/task/_search?q=status:1

curl -XDELETE localhost:9200/task/taskinfo/id

curl -XPUT "http://localhost:9200/config"
curl -H "Content-Type: application/json" -XPOST 'http://localhost:9200/config/config/task?pretty=true' -d '{"list":["alipay","facebookmessage","firechat","imo","meetup","qq","quora","skype","tumblr","twitter","viber","wechat","yixin"]}'





./esm -d http://192.168.1.57:9200 -y "keydamara032" --refresh -i=keydamara032.bin
./esm -s http://192.168.1.59:9200 -x "keydamara032" --refresh -o=keydamara032.bin

PUT telegramgroup1
{
  "settings": {
    "default_pipeline": "my_timestamp_pipeline"
  }
}
PUT _ingest/pipeline/my_timestamp_pipeline
{
  "description": "Adds a field to a document with the time of ingestion",
  "processors": [
    {
      "set": {
        "field": "@timestamp",
        "value": "{{_ingest.timestamp}}"
      }
    }
  ]
}

//health is red ,
curl -XPUT "http://localhost:9200/_settings" -d' {  "number_of_replicas" : 0 } '


curl -X POST "localhost:9200/twitter/_doc/_delete_by_query?conflicts=proceed" -H 'Content-Type: application/json' -d'
{
  "query": {
    "match_all": {}
  }
}

curl -X PUT "http://192.168.1.57:9200/task1" -H 'Content-Type: application/json' -d '
{
    "settings" : {
        "index" : {
            "number_of_shards" : 1, 
            "number_of_replicas" : 1
        }
    },
    "mappings": 
    {
      "_default_": { 
        "properties": 
        { 
          "ctime": { 
            "type": "date",
            "format": "strict_date_optional_time||epoch_second"
          },
          "ptime": {
            "type": "date",
            "format": "strict_date_optional_time||epoch_second"
          },
          "etime": { 
            "type": "date",
            "format": "strict_date_optional_time||epoch_second"
          }
        }
      }
    }
}'
POST /_aliases
{
    "actions" : [
        { "add" : { "index" : "task1", "alias" : "task" } }
    ]
}
curl -XPUT "http://192.168.7.13:9200/devices" -H 'Content-Type: application/json' -d '
{
    "settings" : {
        "index" : {
            "number_of_shards" : 3, 
            "number_of_replicas" : 2 
        },
	"index.write.wait_for_active_shards": "2"
    }
}'
PUT devices
{
  "mappings": {
    "info": {
      "_routing": {
        "required": true 
      }
  }
}
curl -XPOST http://192.168.1.131:9200/task/taskinfo/UE_53moBrOxxijhUjtXf/_update -d '{ "doc":{"status":4}}' -H 'Content-Type: application/json'

curl -X PUT "http://192.168.1.66:9200/ip2location" -H 'Content-Type: application/json' -d '
{
  "mappings": {
    "data": { 
      "properties": { 
        "ipnum": { 
          "type": "long_range" 
        },
        "country_code":{
          "type":"text"
        },
        "city_name":{
          "type":"text"
        },
        "country_name":{
          "type":"text"
        },
        "region_name":{
          "type":"text"
        },
        "location" : {
          "type" : "geo_point"
        }
      } 
    }
  }
}'
POST ip2location/data/?pretty
{
  "country_code":"CN",
  "city_name":"Xi'an",
  "ipnum":{
    "gte": 22104832,
    "lte": 22105087
  },
  "country_name":"China",
  "location":{
    "lon":108.92861,
    "lat":34.25833
  },
  "region_name":"Shaanxi"
}

GET ip2location/data/_search? 
{ 
  "query" : { 
    "term" : { 
      "ipnum": { 
        "value": 22104842
      }
    }
  }
}

get ip2location/data/_search?
{
    "query": {
        "bool" : {
            "must" : {
                "match_all" : {}
            },
            "filter" : {
              "geo_distance": {
                "distance": "100km",
                "location": {
                  "lat": 34.25833,
                  "lon": 108.92861
                }
              }
            }
        }
    }
}
curl -XPUT 'http://192.168.1.99:9200/_template/template_http_request_record' -H 'Content-Type: application/json' -d '{"index_patterns": ["record_*"],"settings": {"number_of_shards": 1,"number_of_replicas": 1}}'


PUT my_index
{
  "settings": {
    "default_pipeline": "my_timestamp_pipeline"
  }
}

PUT _ingest/pipeline/my_timestamp_pipeline
{
  "description": "Adds a field to a document with the time of ingestion",
  "processors": [
    {
      "set": {
        "field": "ingest_timestamp",
        "value": "{{_ingest.timestamp}}"
      }
    }
  ]
}

