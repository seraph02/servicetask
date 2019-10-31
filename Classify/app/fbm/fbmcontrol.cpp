#include "fbmcontrol.h"

#include "comm.h"
#include <glog/logging.h>
#include "appconf.h"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
string FBMControl::getkey(string data,string key)
{
    string keystr = key+"=(.*?), ";
    return getkey4reg(data,keystr);

}
string FBMControl::getkey4reg(string data,string regex)
{
    string keystr = regex;
    boost::regex reg(keystr);
    boost::smatch m;
    if (boost::regex_search(data,m,reg)) {
    if(m["val"].matched)
    {
        string ret(m["val"].str());
        return ret;
    }
    if (m[1].matched)
    {   string ret1(m[1].str());
        return ret1;
    }
    }


    return "";

}

//void FBMControl::work()
//{
//    GroupTxt();
//    absControl::work();

//}
//void FBMControl::GroupTxt()
//{
//    for(int fileite =0;fileite< files.size();fileite++)
//    {
//        string file = files[fileite];
//        string filename=getkey4reg(file,"([\\w]+)\.txt$");

//        if(filename.rfind("group")!=string::npos)
//        {
//            int linenum = getFiletotals(file);
//            for(int i =0;i<linenum;i++)
//            {
//                string bodystr=ReadLocalFile(file,i+1);
//                string userbodystr=this->getkey4reg(bodystr,"^User\\((.*)\\)");
//                bodystr=userbodystr;
//                string other=bodystr;
//                vector<string> strsubarray =vector<string>();
//                SplitString(bodystr.c_str(),strsubarray,", ");
//                if(strsubarray.size()==17)
//                {
//                    for(int it=0;it<strsubarray.size();it++)
//                    {
//                        string keystr=getkey4reg(strsubarray[it],"(.*?)=");
//                        string valuestr=getkey4reg(strsubarray[it],"(='(?<val>.*)')|(=(?<val>.*))");
//                        cout<<keystr<<" "<<valuestr<<endl;
//                    }
//                }
//            }
//        }
//    }
//}
void FBMControl::ProcessGroup(Json::Value jbody)
{
    if(jbody.size()<1) return;
    for(int ind =0;ind<jbody.size();ind++)
    {
        Json::Value jones = jbody[ind];
        if(jones.isNull() || !jones.isObject()) continue;
        filteravatar(jones);
        jbody[ind]=jones;
//        Json::FastWriter jwrite;
//        Json::Value jgroup;
//        jgroup["type"]="group";
//        //jgroup["tag"]= dt;
//        jgroup["body"]=jones;
//        string strgroup = jwrite.write(jgroup);
//        rst.putjson(strgroup);
    }
    Json::Value jelement;
    jelement["type"]="group";
    jelement["tag"]= dt;
    jelement["body"]=jbody;
    Json::FastWriter jwrite;
    string strcontacts = jwrite.write(jelement);
    rst.putjson(strcontacts);
}
void FBMControl::ProcessFile(string file, string filename, string fileext)
{
    if(fileext.rfind("txt") != string::npos)
    {
        Json::Value jarray;
        try
        {
            int linenum = getFiletotals(file);

            if(filename.rfind("contacts")!=string::npos ||filename.rfind("contact")!=string::npos)
            {
                for(int i=0;i<linenum;i++)
                {

                    Json::Value jfile;
                    string bodystr=ReadLocalFile(file,i+1);
                    string userbodystr=this->getkey4reg(bodystr,"^User\\((.*)\\)");
                    bodystr=userbodystr;
                    string other=bodystr;
                    vector<string> strsubarray =vector<string>();
                    SplitString(bodystr.c_str(),strsubarray,", ");
                    if(strsubarray.size()==17)
                    {
                        for(int it=0;it<strsubarray.size();it++)
                        {
                            string keystr=getkey4reg(strsubarray[it],"(.*?)=");
                            string valuestr=getkey4reg(strsubarray[it],"(='(?<val>.*)')|(=(?<val>.*))");
                            jfile[keystr]=valuestr;
                        }
                    }
                    else
                    {
                        string uid,type,photo,name,url,first_name,last_name,gender,nickname,own_nickname,is_friend;
                        uid=this->getkey(bodystr,"uid");
                        type=this->getkey(bodystr,"type");
                        photo=this->getkey(bodystr,"photo");
                        name=this->getkey(bodystr,"name");
                        url=this->getkey(bodystr,"url");
                        first_name=this->getkey(bodystr,"first_name");
                        last_name=this->getkey(bodystr,"last_name");
                        gender=this->getkey(bodystr,"gender");
                        nickname=this->getkey(bodystr,"nickname");
                        own_nickname=this->getkey(bodystr,"own_nickname");
                        is_friend=this->getkey(bodystr,"is_friend");

                        jfile["uid"]=uid;
                        jfile["type"]=type;
                        jfile["photo"]=photo;
                        jfile["name"]=name;
                        jfile["url"]=url;
                        jfile["first_name"]=first_name;
                        jfile["last_name"]=last_name;
                        jfile["gender"]=gender;
                        jfile["nickname"]=nickname;
                        jfile["own_nickname"]=own_nickname;
                        jfile["is_friend"]=is_friend;

                    }
                    jfile["other"]=other;
                    jarray.append(jfile);

                }
                ProcessContacts(jarray);
            }
            else if(filename.rfind("group")!=string::npos)
            {

                Json::Value jfile;
                int linenum = getFiletotals(file);
                for(int i =0;i<linenum;i++)
                {
                    string bodystr=ReadLocalFile(file,i+1);
                    string userbodystr=this->getkey4reg(bodystr,"^User\\((.*)\\)");
                    bodystr=userbodystr;
                    string other=bodystr;
                    vector<string> strsubarray =vector<string>();
                    SplitString(bodystr.c_str(),strsubarray,", ");
                    if(strsubarray.size()==17)
                    {
                        for(int it=0;it<strsubarray.size();it++)
                        {
                            string keystr=getkey4reg(strsubarray[it],"(.*?)=");
                            string valuestr=getkey4reg(strsubarray[it],"(='(?<val>.*)')|(=(?<val>.*))");
                            //cout<<keystr<<" "<<valuestr<<endl;
                            jfile[keystr]=valuestr;
                        }
                    }
                    jfile["other"]=other;
                    jarray.append(jfile);
                }
                ProcessGroup(jarray);

            }
            else// if(filename.rfind("message")!=string::npos)
            {
                for(int i=0;i<linenum;i++)
                {
                    Json::Value jfile;
                    string bodystr=ReadLocalFile(file,i+1);
                    string other=bodystr;
                    string text="";
                    string sender="";
                    string receiver="";
                    //string type="";
                    string date="";
                    string mid="";
                    string attachments="";


                    text=this->getkey(bodystr,"text");
                    boost::replace_all(text,"'","");
                    if(text == "None") text="";
                    sender=this->getkey(bodystr,"author");
                    boost::replace_all(sender,"'","");
                    receiver=filename.substr(0,filename.rfind(fileext));

                    attachments=this->getkey4reg(bodystr,"attachments=\\[(.*\?)\\]");
//                    if(filename.rfind("3157")!=string::npos)
//                    {
//                        cout<<filename<<endl;
//                        cout<<bodystr<<endl;
//                    }
                    string type="";
                    string url="";
                    if(!attachments.empty())
                    {
                        //cout<<attachments<<endl;
                        if(attachments.find("ImageAttachment")!=string::npos)
                        {
                            url=getkey(attachments,"preview_url");
                        }
                        else
                            url=getkey(attachments,"url");
                        boost::replace_all(url,"'","");
                        type=attachments.substr(0,attachments.find("("));

                        if(!url.empty()&&attachments.find("ShareAttachment")==string::npos){

                        //down avatar file
                        string path_dir = APPCONF::getInstance()->refpath +"app/fbm/"+app->getkey()+"/"+ type +"/";
                        checkdir(path_dir);
                        string fileext=getfileext(url);
                        string path= path_dir + filename4time(fileext);
                        try
                        {
                            //cout<<url<<endl;

                            if(DOWNLOAD_FILE(url.c_str(),path.c_str())==0)
                            {
                                jfile["localurl"]=(GetCurrPath()+path).c_str();
                            }
                        }
                        catch(...)
                        {

                        }
                        }
                    }
                    //type="";
                    mid=this->getkey(bodystr,", uid");
                    boost::replace_all(mid,"'","");
                    date=this->getkey(bodystr,"timestamp");
                    boost::replace_all(date,"'","");

                    if(!sender.empty()) jfile["senderid"]=sender;
                    if(!receiver.empty()) jfile["receiverid"]=receiver;

                    if(!text.empty())
                    {
                        jfile["text"]=text;
                        if(type.empty()) type="text";
                    }
                    if(!type.empty()) jfile["type"]=type;
                    //jfile["docid"]=mid+sender;
                    if(!mid.empty()) jfile["docid"]=mid;
                    if(!mid.empty()) jfile["msgid"]=mid;
                    if(!date.empty()) jfile["date"]=date;
                    if(!url.empty()) jfile["url"]=url;
                    jfile["other"]=other;
                    jarray.append(jfile);

                }
                ProcessMessage(jarray);
            }

        }
        catch(exception &proe)
        {
            LOG(ERROR)<<proe.what();
            return;
        }
    }

}

void FBMControl::filteravatar(Json::Value& jones)
{
    try
    {
    if(jones.isMember("photo"))
    {
        string avatar=jones["photo"].asString();
        //cout<<avatar<<endl;
        if(avatar.empty()){return;}
        string atttype="avatar";
        //down avatar file
        string path_dir = APPCONF::getInstance()->refpath +"app/fbm/"+app->getkey()+"/"+ atttype +"/";
        checkdir(path_dir);
        string fileext=getfileext(avatar);
        string path= path_dir + filename4time(fileext);
        if(DOWNLOAD_FILE(avatar.c_str(),path.c_str())==0)
        {
            jones["url"]=(GetCurrPath()+path).c_str();
        }
    }
    }
    catch(...)
    {

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


