set esurl='http://localhost:9200/'
curl -H "Content-Type: application/json" -XPUT $esurl'config'
curl -H "Content-Type: application/json" -XPUT $esurl'devices'
curl -H "Content-Type: application/json" -XPUT $esurl'task'
curl -H "Content-Type: application/json" -XPOST $esurl'config/config/task' -d '{"list" : 
[
"alipay",
"facebookMessage",
"firechat",
"imo",
"meetup",
"qq",
"quora",
"skype",
"tumblr",
"twitter",
"viber",
"wechat",
"yixin"
    ]}'

