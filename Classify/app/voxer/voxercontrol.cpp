#include "voxercontrol.h"
#include "comm.h"
#include <glog/logging.h>
#include "appconf.h"
void VOXERControl::filteravatar(Json::Value& jones)
{
    if(jones.isMember("avatarUrl"))
    {
        string avatar=jones["avatarUrl"].asString();
        if(avatar.empty()){return;}
        string atttype="avatar";
        //down avatar file
        string path_dir = APPCONF::getInstance()->refpath + atttype +"/"+app->getkey()+"/";
        checkdir(path_dir);
        string fileext=getfileext(avatar);
        string path= path_dir + filename4time(fileext);
        if(DOWNLOAD_FILE(avatar.c_str(),path.c_str())==0)
        {
            jones["avatarUrl"]=(GetCurrPath()+path).c_str();
        }

    }
}
void VOXERControl::ProcessMessage(Json::Value jbody)
{
    if(jbody.isArray())
    {
        Json::Value jarray;
        for(int ind =0;ind<jbody.size();ind++)
        {
            Json::Value jones = jbody[ind];
            //LOG(INFO)<<jones.toStyledString();
            Json::Value j_content_type = jones["content_type"];
            if(!j_content_type.isNull())
            {
                string content_type = j_content_type.asString();
                if(content_type.find("receipt")!=string::npos||content_type.find("complete")!=string::npos||content_type.find("recall_messages")!=string::npos)
                {
                    continue;
                }
                else if(content_type.find("profile")!=string::npos){
                    ProcessArray(jarray,"profile",0);
                }

                jarray.append(jones);
            }
        }
        ProcessArray(jarray,"message",0);
    }



}
void VOXERControl::filtermessage(Json::Value& jones)
{
//    Json::Value content = jones["content_json"];

//    string atttype=content["local"].asString();
//    LOG(INFO)<<"ATTTYPE: "<<atttype;
//    if(atttype.empty()) return;
//    //down media file
//    if(atttype.rfind("text")!=string::npos) {

//    }
//    else if(atttype.rfind("media")!=string::npos) {
//        string mediaurl = jones[atttype].asString();
//        string path_dir = APPCONF::getInstance()->refpath + atttype +"/"+app->getkey()+"/";
//        checkdir(path_dir);
//        string fileext=getfileext(mediaurl);
//        string path= path_dir + filename4time(fileext);
//        DOWNLOAD_FILE(mediaurl.c_str(),path.c_str());
//        jones[atttype]=(GetCurrPath()+path).c_str();

//    }
//    else{}
    absControl::filtermessage(jones);
}
