#include "imocontrol.h"
#include "comm.h"
#include <glog/logging.h>
#include "appconf.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
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
        try
        {
            if(DOWNLOAD_FILE(avatar.c_str(),path.c_str())==0)
            {
                jones["avatarUrl"]=(GetCurrPath()+path).c_str();
            }
        }
        catch(...)
        {

        }

    }
}
void IMOControl::ProcessMessage(Json::Value jbody)
{
    Json::Value jones_change;
    for(int ind =0;ind<jbody.size();ind++)
    {
        Json::Value jones = jbody[ind];
        if(jones.isNull() || !jones.isObject()) continue;
        Json::Value msgtime  = jones["date"];
        if(!msgtime.isNull())
        {
//            LOG(INFO)<<"--------------------";
//            LOG(INFO)<<msgtime.asString();
            boost::smatch mat; // 匹配结果
            boost::regex reg("^[\\d]{10}"); // 匹配非数字
            string body = msgtime.asString().c_str();
            boost::regex_search(body,mat,reg);
            string str = string(*mat.begin());
            int timesp = atoi(str.c_str());
            //LOG(INFO)<<timesp;
            jones["date"] = timesp;
            jones_change.append(jones);
        }
    }
//    Json::FastWriter jwrite;
//    string strbody = jwrite.write(jones_change);
//    LOG(INFO)<<strbody;

    absControl::ProcessMessage(jones_change);
    return;
}

