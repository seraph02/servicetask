显示所有索引为task type为typeinfo的前10条
http://localhost:9200/task/taskinfo/_search?pretty=true
创建INDEX
curl -XPUT "http://localhost:9200/task/"
添加
curl -H "Content-Type: application/json" -XPOST 'http://localhost:9200/task/taskinfo?pretty=true' -d '{"progress": 0,"key":"1550654901", "ctime": 1550654901, "status": 1, "info": ["skype", "qq", "firechat"]}'

http://localhost:9200/task/_search?q=status:1

curl -XDELETE localhost:9200/task/taskinfo/id

curl -XPUT "http://localhost:9200/config"
curl -H "Content-Type: application/json" -XPOST 'http://localhost:9200/config/config/task?pretty=true' -d '{"list":["alipay","facebookmessage","firechat","imo","meetup","qq","quora","skype","tumblr","twitter","viber","wechat","yixin"]}'





./esm -d http://192.168.1.57:9200 -y "keydamara032" --refresh -i=keydamara032.bin
./esm -s http://192.168.1.59:9200 -x "keydamara032" --refresh -o=keydamara032.bin



//health is red ,
curl -XPUT "http://localhost:9200/_settings" -d' {  "number_of_replicas" : 0 } '



