#include <iostream>
#include "dataclassify.h"
#include "getconfig.h"
#include <glog/logging.h>
#include <jsoncpp/json/json.h>
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
    string strq="";
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

    }
    LOG(INFO)<<" type: "<< strtype <<" key "<<strkey<< " body: "<<strbody;
    DataClassify dc;
    dc.Go(strbody,strtype,strkey);

    return 0;
}
