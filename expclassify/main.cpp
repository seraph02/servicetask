#include <iostream>

#include <glog/logging.h>
#include <jsoncpp/json/json.h>
#include "jsoncpp/json/json.h"
#include "expclassify.h"
#include <boost/algorithm/string.hpp>
using namespace std;


int main(int argc, char* argv[])
{
    google::InitGoogleLogging((const char *)argv[0]);

    string logpath = "log/";
    if(access(logpath.c_str(),0)==-1)    {      system(("mkdir -p "+logpath).c_str());    }
    if(access(logpath.c_str(),0)==-1)    {      LOG(ERROR)<<logpath<<" create error"; return 0;    }

    google::SetStderrLogging(google::GLOG_INFO);
    google::SetLogDestination(google::GLOG_INFO, (logpath+"INFO_").c_str());
    FLAGS_colorlogtostderr = true;

    string strbody="";
    string strtype ="";
    string strkey="";
    string strid="";
    string strindices="";
    for(int i=0;i<argc;i++)
    {
        if(strcmp(argv[i],"-h")==0 )
        {
            cout<<"\n\t-h\t\t\thelp \n"<<
                  "\t-D or -d\t\tdaemon\n"<<endl;
            return 0;
        }
        if((strcmp(argv[i],"-B")==0 || strcmp(argv[i],"-b")==0)&&(i+1 <argc))
        {
            strbody=argv[i+1];i++;
//            cout<<strbody<<endl;
        }
        if((strcmp(argv[i],"-K")==0 ||strcmp(argv[i],"-k")==0)&&(i+1 <argc))
        {
            strkey=argv[i+1];i++;
//            cout<<strtype<<endl;
        }
        if((strcmp(argv[i],"-T")==0 ||strcmp(argv[i],"-t")==0)&&(i+1 <argc))
        {
            strtype=argv[i+1];i++;
//            cout<<strtype<<endl;
        }
        if((strcmp(argv[i],"-id")==0 ||strcmp(argv[i],"-ID")==0)&&(i+1 <argc))
        {
            strid=argv[i+1];i++;
//            cout<<strtype<<endl;
        }
        if((strcmp(argv[i],"-indices")==0)&&(i+1 <argc))
        {
            strindices=argv[i+1];i++;
//            cout<<strtype<<endl;
        }
    }
    LOG(INFO)<<" type: "<< strtype <<" key ";
    expclassify dc;

    boost::trim_left(strkey);//去掉字符串左边空格
    boost::trim_right(strkey);//去掉字符串右边空格
    Json::Value args;
    args["result"]=strbody;
    args["key"]=strkey;
    args["info"]=strtype;
    args["taskid"]=strid;
    if(!strindices.empty()) args["indices"]=strindices;
    else if(!strkey.empty()){string strtmp = strkey; boost::to_lower(strtmp);boost::replace_all(strtmp,"+","");args["indices"]="key"+strtmp;}

    dc.Go(args);
    return 0;
}
