#ifndef MANAGER_LISTEN_H
#define MANAGER_LISTEN_H
#include <comm.h>
using namespace std;

class Manager_Listen
{
public:
    Manager_Listen();
    void Init(int port);
    static Manager_Listen* getInstance()
    {
        return m_listenMNG;
    }
    void RecvMsg(string ip, string msg, int len);



public:
    static Manager_Listen* m_listenMNG;
};

#endif // MANAGER_LISTEN_H
