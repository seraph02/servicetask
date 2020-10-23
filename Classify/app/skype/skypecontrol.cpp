#include "skypecontrol.h"

#include "comm.h"
#include <glog/logging.h>
#include "appconf.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>



void SKYPEControl::ProcessFile(string file, string filename, string fileext)
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
                //string other=bodystr;
                //LOG(INFO)<<bodystr;
                string strcmd = bodystr;
                string pyrst = strcmd;
                try{
                    Json::Reader jread;
                    if(!jread.parse(pyrst,jfile)||jfile.isNull()){throw exception();}
                    //LOG(INFO)<<pyrst;
                    //LOG(INFO)<<jfile["id"];
                }
                catch(exception &e)
                {

                }

                //jfile["other"]=other;
                jarray.append(jfile);

            }
            ProcessContacts(jarray);
            LOG(INFO)<<"contacts len: "<<jarray.size()<<"\t("<< filename<<")";
        }
        else if(filename.rfind("_chat")!=string::npos)
        {
            for(int i=0;i<linenum;i++)
            {
                Json::Value jfile;
                string bodystr=ReadLocalFile(file,i+1);
                //string other=bodystr;
                //LOG(INFO)<<bodystr;
                string strcmd = bodystr;
                string pyrst = strcmd;
                try{
                    Json::Reader jread;
                    if(!jread.parse(pyrst,jfile)||jfile.isNull()){throw exception();}
                    //LOG(INFO)<<pyrst;
                    //LOG(INFO)<<jfile["id"];
                }
                catch(exception &e)
                {

                }
                Json::Value content=jfile["content"];
                if(content.asString().rfind("type=\"")!=string::npos)
                {
                    boost::smatch mat; // 匹配结果
                    boost::regex reg("type=\"(.*?)\""); // 匹配非数字
                    string body = content.asString().c_str();
                    boost::regex_search(body,mat,reg);
                    string str = string(mat[1]);
                    vector<string> tp;
                    SplitString(str,tp,".");
                    if(tp.size()>1)
                    {
                        str = tp.front();
                    }
                    jfile["type"]=str;

                }

                //jfile["other"]=other;
                jarray.append(jfile);

            }
            ProcessMessage(jarray);
            LOG(INFO)<<"Msg len: "<<jarray.size()<<"\t("<< filename<<")";
        }


    }
    catch(exception &e)
    {

    }


    return;
}

void SKYPEControl::ProcessMessage(Json::Value jbody)
{
    Json::Value jones_change;
    for(int ind =0;ind<jbody.size();ind++)
    {
        Json::Value jones = jbody[ind];
        if(jones.isNull() || !jones.isObject()) continue;
        Json::Value msgtime  = jones["time"];
        if(!msgtime.isNull())
        {
//            LOG(INFO)<<"--------------------";
//            LOG(INFO)<<msgtime.asString();
            boost::smatch mat; // 匹配结果
            boost::regex reg("[\\d]{10}"); // 匹配非数字
            string body = msgtime.asString().c_str();
            boost::regex_search(body,mat,reg);
            string str = string(*mat.begin());
            int timesp = atoi(str.c_str());
            //LOG(INFO)<<timesp;
            jones["messageTime"] = timesp;
            jones_change.append(jones);
        }
    }
//    Json::FastWriter jwrite;
//    string strbody = jwrite.write(jones_change);
//    LOG(INFO)<<strbody;

    absControl::ProcessMessage(jones_change);
    return;
}


