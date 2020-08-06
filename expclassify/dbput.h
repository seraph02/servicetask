#ifndef DBPUT_H
#define DBPUT_H

#include "string"
#include <vector>
using std::string;
using std::vector;
class dbput
{
public:


    static dbput* getInstance()
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
    string GetESInfo();
    bool POSTTaskResult(string indices,string apptype,string id,string strpostdata);
    bool POSTTaskResult(string indices,string apptype,string strpostdata);
    string GetTaskInfo(string docid,int& statuscode);
    int UpdateTaskInfo(string taskid,string putstrjson,int version);
    int UpdateTaskInfo(string taskid,string putstrjson);
private:
    string getJson2md5(string strpostdata,string apptype);
private:
    static std::vector<std::string> m_hosts;
    static dbput* esMNG;
    long bulkcount=0;
    string bulkindices="";

};

#endif // DBPUT_H
