#include "manager_listen.h"
#include "manager_es.h"
#include "manager_task.h"
#include "chat_server.h"
#include "glog/logging.h"
#include <jsoncpp/json/json.h>
#include "dataclassify.h"
#include "Base64Encoder.h"
#include <boost/algorithm/string.hpp>
#include "taskinfo.pb.h"
#include "jsoncpp2pb.h"
#include <unistd.h>
#include "expclassify.h"


Manager_Listen* Manager_Listen::m_listenMNG = new Manager_Listen;
Manager_Listen::Manager_Listen()
{

}
void *Manager_Listen::thread_classify(void *threadid)
{
    while(true)
    {
        if(msglist.size()>0)
        {
            vector<string>tmp_msglist;
            tmp_msglist.swap(msglist);
            for (vector<string>::const_iterator iter = tmp_msglist.cbegin();iter != tmp_msglist.cend(); iter++)
            {
                //cout << (*iter) << endl;
                string msg = *iter;
                std::cout<<"msg("<<msg<<")"<<std::endl;

                Json::Value jmsg;
                try{
                    Json::Reader jread;
                    if(!jread.parse(msg,jmsg)||jmsg.isNull()){throw exception();}
                    //LOG(INFO)<<pyrst;
                    //LOG(INFO)<<jfile["id"];
                }
                catch(exception &e)
                {
                    LOG(ERROR)<<msg<<e.what();

                }
                string status = jmsg["status"].asString();
                string taskid = jmsg["task"].asString();
                string strkey = jmsg["phone"].asString();
                boost::replace_all(strkey,"+","");
                string indices = "key" + strkey;
                string strtype = jmsg["app_name"].asString();
                Base64Encoder b64;
                string strbody = b64.encode(msg);

                LOG(INFO)<<" type: "<< strtype <<" key "<<strkey<< " body: "<<strbody;
                DataClassify dc;
                dc.Go(strbody,strtype,strkey);


                expclassify exdc;

                boost::trim_left(strkey);//去掉字符串左边空格
                boost::trim_right(strkey);//去掉字符串右边空格
                Json::Value args;
                args["result"]=strbody;
                args["key"]=strkey;
                args["info"]=strtype;
                args["taskid"]=taskid;
                if(!strkey.empty()){string strtmp = strkey; boost::to_lower(strtmp);args["indices"]="key"+strtmp;}

                exdc.Go(args);
//                int msgcount=Manager_Task::getInstance()->PUSHRemoteDataCF(strtype,taskid,strkey,indices,strbody);
//                if(msgcount >=0 )
//                {
//                    int status = 0;
//            //task->t_task.set_datacount(msgcount);
//                    string task_s = Manager_ES::getInstance()->GetTaskInfo(taskid,status);
//                    Json::Value jsonRoot; Json::Reader reader;
//                    if (reader.parse(task_s, jsonRoot))
//                    {
//                        Json::Value jsontask = jsonRoot["_source"];
//                        Json::FastWriter jfw;
//                        task_s=jfw.write(jsontask);
//                        std::cout << task_s<< std::endl;
//                        TaskInfo t_task,*p_task;
//                        p_task = &t_task;
//                        TaskInfo change_task;
//                        json2pb(t_task,task_s);
//                        change_task.set_datacount(t_task.datacount()+msgcount);

//                        string putjson = pb2json(change_task);
//                        int intret =Manager_ES::getInstance()->UpdateTaskInfo(taskid,putjson);
//                    }
//                }
            }


        }
        else
        {
            sleep(2);
        }

    }
   pthread_exit(NULL);
}
void Manager_Listen::Init(int port)
{
    try
    {
      boost::asio::io_service io_service;

      chat_server_list servers;

      using namespace std; // For atoi.
      tcp::endpoint endpoint(tcp::v4(), port);
      chat_server_ptr server(new chat_server(io_service, endpoint));
      servers.push_back(server);

      pthread_t threads;
      int rc;
      rc = pthread_create(&threads, NULL,Manager_Listen::thread_classify, (void *)0);
      io_service.run();
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }
}

void Manager_Listen::RecvMsg(string ip,string msg,int len)
{
    std::cout<<ip<<"("<<len<<"):"<<msg<<std::endl;
    if(len > 4)
        msglist.push_back(msg);

}

