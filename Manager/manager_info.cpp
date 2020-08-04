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
Manager_Info::~Manager_Info()
{

}
void Manager_Info::run()
{

//    LOG(INFO)<<"selfchk heartbeat "<<endl;
    //nodeid is null set off
        CStatus adb;
#ifdef ADB
       adb.status=On;
#else
        adb.status =health->b_dev->nodeid().size()<2?Off:On;
#endif
        MyHealth::getInstance()->SetState(Adb,adb.status);
#ifdef NETDISK
        MyHealth::getInstance()->SetState(NetDisk,On);
#else
        CStatus netdisk;
        Manager_Info::getInstance()->GetNetDiskInfo(&netdisk);
        MyHealth::getInstance()->SetState(NetDisk,netdisk.status);
#endif
        CStatus netinfo;
        //get net info
        try
        {
            Manager_Info::getInstance()->GetNetInfo(&netinfo);
            MyHealth::getInstance()->SetState(Local,netinfo.status);
        }
        catch(exception & e)
        {
           LOG(ERROR)<<e.what();
           return ;
        }
        if(adb.status == On )
        {
            CStatus es;
            Manager_Info::getInstance()->GetESInfo(&es);
            MyHealth::getInstance()->SetState(ES,es.status);
    //set devinfo 2 es devices/info/
            try
            {
                if(MyHealth::getInstance()->CheckStatus(ES))
                {
                    DevInfo retdev,changedev;
                    Manager_Info::getInstance()->GetDevInfo(&retdev);
                    //remote change
                    {
                        //remote flag
                        bool remote_flag_off = ::CheckStatus(Offline,retdev.flag());
                        bool locale_flag_off = ::CheckStatus(Offline,MyHealth::getInstance()->b_dev->flag());
                        if(remote_flag_off != locale_flag_off)
                        {
                            MyHealth::getInstance()->SetState(Offline,remote_flag_off?On:Off);
                        }
                    }



                    time_t time_now; time(&time_now);
                    changedev.set_etime(time_now);
                    long ltime = retdev.etime();
                    string ip_str = MyHealth::getInstance()->b_dev->ip();
                    string job_str = MyHealth::getInstance()->b_dev->jobs();
                    if(retdev.ip().compare(MyHealth::getInstance()->b_dev->ip()) !=0 ){changedev.set_ip(ip_str);}
                    if(retdev.jobs().compare(MyHealth::getInstance()->b_dev->jobs()) !=0 ){changedev.set_jobs(job_str);}

                    std::tm tmtoday = gettm(time_now*1000);
                    std::tm tm = gettm(ltime*1000);
                    if(tm.tm_year==tmtoday.tm_year&&tm.tm_mon==tmtoday.tm_mon&&tm.tm_mday==tmtoday.tm_mday)  {        }
                    else
                    {
                       changedev.set_json("");
                    }
                    string putstrjson = pb2json(changedev);
                    Manager_ES::getInstance()->UpdateDevInfo(MyHealth::getInstance()->b_dev->nodeid(),putstrjson);



                }
            }
            catch(exception &e)
            {
                LOG(ERROR)<<e.what();
            }

    #ifdef PROXYOK
            MyHealth::getInstance()->SetState(Proxy,On);
    #else
            CStatus proxy;
            Manager_Info::getInstance()->GetProxyInfo(&proxy);
            MyHealth::getInstance()->SetState(Proxy,proxy.status);
            if(proxy.status==On) proxystatus.timeoutcount =0;
            if(proxystatus.timeoutcount>=MAX_TIMEOUT)
            {
                //proxystatus.id = proxystatus.id>MAX_PROXYCOUNT?0:proxystatus.id+1;
                //ChangeProxy(proxystatus.id);
                proxystatus.timeoutcount = 0;
            }
            else
            {
 //               LOG(ERROR)<<"proxy timeout "<<proxystatus.timeoutcount;
                proxystatus.timeoutcount++;
                
            }
                     

    #endif

        }
        else
        {
            MyHealth::getInstance()->SetState(Proxy,Off);
            MyHealth::getInstance()->SetState(ES,Off);
        }
}
void Manager_Info::Update(int state)
{
    int tstatus = GetFlag();
    if(tstatus!=status){
        status = tstatus;
        try{
        DevInfo* changestatus = new DevInfo;
        changestatus->set_flag(status);
        string putstrjson = pb2json(*changestatus);
        Manager_ES::getInstance()->UpdateDevInfo(MyHealth::getInstance()->b_dev->nodeid(),putstrjson);
        }
        catch(...){}
    }
    bool isChange = CheckStatus(Dev);
    if(isChange)
    {
        DevInfo* info = new DevInfo;
        try
        {
            info->set_process(MyHealth::getInstance()->b_dev->process());
            info->set_complete(MyHealth::getInstance()->b_dev->complete());
            string putstrjson = pb2json(*info);
            Manager_ES::getInstance()->UpdateDevInfo(MyHealth::getInstance()->b_dev->nodeid(),putstrjson);
            MyHealth::getInstance()->SetState(Dev,Off);
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
    string strjson = MyHealth::getInstance()->b_dev->json();
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
    MyHealth::getInstance()->b_dev->set_json(strjson);
    LOG(INFO)<<strjson;

    DevInfo* info = new DevInfo;
    try
    {
    info->set_json(strjson);
    string putstrjson = pb2json(*info);
    Manager_ES::getInstance()->UpdateDevInfo(MyHealth::getInstance()->b_dev->nodeid(),putstrjson);

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
    CURLcode nRes =HttpGet(BaiDuURL,response,500);
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
    CURLcode nRes =HttpGet(GoogleURL,response,500);
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


//          ESInfo esinfo;
//          size_t size =strret.size();

          if(stresinfo.find("yellow")!= stresinfo.npos||stresinfo.find("green")!= stresinfo.npos)
          {
              info->status=On;
              ret = true;
          }
          else if(stresinfo.find("red")!= stresinfo.npos)
          {
              info->status=Off;
              ret =true;
          }
          else
          {
              ret =false;
          }
//          json2pb(esinfo,stresinfo.c_str());
//          if(esinfo.version().number().size()>1)
//          {
//              info->status=On;
//              ret = true;
//          }
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
    if(!CheckStatus(ES)) return false;
    if(MyHealth::getInstance()->b_dev->nodeid().empty()) return false;
    string strret="";
//get remote devinfo
    try
    {
        strret = Manager_ES::getInstance()->GetDevInfo(MyHealth::getInstance()->b_dev->nodeid());
        //LOG(INFO)<<"get remote devinfo"<<strret;
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
         if(JsonDev.isNull()||(JsonDev["_source"]["ip"]).isNull())
         {
//remote isnull, post dev
            string putstrjson = pb2json(*MyHealth::getInstance()->b_dev);
            LOG(INFO)<<"putstrjson"<<putstrjson;
            Manager_ES::getInstance()->UpdateDevInfo(MyHealth::getInstance()->b_dev->nodeid(),putstrjson);
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
                json2pb(*info,sinfo);
                //MyHealth::getInstance()->SetDevInfo(sinfo);

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
    MyHealth::getInstance()->b_dev->set_process(MyHealth::getInstance()->b_dev->process()+1);
    MyHealth::getInstance()->SetState(Dev,On);
}
void Manager_Info::DevComplete()
{
    MyHealth::getInstance()->b_dev->set_complete(MyHealth::getInstance()->b_dev->complete()+1);
    MyHealth::getInstance()->SetState(Dev,On);
}
void Manager_Info::DevError()
{
    MyHealth::getInstance()->b_dev->set_error(MyHealth::getInstance()->b_dev->error()+1);
    MyHealth::getInstance()->SetState(Dev,On);
}

void Manager_Info::ChangeProxy(int proxyid)
{
    try
    {
        /* code */
        string strcmd="";
        ostringstream ocmd;    
        ocmd << "java -jar " << CHANGEPROXYJARPATH << " -i=" << proxyid;
        strcmd = ocmd.str();
        string strret= RunShell(strcmd.c_str());
        LOG(INFO)<< strcmd << " => " << strret;
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << e.what() << '\n';
    }
    


}
