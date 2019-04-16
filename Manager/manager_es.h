#ifndef MANAGER_ES_H
#define MANAGER_ES_H
#include <string>
#include <vector>
#include "obmanager.h"

using namespace std;
class Manager_ES
{
public:
    bool IsAction();
    string GetESInfo();
    string GetDevInfo(string nodeid);
    string GetTaskInfo();
    void UpdateDevInfo(string nodeid,string putstrjson);
    void UpdateTaskInfo(string taskid,string putstrjson);
    bool POSTTaskResult(string indices,string strpostjson);
    virtual void Update(int status){}
    static Manager_ES* getInstance()
    {
        return esMNG;
    }
    static void ChangeHosts(vector<std::string> hosts)
    {
        m_hosts.clear();
        for(vector<std::string>::iterator it = hosts.begin(); it != hosts.end();it++)
        {
            m_hosts.push_back(*it);

        }

    }

private:
    static std::vector<std::string> m_hosts;
    static Manager_ES* esMNG;
};

#endif // MANAGER_ES_H
