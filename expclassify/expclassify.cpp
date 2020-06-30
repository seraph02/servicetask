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
    PUSHRemoteDataCF(info,taskid,key,indices,result);

}

int expclassify::PUSHRemoteDataCF( string info,string taskid,string strkey,string indices,string resultjson)
{
    //-t fbmcontrol -k 133 -id abcde -b eyJjb250cm9sRmlsZVBhdGgiOiIvaG9tZS90d2Qvc291cmNlL3NlcnZpY2V0YXNrL1BPQy9kb3dubG9hZC9mYWNlYm9va01lc3NhZ2UvY29udHJvbERhdGEvMzI3NDI0NDA1OEBxcS5jb20iLCJjb250YWN0UGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9jb250YWN0cy5qc29uIiwic3RhdHVzIjoib2siLCJtZXNzYWdlUGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9tZXNzYWdlLmpzb24ifQ==
    //info=fbmcontrol
    //taskid=abcde
    //strkey=133
    //indices=key133
    //{"args":"-o=twd1q2w3e4r","contactPath":"/home/twd/source/POC/result/1194093570@qq.com/contacts.json","status":"ok"}
    int datacount=-1;

        string file = "dataresult";
        int linenum = getFiletotals(file);



        LOG(INFO)<< "dataresult => " << linenum << " ";
        if(linenum<1) return 0;

            for(int i=0;i<linenum;i++)
            {
                string bodystr=ReadLocalFile(file,i+1);
                string other=bodystr;
                if(other.size()<3) continue;
        //        task->set_datacount(task->datacount()+1);
                Json::Value jdataclassify;
                Json::Reader jread;
                jread.parse(other,jdataclassify);
                if(!jdataclassify.isNull())
                {
                Json::Value jelement = jdataclassify;
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
    return datacount;
}
//bool expclassify::PUSHRemoteDataCF( string info,TaskInfo* task,string strkey,string indices ,string resultjson)
//{
//    //-t fbmcontrol -k 133 -b eyJjb250cm9sRmlsZVBhdGgiOiIvaG9tZS90d2Qvc291cmNlL3NlcnZpY2V0YXNrL1BPQy9kb3dubG9hZC9mYWNlYm9va01lc3NhZ2UvY29udHJvbERhdGEvMzI3NDI0NDA1OEBxcS5jb20iLCJjb250YWN0UGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9jb250YWN0cy5qc29uIiwic3RhdHVzIjoib2siLCJtZXNzYWdlUGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9tZXNzYWdlLmpzb24ifQ==
//    string body = b64_encode(resultjson);

//    ostringstream sscmd;
//    sscmd<< "./"+dcfexename+" ";
//    sscmd<<" -t " << info << " -k "<< strkey << " -b " <<body;

//    string DataClassifycmd;
//    DataClassifycmd = sscmd.str();
//    RunShell("echo \"\" >dataresult");//??initdataresult
//    LOG(INFO)<<"runshell :"<<DataClassifycmd.c_str();
//    RunShell(DataClassifycmd.c_str());

////    string put2dbcmd;
////    //sscmd.clear();
////    sscmd<<" -id "<<task->id();
////    put2dbcmd = sscmd.str();
////    LOG(INFO)<<"runshell :"<<put2dbcmd.c_str();
////    RunShell(put2dbcmd.c_str());


//    //string strret= ReadLocalFile("dataresult");//
//    string file = "dataresult";
//    int linenum = getFiletotals(file);



//    LOG(INFO)<< "dataresult => " << linenum << " ";
//    if(linenum<1) return true;

////            result.set_dataclassify(strret);

////result dataclassify

//        for(int i=0;i<linenum;i++)
//        {
//            string bodystr=ReadLocalFile(file,i+1);
//            string other=bodystr;
//            if(other.size()<3) continue;
//    //        task->set_datacount(task->datacount()+1);
//            Json::Value jdataclassify;
//            Json::Reader jread;
//            jread.parse(other,jdataclassify);
//            if(!jdataclassify.isNull())
//            {

//                Json::Value jelement = jdataclassify;
//                Json::Value jsondata;
//                time_t time_now; time(&time_now);
//                jsondata["spidedate"] = std::to_string(time_now);
//                jsondata["taskid"]=task->id();
//                jsondata["task"]=info;
//                string type =jelement["type"].isString()? jelement["type"].asString():"unknow";//message  or contacts
//                jsondata[type]=jelement["body"];
//                Json::FastWriter jfw;
//                std::string strmessageid="";
//                try{
//                    //voxerid
//                    string messageid = jelement["body"]["message_id"].asString();
//                    if(messageid.empty()) messageid=jelement["body"]["docid"].asString();
//                    strmessageid.append(messageid);
//                }catch(...){}
//                try{
//                    //imo is no id
////                    string messageid = jelement["body"]["message_id"].asString();
////                    strmessageid.append(messageid);
//                }catch(...){}
//                std::string strpostdata=jfw.write(jsondata);
//                bool puttrue = true;
//        if(strmessageid.empty()||strmessageid.size()<2) puttrue = Manager_ES::getInstance()->POSTTaskResult(indices,info,strpostdata);
//                else
//                {
//                    boost::to_lower(strmessageid);
//                    boost::replace_all(strmessageid,"/","");
//                    boost::replace_all(strmessageid,"\\","");
//                    boost::replace_all(strmessageid,"$","");
//                    puttrue = Manager_ES::getInstance()->POSTTaskResult(indices,info,strmessageid,strpostdata);
//                }
//                if(type.compare("message")==0 && puttrue ){  task->set_datacount(task->datacount()+1);}
//            }
//    }
//    return true;
//}
