#include "imocontrol.h"
#include "comm.h"
#include <glog/logging.h>
#include "appconf.h"

void IMOControl::filteravatar(Json::Value& jones)
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
        DOWNLOAD_FILE(avatar.c_str(),path.c_str());
        jones["avatarUrl"]=(GetCurrPath()+path).c_str();

    }
}

