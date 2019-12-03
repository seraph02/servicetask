#include "expclassify.h"
#include "comm.h"
#include "dbput.h"
#include "bin2ascii.h"
#include "jsoncpp/json/json.h"
#include "sstream"
#include "glog/logging.h"
#include <boost/algorithm/string.hpp>
using std::ostringstream;
expclassify::expclassify()
{

}
void expclassify::Go(Json::Value args)//string result,string appname,string key)
{
    string localip=::GetLocalIP();
    string esport="9200";
    dbput::getInstance()->ChangeHosts({"http://"+localip+":"+esport+"/"});

    LOG(INFO)<<"ES CONNECT "<<"http://"+localip+":"+esport+"/";
    string info=args["info"].asString();
    string taskid=args["taskid"].asString();
    string indices =args["indices"].asString();
    string key=args["key"].asString();
    string result=args["result"].asString();
    int datacount = PUSHRemoteDataCF(info,taskid,key,indices,result);

}

int expclassify::PUSHRemoteDataCF( string info,string taskid,string strkey,string indices ,string resultjson)
{
    //-t fbmcontrol -k 133 -b eyJjb250cm9sRmlsZVBhdGgiOiIvaG9tZS90d2Qvc291cmNlL3NlcnZpY2V0YXNrL1BPQy9kb3dubG9hZC9mYWNlYm9va01lc3NhZ2UvY29udHJvbERhdGEvMzI3NDI0NDA1OEBxcS5jb20iLCJjb250YWN0UGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9jb250YWN0cy5qc29uIiwic3RhdHVzIjoib2siLCJtZXNzYWdlUGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9tZXNzYWdlLmpzb24ifQ==

    int datacount=-1;

    string strret= ReadLocalFile("dataresult");
    LOG(INFO)<< " => " << strret;
    if(strret.compare("-1")==0 || strret.size()<3) return true;

//            result.set_dataclassify(strret);

//result dataclassify
    if(strret.size()>10)
    {
//        task->set_datacount(task->datacount()+1);
        Json::Value jdataclassify;
        Json::Reader jread;
        jread.parse(strret,jdataclassify);
        if(!jdataclassify.isNull()&&jdataclassify.isArray())
        {
            for(int i=0;i<jdataclassify.size();i++)
            {
                Json::Value jelement = jdataclassify[i];
                Json::Value jsondata;
                time_t time_now; time(&time_now);
                jsondata["spidedate"] = std::to_string(time_now);
                jsondata["taskid"]=taskid;
                jsondata["task"]=info;
                string type =jelement["type"].isString()? jelement["type"].asString():"unknow";//message  or contacts

                Json::FastWriter jfw;
                std::string strmessageid="";
                try{
                    //voxerid
                    string messageid = jelement["body"]["message_id"].asString();
                    if(messageid.empty())
                    {
                        messageid=jelement["body"]["docid"].asString();
                        boost::to_lower(messageid);
                        boost::replace_all(messageid,"/","");
                        boost::replace_all(messageid,"\\","");
                        boost::replace_all(messageid,"$","");
                        jelement["body"]["docid"]=messageid;
                    }
                    strmessageid.append(messageid);
                }catch(...){}
                jsondata[type]=jelement["body"];
                try{
                    //imo is no id
//                    string messageid = jelement["body"]["message_id"].asString();
//                    strmessageid.append(messageid);
                }catch(...){}
                std::string strpostdata=jfw.write(jsondata);
                if(strmessageid.empty()||strmessageid.size()<2) dbput::getInstance()->POSTTaskResult(indices,strpostdata);
                else
                {
//                    if(strmessageid.find("oAAAAABh2uHo_CTVeEwAAAB4zAKEx")!=string::npos)
//                    {
//                        LOG(ERROR)<<"1";
//                    }
                    boost::to_lower(strmessageid);
                    boost::replace_all(strmessageid,"/","");
                    boost::replace_all(strmessageid,"\\","");
                    boost::replace_all(strmessageid,"$","");
                    try{
                    //LOG(INFO)<<strmessageid;
                    dbput::getInstance()->POSTTaskResult(indices,strmessageid,strpostdata);
                    }
                    catch(...)
                    {
//                        LOG(ERROR)<<"aaa";
                    }
                }
                if(type.compare("message")==0){  datacount++;}

            }

        }
    }
    return datacount;
}
