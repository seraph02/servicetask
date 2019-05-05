//#include <regex>
#include <unistd.h>
#include "manager_es.h"
#include "manager_info.h"
#include "comm.h"
#include "conf.h"
#include "curl/curl.h"
#include <jsoncpp2pb.h>
#include<list>

using google::protobuf::Message;
using namespace SCPROTO;

Manager_Info* Manager_Info::infoMNG = new Manager_Info;
MyHealth* Manager_Info::health = new MyHealth;
Manager_Info::~Manager_Info()
{
}
void Manager_Info::run()
{

//    LOG(INFO)<<"selfchk heartbeat "<<endl;
    //nodeid is null set off
        CStatus adb;
        adb.status =health->b_dev->nodeid().size()<2?Off:On;
        health->SetState(AdbStatus,adb.status);
        CStatus netinfo;
        //get net info
        try
        {
            Manager_Info::getInstance()->GetNetInfo(&netinfo);
            health->SetState(BaseStatus,netinfo.status);
        }
        catch(exception & e)
        {
           LOG(ERROR)<<e.what();
           return ;
        }
        if(adb.status == On && netinfo.status==On)
        {
            CStatus es;
            Manager_Info::getInstance()->GetESInfo(&es);
            health->SetState(ESStatus,es.status);
    //set devinfo 2 es devices/info/
            try
            {
                if(health->CheckStatus(ESStatus))
                {

                    Manager_Info::getInstance()->GetDevInfo(health->b_dev);
                    DevInfo changedev;
                    time_t time_now; time(&time_now);
                    changedev.set_etime(time_now);
                    long ltime = this->health->b_dev->etime();

                    std::tm* tmtoday = gettm(time_now*1000);
                    std::tm* tm = gettm(ltime*1000);
                    if(tm->tm_year==tmtoday->tm_year&&tm->tm_mon==tmtoday->tm_mon&&tm->tm_mday)  {        }
                    else
                    {
                       changedev.set_json("");
                    }
                    string putstrjson = pb2json(changedev);
                    Manager_ES::getInstance()->UpdateDevInfo(health->b_dev->nodeid(),putstrjson);

                }
            }
            catch(exception &e)
            {
                LOG(ERROR)<<e.what();
            }

    #ifdef PROXYOK
            health->SetState(ProxyStatus,On);
    #else
            CStatus proxy;
            Manager_Info::getInstance()->GetProxyInfo(&proxy);
            health->SetState(ProxyStatus,proxy.status);

    #endif
            CStatus netdisk;
            Manager_Info::getInstance()->GetNetDiskInfo(&netdisk);
            health->SetState(NetDiskStatus,netdisk.status);
        }
        else
        {
            health->SetState(ProxyStatus,Off);
            health->SetState(ESStatus,Off);
            health->SetState(NetDiskStatus,Off);
        }
}
void Manager_Info::Update(int state)
{
    status = state;
    bool isChange = CheckStatus(DevStatus);
    if(isChange)
    {
        DevInfo* info = new DevInfo;
        try
        {
        info->set_process(health->b_dev->process());
        info->set_complete(health->b_dev->complete());
        string putstrjson = pb2json(*info);
        Manager_ES::getInstance()->UpdateDevInfo(health->b_dev->nodeid(),putstrjson);
        health->SetState(DevStatus,Off);
        }
        catch(exception &e)
        {
            LOG(ERROR)<<e.what();
        }
        delete info;
    }
}
void Manager_Info::Updatetasklist(string appname)
{
    string strjson = this->health->b_dev->json();
    Json::Reader jsonread;
    Json::Value jsonlist;


    bool isok = jsonread.parse(strjson,jsonlist);
    bool isfind=false;
    if(isok&&jsonlist.isArray())
    {
        if(jsonlist.size()>0)
        {
          for(int i=0;i<jsonlist.size();i++)
          {
            Json::Value job = jsonlist[i];
            if(!job.isNull()&&!job[appname].isNull()&&job[appname].isInt())
            {
                int appcount = job[appname].asInt();
                job[appname] =  appcount+1;
                jsonlist[i]=job;
                isfind =true;
                break;
            }
          }
        }
    }
    if(!isfind)
    {
        Json::Value job;
        job[appname]=1;
        jsonlist.append(job);
    }
    strjson = jsonlist.toStyledString();
    this->health->b_dev->set_json(strjson);
    LOG(INFO)<<strjson;

    DevInfo* info = new DevInfo;
    try
    {
    info->set_json(strjson);
    string putstrjson = pb2json(*info);
    Manager_ES::getInstance()->UpdateDevInfo(health->b_dev->nodeid(),putstrjson);

    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what();
    }
}
size_t receive_data(void *contents, size_t size, size_t nmemb, void *stream){
    string *str = (string*)stream;
    (*str).append((char*)contents, size*nmemb);
    return size * nmemb;
}
CURLcode HttpGet(const std::string & strUrl, std::string & strResponse,int nTimeout){
    CURLcode res;
    CURL* pCURL = curl_easy_init();

    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(pCURL, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, nTimeout);
    curl_easy_setopt(pCURL, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, receive_data);
    curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void*)&strResponse);
    res = curl_easy_perform(pCURL);
    curl_easy_cleanup(pCURL);
    return res;
}

