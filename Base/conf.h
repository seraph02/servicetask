#ifndef CONF_H
#define CONF_H
#include "conf.pb.h"
#include "getconfig.h"
class Manager_conf
{
public:
    static SCPROTO::ConfInfo* getInstance()
    {
        return conf;
    }
private:
    static SCPROTO::ConfInfo* conf;
    static Manager_conf* confMNG;
};

#endif // CONF_H
