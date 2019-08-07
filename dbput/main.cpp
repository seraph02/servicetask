
#include "esmanager.h"
#include <iostream>
#include <glog/logging.h>
#include <string>

using namespace std;

int init(int argc, char* argv[])
{
    google::InitGoogleLogging((const char *)argv[0]);

    string logpath = "log/";
    if(access(logpath.c_str(),0)==-1)    {      system(("mkdir -p "+logpath).c_str());    }
    if(access(logpath.c_str(),0)==-1)    {      LOG(ERROR)<<logpath<<" create error"; return 0;    }

    google::SetStderrLogging(google::GLOG_INFO);
    google::SetLogDestination(google::GLOG_INFO, (logpath+"INFO_").c_str());
    FLAGS_colorlogtostderr = true;
    string strbody="";
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



    }

    return 0;

}
int main(int argc, char* argv[])
{

    init(argc,argv);

    esmanager esmng;
    esmng.Go();




    return 0;
}
