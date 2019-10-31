#ifndef MANAGER_ES_H
#define MANAGER_ES_H
#include <string>
#include <vector>
#include "obmanager.h"
#include <elasticlient/bulk.h>

using namespace std;
class Manager_ES
{
public:
    bool IsAction();
    string GetESInfo();
    string GetDevInfo(string nodeid);
    string GetTaskInfo(string docid,int& statuscode);
    vector<string> GetNewTaskId();
    int UpdateTaskInfo(string taskid,string putstrjson);
    int UpdateTaskInfo(string taskid,string putstrjson,int version);
    void UpdateDevInfo(string nodeid,string putstrjson);

    bool createLock4taskid(string taskid,string ownid);
    bool deleteLock4taskid(string taskid);
    string getLock4taskid(string taskid);

    bool POSTTaskResult(string indices,string strpostjson);
    bool POSTTaskResult(string indices,string id,string strpostdata);

    elasticlient::SameIndexBulkData& POSTBulkStart(string indices);
    bool POSTBulkdata(string indices,string docid,string data);
    void POSTBulkend(elasticlient::SameIndexBulkData& bulkdata);
    virtual void Update(int status){}
    static Manager_ES* getInstance()
    {
        return esMNG;
    }
    static void AddHosts(std::string host)
    {
        m_hosts.push_back(host);
    }
    static void ChangeHosts(vector<std::string> hosts)
    {
        m_hosts.clear();
        for(vector<std::string>::iterator it = hosts.begin(); it != hosts.end();it++)
        {
            m_hosts.push_back(*it);

        }

    }
    void startbulk(string indices);

private:
    static std::vector<std::string> m_hosts;
    static Manager_ES* esMNG;
    long bulkcount=0;
    string bulkindices="";
    std::shared_ptr<elasticlient::SameIndexBulkData> bulk;
};

#endif // MANAGER_ES_H
