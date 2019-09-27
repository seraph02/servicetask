
urlencode() {

echo $1|tr -d '\n' |od -An -tx1|tr ' ' %
}

cat aa.txt |while read line
do
#echo $line
#enline=`urlencode $line`
enline=$line
echo $enline
#ab=`curl -XPOST http://192.168.1.86:9200/db_sum/data/_search?pretty -d '{  "query":  {    "bool": {      "must": [        {          "match_phrase": {            "userid": "'$enline'"     }     }   ]   }  } }' -H 'Content-Type: application/json' |grep phone |sed '1d'|awk '{print $3}'|grep -o '[0-9]\+'`
  ab=`curl -XPOST http://192.168.1.86:9200/db_sum/data/_search?pretty -d '{  "query":  {    "bool": {      "must": [        {          "match_phrase": {            "userid": "'$enline'"     }     }   ]   }  } }' -H 'Content-Type: application/json' |grep phone |awk '{print $3}'|grep -o '[0-9]\+'`
  echo $ab
  if test -z "$ab"
  then
   echo ""
  else
	  if [ $ab -ne "0" ];then  
		echo $line","$ab >>curl.txt
	  fi
  fi
  
done