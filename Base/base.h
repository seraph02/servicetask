#ifndef BASE_H
#define BASE_H

#include <exception>
#include "glog/logging.h"
#if __amd64 ==1
#define PROXYOK 0
#endif
#define MAXRETRY 3

enum EType
{
    AdbStatus,
    BaseStatus,
    ProxyStatus,
    ESStatus,
    NetDiskStatus,
    DevStatus,


};
enum EStatus
{
    Off,
    On
};
struct CStatus
{
    EStatus status;

};
#endif // BASE_H