bool Manager_Info::GetNetInfo(CStatus* info)
{

    bool ret = false;

    try
    {


    string response;
    CURLcode nRes =HttpGet(BaiDuURL,response,300);
    if(nRes==CURLE_OK)
    {
      ret = true;
      info->status=On;
    }


    }
    catch (exception& e)
    {
        LOG(ERROR) << e.what();
        cout<<e.what()<<endl;
    }

    return ret;


}

bool Manager_Info::GetProxyInfo(CStatus* info)
{
    info->status=Off;
    bool ret = false;
  try
    {

    string response;
    CURLcode nRes =HttpGet(GoogleURL,response,300);
    if(nRes==CURLE_OK)
    {
      ret = true;
      info->status=On;
    }


    }
    catch(exception &e)
    {
      LOG(ERROR)<<e.what();
    }
    return ret;

}


bool Manager_Info::GetESInfo(CStatus* info)
{
    bool ret = false;
    info->status = Off;
        std::string stresinfo;
        try
        {
            if(!(Manager_ES::getInstance()->IsAction()))
            {
                return ret;
            }
            stresinfo = Manager_ES::getInstance()->GetESInfo();

        }
        catch(exception& e)
        {
            LOG(ERROR)<<"CURL ERROR :"<<e.what();
//            cout<<"CURL ERROR :"<<e.what()<<endl;
        }


        try
        {


          ESInfo esinfo;
//          size_t size =strret.size();
          json2pb(esinfo,stresinfo.c_str());
          if(esinfo.version().number().size()>1)
          {
              info->status=On;
              ret = true;
          }
        }
        catch(exception& e)
        {
          LOG(ERROR)<<e.what()<<stresinfo;

        }


        return ret;

}
Json::Value GetJsonDev(string src)
{
//    json_error_t err;
    Json::Value jsonRoot; //定义根节点

    string jsonStr = src; //json字符串
    jsonRoot.clear();

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    JSONCPP_STRING errs;
    Json::CharReader* reader = builder.newCharReader();
    if (!reader->parse(jsonStr.data(), jsonStr.data() + jsonStr.size(), &jsonRoot, &errs)) //从jsonStr中读取数据到jsonRoot
    {
        return Json::nullValue;
    }
    Json::Value jsonfound = jsonRoot["found"];
    if(jsonfound.isNull()
            ||(!jsonfound.isNull()&&!jsonfound.asBool()))
    {
        return Json::nullValue;

    }


    delete reader;
    return jsonRoot;
}
bool Manager_Info::GetDevInfo(DevInfo* info)
{
    if(!CheckStatus(ESStatus)) return false;

    string strret="";
//get remote devinfo
    try
    {
        strret = Manager_ES::getInstance()->GetDevInfo(info->nodeid());
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what()<<"--"<<strret;
    }
//parse devinfo
    Json::Value jsondev;
    try
    {
         Json::Value JsonDev = GetJsonDev(strret);
         if(JsonDev.isNull())
         {
//remote isnull, post dev
            string putstrjson = pb2json(*info);
            Manager_ES::getInstance()->UpdateDevInfo(health->b_dev->nodeid(),putstrjson);
         }
         else
         {
            Json::Value source = JsonDev["_source"];
            if(source.isObject())
            {
                jsondev = source;
            }
         }
         if(jsondev.isObject())
         {
             string sinfo;
             try
             {
                sinfo = jsondev.toStyledString();
//merge and update
                health->SetDevInfo(sinfo);

             }
             catch(exception &source)
             {
                 LOG(ERROR)<<source.what()<<sinfo;
             }
         }
    }
    catch(exception& e)
    {
      LOG(ERROR)<<e.what()<<strret;

    }
    return false;


}

bool Manager_Info::GetNetDiskInfo(CStatus* info)
{
    ConfInfo* conf = Manager_conf::getInstance();
    bool ret = false;
    info->status=Off;
    string diskpath = conf->netdiskpath();
    if(diskpath.size()<1) return ret;
    if(access(diskpath.c_str(), 0 ) != -1)
    {
        ret = true;        info->status=On;
    }
    else
    {
        ret = true;        info->status=Off;
    }
    return ret;
}
void Manager_Info::DevProcess()
{
    this->health->b_dev->set_process(this->health->b_dev->process()+1);
    this->health->SetState(DevStatus,On);
}
void Manager_Info::DevComplete()
{
    this->health->b_dev->set_complete(this->health->b_dev->complete()+1);
    this->health->SetState(DevStatus,On);
}
void Manager_Info::DevError()
{
    this->health->b_dev->set_error(this->health->b_dev->error()+1);
    this->health->SetState(DevStatus,On);
}


