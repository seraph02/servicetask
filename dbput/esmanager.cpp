#include "esmanager.h"
#include "manager_es.h"
#include <iostream>
#include <fstream>
#include <glog/logging.h>
#include <jsoncpp/json/json.h>
#include <comm.h>


#include "boost/algorithm/string/replace.hpp"
esmanager::esmanager()
{
    Manager_ES::getInstance();
    Manager_ES::getInstance()->AddHosts("http://localhost:9200/");
}
void Manager_ES::POSTBulkend(elasticlient::SameIndexBulkData& bulkdata)
{
    std::shared_ptr<elasticlient::Client> es = std::make_shared<elasticlient::Client>(
           std::vector<std::string>(m_hosts));
    elasticlient::Bulk bulkIndexer(es);
    size_t errors = bulkIndexer.perform(bulkdata);
    bulkdata.clear();
}
bool esmanager::POSTBulkdata(elasticlient::SameIndexBulkData& bulkdata,string type,string docid,string data)
{


    return 0;
}
int esmanager::Go()
{
    std::ifstream fin("dataresult", std::ios::in);
    if(!fin)
    {
      printf("Read data from file faild, check if the file\n\
      %s\nexist","dataresult");
      return 0;
    }
//    while(fin.getline(line, sizeof(line)))
//    {
//        std::stringstream word(line);
//        LOG(INFO)<<word.str();
//    }
    string line;
    int lineid=0;
    string info="telegram";
    string indices="telegramgroup";
    elasticlient::SameIndexBulkData blukdata(indices, 100);
    while (getline (fin, line)) // line中不包括每行的换行符
    {
        if(line.length()<3) continue;
        if(line[0]==',') line = line.substr(1);
        if(line[line.length()-1] == ']' )
                line.pop_back();
       LOG(INFO) <<lineid++<<":"<< line << endl;

       string strret = line;

       Json::Value jdataclassify;
       Json::Reader jread;
       Manager_ES::getInstance()->ChangeHosts(m_hosts);
       std::shared_ptr<elasticlient::Client> es = std::make_shared<elasticlient::Client>(
              std::vector<std::string>(m_hosts));
       elasticlient::Bulk bulkIndexer(es);
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
                   string messageid = jelement["body"]["id"].asString();
                   strmessageid.append(messageid);
               }catch(...){}
               try{
                   //imo is no id
//                    string messageid = jelement["body"]["id"].asString();
//                    strmessageid.append(messageid);
               }catch(...){}
               std::string strpostdata=jfw.write(jsondata);
               boost::algorithm::replace_all(strpostdata,"\n"," ");

               try
               {
               if(strmessageid.empty()||strmessageid.size()<2)
               {

                   LOG(INFO)<<"";//Manager_ES::getInstance()->POSTTaskResult(indices,"",strpostdata);
               }
               else
               {
                   blukdata.indexDocument("data",strmessageid, strpostdata);
                   bulkcount++;
                   if(bulkcount<10000) ;
                   else
                   {


                       size_t errors = bulkIndexer.perform(blukdata);
                       if(errors!=0)
                       {
                           LOG(INFO)<<errors;
                       }


                       blukdata.clear();
                       bulkcount=0;
                   }
               }

                }
               catch(exception &e)
               {
                    LOG(ERROR)<<strpostdata<<e.what();
               }



       }
    }
     Manager_ES::getInstance()->POSTBulkend(blukdata);

    fin.close();
    return 0;
}
