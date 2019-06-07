#include "fbmcontrol.h"

#include "comm.h"
#include <glog/logging.h>
#include "appconf.h"


void FBMControl::filteravatar(Json::Value& jones)
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

//void FBMControl::filtermessage(Json::Value& jones)
//{
//    string atttype=jones["attachmentType"].asString();
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
//}

