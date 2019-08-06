#include "esmanage.h"
#include <iostream>
#include <fstream>
#include <glog/logging.h>
#include <jsoncpp/json/json.h>
#include <comm.h>
#include <string>
#include "manager_es.h"
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


    std::ifstream fin("dataresult", std::ios::in);
    if(!fin)
    {
      printf("Read data from file faild, check if the file\n\
      %s\nexist","dataresult");
      return false;
    }
//    while(fin.getline(line, sizeof(line)))
//    {
//        std::stringstream word(line);
//        LOG(INFO)<<word.str();
//    }
    string line;
    int lineid=0;
    Manager_ES::getInstance()->ChangeHosts({"http://localhsot:9200/"});
    while (getline (fin, line)) // line中不包括每行的换行符
    {
        if(line.length()<3) continue;
        if(line[0]==',') line = line.substr(1);
        if(line[line.length()-1] == ']' )
                line.pop_back();
       LOG(INFO) <<lineid++<<":"<< line << endl;

       string strret = line;
       string info="telegram";
       string indices="telegramgroup";
       Json::Value jdataclassify;
       Json::Reader jread;
       jread.parse(strret,jdataclassify);
       if(!jdataclassify.isNull())
       {

               Json::Value jelement = jdataclassify;
               Json::Value jsondata;
               time_t time_now; time(&time_now);
               jsondata["spidedate"] = std::to_string(time_now);
               //jsondata["taskid"]=task->id();
               jsondata["task"]=info;
               string type =jelement["type"].isString()? jelement["type"].asString():"unknow";//message  or contacts
               jsondata[type]=jelement["body"];
               Json::FastWriter jfw;
               std::string strmessageid="";
               try{
                   //voxerid
                   string messageid = jelement["body"]["message_id"].asString();
                   strmessageid.append(messageid);
               }catch(...){}
               try{
                   //imo is no id
//                    string messageid = jelement["body"]["message_id"].asString();
//                    strmessageid.append(messageid);
               }catch(...){}
               std::string strpostdata=jfw.write(jsondata);
               if(strmessageid.empty()||strmessageid.size()<2) Manager_ES::getInstance()->POSTTaskResult(indices,strpostdata);
               else
                   Manager_ES::getInstance()->POSTTaskResult(indices,strmessageid,strpostdata);




       }
    }


    fin.close();
    return 0;
}
