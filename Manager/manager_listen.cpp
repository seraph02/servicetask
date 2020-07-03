#include "manager_listen.h"
#include "manager_task.h"
#include "chat_server.h"
#include "glog/logging.h"
#include <jsoncpp/json/json.h>
#include "dataclassify.h"
#include "Base64Encoder.h"
#include <boost/algorithm/string.hpp>

Manager_Listen* Manager_Listen::m_listenMNG = new Manager_Listen;
Manager_Listen::Manager_Listen()
{

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

    int msgcount=Manager_Task::PUSHRemoteDataCF(strtype,taskid,strkey,indices,strbody);
    if(msgcount >=0 )
    {
//task->t_task.set_datacount(msgcount);
    }
}

